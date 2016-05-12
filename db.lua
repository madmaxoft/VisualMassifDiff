-- db.lua

-- Support library for both vgdb.lua and valgrind.lua, for SQLite DB access





local lsqlite = require("lsqlite3")
local os = os





local _pkg = {}





--- Executes the SQL statement, substituting "?" in the SQL with the specified params
-- Calls a_Callback for each row
-- The callback receives a dictionary table containing the row values (stmt:nrows())
-- The RowID callback receives the RowID of the inserted row (if applicable)
-- Returns false and error message on failure, or true on success
function _pkg:executeStatement(a_SQL, a_Params, a_Callback, a_RowIDCallback)
	-- Check params:
	assert(self)
	assert(self.m_DB)
	assert(a_SQL)
	assert((a_Params == nil) or (type(a_Params) == "table"))
	assert(self.m_DB)
	assert((a_Callback == nil) or (type(a_Callback) == "function"))
	assert((a_RowIDCallback == nil) or (type(a_RowIDCallback) == "function"))
	
	-- Prepare the statement (SQL-compile):
	local Stmt, ErrCode, ErrMsg = self.m_DB:prepare(a_SQL)
	if (Stmt == nil) then
		ErrMsg = (ErrCode or "<unknown>") .. " (" .. (ErrMsg or "<no message>") .. ")"
		LOGWARNING("Cannot prepare SQL \"" .. a_SQL .. "\": " .. ErrMsg)
		LOGWARNING("  Params = {" .. table.concat(a_Params or {}, ", ") .. "}")
		return nil, ErrMsg
	end
	
	-- Bind the values into the statement:
	if (a_Params) then
		ErrCode = Stmt:bind_values(unpack(a_Params))
		if ((ErrCode ~= sqlite3.OK) and (ErrCode ~= sqlite3.DONE)) then
			ErrMsg = (ErrCode or "<unknown>") .. " (" .. (self.m_DB:errmsg() or "<no message>") .. ")"
			LOGWARNING("Cannot bind values to statement \"" .. a_SQL .. "\": " .. ErrMsg)
			Stmt:finalize()
			return nil, ErrMsg
		end
	end
	
	-- Step the statement:
	if not(a_Callback) then
		ErrCode = Stmt:step()
		if ((ErrCode ~= sqlite3.ROW) and (ErrCode ~= sqlite3.DONE)) then
			ErrMsg = (ErrCode or "<unknown>") .. " (" .. (self.m_DB:errmsg() or "<no message>") .. ")"
			LOGWARNING("Cannot step statement \"" .. a_SQL .. "\": " .. ErrMsg)
			Stmt:finalize()
			return nil, ErrMsg
		end
		if (a_RowIDCallback) then
			a_RowIDCallback(self.m_DB:last_insert_rowid())
		end
	else
		-- Iterate over all returned rows:
		for v in Stmt:nrows() do
			a_Callback(v)
		end
		
		if (a_RowIDCallback) then
			a_RowIDCallback(self.m_DB:last_insert_rowid())
		end
	end
	Stmt:finalize()
	return true
end





--- Calls the specified callback for each instance
-- The callback receives a table with members "Pid", "CmdLine" and "StartTime"
-- Returns true on success, false and optional message on failure
function _pkg:forEachInstance(a_Callback)
	assert(self)
	assert(type(a_Callback) == "function")
	
	return self:executeStatement(
		"SELECT * FROM ValgrindInstances",
		nil,
		a_Callback
	)
end





--- Returns a table describing the valgrind instance specified by its pid
-- Returns nil and optional error message on error
-- Returns empty table if instance not found
-- The table returned on success has "Pid", "ValgrindParams", "CmdLine" and "StartTime" members
function _pkg:getInstance(a_Pid)
	assert(self)
	assert(tonumber(a_Pid))
	
	local inst = {}
	local isSuccess, msg = self:executeStatement(
		"SELECT * FROM ValgrindInstances",
		nil,
		function (a_Values)
			inst = a_Values
		end
	)
	if not(isSuccess) then
		return nil, msg
	end
	
	return inst
