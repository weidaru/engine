local fs = require("fs")
local option_parser = require("option_parser")


local dir = nil
local rule = option_parser.new()
rule[{"--dir", "-d", "Set the directory to scan, the host will . Use absolute path!!"}] = 
function(_, v) 
	dir = v
end

if not pcall(rule.parse, rule, ...) then
	print(rule:to_string())
end
assert(dir~=nil, string.format("Soruce Dir is nil! Usage:\n%s", rule:to_string()))

local any_failure = false
fs.scan_dir(dir, {"*.lua"}, 
function(filepath)
	local filename = fs.get_last_entry(filepath)
	print(string.format([[======Unit test for %s ======]], filename))
	local traceback = ""
	local save_traceback = function(e)
		traceback = e .. "\n" .. debug.traceback()
	end
	local result = xpcall(dofile, save_traceback, filepath)
	if result then
		print([[======Unit test Succeed ======]])
	else
		any_failure = true
		print([[======Unit test Failed ======]])
		print(traceback)
	end
	print("\n\n")
end)
if any_failure then
	os.exit(false)
end
