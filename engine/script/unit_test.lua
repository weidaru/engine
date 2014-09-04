local fs = require("fs")
local option_parser = require("option_parser")

local dir = nil
local rule = option_parser.create_rule()
rule[{"--dir", "-d", "Set the directory to scan, the host will . Use absolute path!!"}] = 
function(_, v) 
	dir = v
end

if not pcall(rule.parse, rule, ...) then
	print(rule:to_string())
end
assert(dir~=nil, "Soruce Dir is nil!")

fs.scan_dir(dir, {"*.lua"}, 
function(filepath)
	print(string.format([[======Unit test for %s ======]], filepath))
	local result = pcall(dofile, filepath) 
	if result[1] then
		print([[======Unit test Succeed ======]])
	else
		print([[======Unit test Failed ======]])
		print(select(2, table.unpack(result)))
	end
	print("\n\n\n")
end)
