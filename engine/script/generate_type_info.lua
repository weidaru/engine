local option_parser = require("option_parser")
local lexer = require("lexer")
local context_class = require("context")
local parser = require("parser")

local function get_os() 
	if package.config:sub(1,1)=="/" then
		return "unix"
	elseif package.config:sub(1,1)=="\\" then
		return "windows"
	end
end

--TODO: Lift this up for global use. 
local function scan(dir, func) 
	local f=nil
	if get_os()=="windows" then
		f = io.popen(string.format("dir %s\\*.h %s\\*.hpp %s\\*.cpp %s\\*.cc /s/b/a-s", dir, dir, dir, dir)) 
	elseif get_os()=="unix" then
		f = io.popen(string.format("find %s -name \"*.h\" -or -name \"*.hpp\" -or -name \"*.cpp\" -or -name \"*.cc\"", dir))
	end
	
	assert(f, "Cannot list related files")
	
	for line in f:lines() do
		func(line)
	end
end

do
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

	local context = context_class.new()
	
	--Start scan
	scan(source_dir, 
	function(filepath)
		local f = assert(io.open(filepath, "r"))
		--Cache the file
		local buffer = {}
		for line in f:lines() do
			table.insert(buffer, line)
		end
		f:close()
		
		local lex = lexer.new(table.concat(buffer))
		print("===========Parsing " .. filepath)
		parser.parse(context, lex)
		print("===========Parsing complete")
	end)
	
	print("===========Linking...")
	parser.link(context)
	print("===========Linking complete.")
end






