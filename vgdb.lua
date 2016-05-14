-- vgdb.lua

-- This is a vgdb test harness that simulates some vgdb output on Windows, so that VisualMassifDiff can be tested there as well.

-- Example parameters 1: --pid=1 detailed_snapshot test.massif
-- Example parameters 2: -l
-- Example parameters 3: --help




local db = require("db")
local os = os
local math = math





local function listInstances()
	db:forEachInstance(
		function (a_Values)
			assert(a_Values.Pid)
			assert(a_Values.CmdLine)
			
			print("use --pid=" .. a_Values.Pid .. " for " .. a_Values.ValgrindParams .. " " .. a_Values.CmdLine)
		end
	)
	return 1  -- "vgdb -l" returns a 1 on Linux
end





local function displayHelp()
	-- No extra help is needed, everything was already output in the header
	return 0
end





local function reportError()
	io.stderr:write("Unhandled parameter")
	return -2
end





--- Outputs a dummy snapshot into the specified file
-- a_Pid is the Pid of the instance to dump (checked valid)
-- a_FileName is the output file name
-- Returns the vgdb exit code
local function outputSnapshotFile(a_Pid, a_FileName)
	assert(tonumber(a_Pid))
	
	-- The filename defaults to "massif.vgdb.out":
	a_FileName = a_FileName or "massif.vgdb.out"
	
	local inst, msg = db:getInstance(a_Pid)
	if (not(inst) or not(inst.Pid)) then
		io.stderr:write("Unable to query valgrind pid " .. a_Pid .. ": " .. (msg or "<no message>"))
		return 1
	end
	
	-- Open the file:
	local f
	f, msg = io.open(a_FileName, "wb")
	if not(f) then
		io.stderr:write("Cannot open file " .. a_FileName .. " for writing: " .. (msg or "<no message>"))
		return 1
	end
	
	-- Output the header:
	f:write("desc: " .. inst.ValgrindParams .. "\n")
	f:write("cmd: " .. inst.CmdLine .. "\n")
	f:write("time_unit: " .. inst.TimeUnits .. "\n")
	f:write("#-----------\nsnapshot=0\n#-----------\n")
	f:write("time=" .. math.ceil(os.time() - inst.StartTime) .. "\n")

	-- Output the dummy snapshot:
	f:write([[
mem_heap_B=8000
mem_heap_extra_B=1000
mem_stacks_B=0
heap_tree=detailed
n3: 8000 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.
 n0: 1000 0x1000: TestFn1 (main.cpp:10)
 n0: 1000 0x2000: TestFn2 (main.cpp:20)
 n2: 6000 0x3000: TestFn3 (main.cpp:30)
  n0: 3000 0x1000: TestFn1() (main.cpp:10)
  n0: 3000 0x2000: TestFn2() (main.cpp:20)
]])

	f:close()
	print("Output file " .. a_FileName .. " has been written.")
	return 0
end





--- Handles the vgdb command verb
-- a_Args is the complete command line args
-- a_CmdIdx is the index into a_Args where the verb was found
-- a_Pid is the Pid that had been parsed from a_Args earlier (<0 if not available)
-- Returns the vgdb exit code
local function handleCommand(a_Args, a_CmdIdx, a_Pid)
	assert(type(a_Args) == "table")
	assert(tonumber(a_CmdIdx))
	assert(tonumber(a_Pid))
	
	-- If PID was not given, see if there is only a single instance:
	if (a_Pid < 0) then
		a_Pid = db:getSingleInstancePid()
		if not(a_Pid) then
			io.stderr:write("There are multiple instances of valgrind running, you need to specify --pid=<n> to select one")
			return 1
		end
	end
	
	-- If the instance is not valid, bail out:
	if not(db:isValidInstance(a_Pid)) then
		io.stderr:write("PID " .. a_Pid .. " is not a known instance")
	end
	
	local cmd = a_Args[a_CmdIdx]
	if (
		(cmd == "snapshot") or
		(cmd == "detailed_snapshot") or
		(cmd == "all_snapshots")
	) then
		-- The same behavior for all snapshotting commands - output a single detailed snapshot into the specified file:
		return outputSnapshotFile(a_Pid, a_Args[a_CmdIdx + 1])
	end
end





local function handleCmdLine(a_Args)
	assert(a_Args)
	local exitCode = 0
	local pid = -1
	for i = 1, #a_Args do
		local argv = a_Args[i]
		if (argv:sub(1, 1) == "-") then
			if (argv == "-l") then
				return listInstances()
			elseif (argv == "--help") then
				return displayHelp()
			elseif (argv:match("--pid=%d*")) then
				pid = argv:find("--pid=(%d)")
				if not(pid) then
					io.stderr:write("Invalid PID specification: " .. argv)
					return 1
				end
			else
				return reportError()
			end
		else
			return handleCommand(a_Args, i, pid)
		end
	end
	return exitCode or 0
end





--- Main entrypoint:

-- Print a header to indicate we're a dummy:
print("This is a dummy vgdb emulator.")
print("It is used as a test harness for VisualMassifDiff on Windows.")

-- Initialize the DB backend:
local isSuccess, msg = db:init()
if not(isSuccess) then
	io.stderr:write("Failed to initialize the DB: " .. (msg or "<no message>") .. "\n")
	os.exit(1)
end

-- Based on the arguments, decide what to do:
local exitCode = handleCmdLine(arg)
io.stderr:write("VGDB dummy exit code: " .. exitCode .. "\n")

-- Report the correct exit code:
os.exit(exitCode)




