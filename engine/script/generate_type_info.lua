local option_parser = require("option_parser")

local function get_os() 
	if package.config:sub(1,1)=="/" then
		return "unix"
	elseif package.config:sub(1,1)=="\\" then
		return "windows"
	end
end

local function scan(dir, func) 
	local f=nil
	if get_os()=="windows" then
		f = io.popen(string.format("dir %s\\*.h %s\\*.hpp %s\\*.cpp %s\\*.cc /s/b/a-s", dir, dir, dir, dir)) 
	elseif get_os()=="unix" then
		f = io.popen(string.format("find %s -name \"*.h\" -or -name \"*.hpp\" -or -name \"*.cpp\" -or -name \"*.cc\"", dir))
	end
	
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
scan(source_dir, 
function(filepath)
	local f = assert(io.open(filepath, "r"))
	--Read line by line
	for line in f:lines() do
		local annotation = line:match("[%s\t]*//%[%[[%s\t]*(%w+)[%s\t]*%]%]//[%s\t\n]*")
		if annotation == "TypeInfo" then
			print(filepath)
		end
	end
	f:close()
end)





