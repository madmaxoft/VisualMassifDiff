-- valgrind.lua

-- This is a valgrind test harness that simulates some valgrind output on Windows, so that VisualMassifDiff can be tested there as well.

-- Example parameters: --tool=massif --time-unit=ms testProg.exe param1 --param2 --param3




local db = require("db")




-- Print a header to indicate we're a dummy:
print("This is a dummy valgrind emulator.")
print("It is used as a test harness for VisualMassifDiff on Windows.")





-- Initialize the DB backend:
local isSuccess, msg = db:init()
if not(isSuccess) then
	print("Failed to initialize the DB: " .. (msg or "<no message>"))
	os.exit(1)
end




-- Store the instance in the DB:
local pid
local cmdLine, valgrindParams
local timeUnits = "i"
for i = 1, #arg do
	local argv = arg[i]
	if (argv:sub(1, 13) == "--time-units=") then
		timeUnits = argv:sub(14)
	elseif (argv:sub(1, 1) ~= "-") then
		valgrindParams = table.concat(arg, " ", 0, i - 1)
		cmdLine = table.concat(arg, " ", i)
		break
	end
end
pid, msg = db:insertNewInstance(valgrindParams, cmdLine, timeUnits)
if not(pid) then
	print("Failed to insert new instance to DB: " .. (msg or "<no message>"))
	os.exit(1)
end





-- Simply keep the process running, until asked to terminate:
while not(db:shouldInstanceTerminate(pid)) do
	print("Valgrind simulator " .. pid .. " running.")
	
	-- Sleep for 10 seconds by pinging (there's no os.sleep() in Lua)
	os.execute("ping -n 11 localhost > NUL")
end




