local option_parser = require("option_parser")

local function scan(dir, func) 
	local f = io.popen(string.format("find %s -name \"*.h\" -or -name \"*.hpp\" -or -name \"*.cpp\" -or -name \"*.hcc\""))
	f = f or io.popen(string.format("dir %s *.h *.hpp *.hh *.cpp *.cc /s/b", dir))
	assert(f, "Cannot list all the related files")
	
	for line in f:lines() do
		func(line)
	end
end

local source_dir
local dest

local rule = option_parser.create_rule()
rule[{"--source_dir", "-s", "Set the directory which needs scanning, all the .h .hpp .cpp .cc files will be scanned. Use absolute path!!"}] = 
function(_, v) 
	source_dir = v
end

rule[{"--dest", "-d", "The file path that will be gnerated."}] = 
function(_, v)
	dest = v
end

if not pcall(rule.parse, rule, ...) then
	print(rule:to_string())
end
assert(source_dir~=nil, "Soruce Dir is nil!")
assert(dest~=nil, "Dest Dir is nil!")

--Start scan
local buffer = {}
scan(source_dir, function(filepath)
	
end)