end





--- If there is only a single Valgrind instance, returns its pid
-- Returns nil and optional message on failure (incl. multiple instances)
function _pkg:getSingleInstancePid()
	assert(self)
	
	local singlePid = -1
	local isSuccess, msg = self:executeStatement(
		"SELECT Pid FROM ValgrindInstances",
		nil,
		function (a_Values)
			if (singlePid == -1) then
				-- This is the first Pid encountered, remember it
				singlePid = a_Values.Pid
			else
				-- There are multiple Pids, set the output to "nil"
				singlePid = nil
				return false
			end
		end
	)
	if not(isSuccess) then
		return nil, msg
	end
	
	return singlePid
end





--- Initializes the module
-- Opens the DB and creates tables, if needed
-- Returns true on success, nil and optional message on failure
function _pkg:init()
	assert(self)
	
	-- Open the DB file:
	local DB, err = lsqlite.open("valgrind.sqlite")
	if not(DB) then
		return nil, err
	end
	
	-- Initialize the tables:
	DB:exec([[CREATE TABLE IF NOT EXISTS ValgrindInstances (
		Pid INTEGER PRIMARY KEY,
		ValgrindParams TEXT,
		CmdLine TEXT,
		TimeUnits TEXT,
		StartTime DATETIME
	)
	]])
	
	-- All OK
	self.m_DB = DB
	return true
end

	
	
	

--- Returns whether the Valgrind instance specified by its Pid is valid
-- Returns nil and optional message on failure
function _pkg:isValidInstance(a_Pid)
	assert(self)
	assert(tonumber(a_Pid))
	
	local isValid = false
	local isSuccess, msg = self:executeStatement(
		"SELECT Pid FROM ValgrindInstances WHERE Pid = ?",
		{a_Pid},
		function (a_Values)
			if (a_Values.Pid == a_Pid) then
				isValid = true
			end
		end
	)
	if not(isSuccess) then
		return nil, msg
	end
	
	return isValid
end





--- Inserts a new Valgrind instance to the DB
-- Returns the PID of the instance, as assigned by the DB
-- Returns nil and optional message on error
function _pkg:insertNewInstance(a_ValgrindParams, a_CmdLine, a_TimeUnits)
	assert(self)
	assert(type(a_ValgrindParams) == "string")
	assert(type(a_CmdLine) == "string")
	assert(type(a_TimeUnits) == "string")
	
	local pid = -1
	local isSuccess, msg = self:executeStatement(
		"INSERT INTO ValgrindInstances (ValgrindParams, CmdLine, TimeUnits, StartTime) VALUES (?, ?, ?, ?)",
		{
			a_ValgrindParams,
			a_CmdLine,
			a_TimeUnits,
			os.clock()
		},
		nil,
		function (a_RowID)
			pid = a_RowID
		end
	)
	if (not(isSuccess) or (pid < 0)) then
		return nil, msg
	end
	
	return pid
end





--- Returns whether if the Valgrind instance specified by its Pid is scheduled for termination
-- Returns nil and optional message on failure
function _pkg:shouldInstanceTerminate(a_Pid)
	assert(self)
	assert(tonumber(a_Pid))
	
	local isThere = false
	local isSuccess, msg = self:executeStatement(
		"SELECT * FROM ValgrindInstances WHERE Pid = ?",
		{a_Pid},
		function (a_Values)
			if (a_Values.Pid == a_Pid) then
				isThere = true
			end
		end
	)
	if not(isSuccess) then
		return nil, msg
	end
	
	return not(isThere)
end




--- Schedules the Valgrind instance specified by its Pid for termination
-- Returns true on success, nil and optional message on failure
function _pkg:terminateInstance(a_Pid)
	assert(self)
	assert(tonumber(a_Pid))

	return self:executeStatement(
		"DELETE FROM ValgrindInstances WHERE Pid = ?",
		{a_Pid}
	)
end





return _pkg




