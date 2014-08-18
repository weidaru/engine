local option_parser = require("option_parser")
local lexer = require("lexer")

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

--[[
Here is the rules for parsing.
The struct should be plain old c style except that it allows function declaration.
It should have no virtual things or any public private declaration.
Actually implementation may just use pattern matching.

<what-we-care> ::= <annotation-warp> <struct>
<annotation-warp> ::= <SOL> <space-tab> "//[[" <annotation> "]]//" <space-tab> <EOL>
<annotation> ::= "TypeInfo"
<comment> ::= "//" <text> <EOL> | "/*" <text> "*/"
<struct> ::= "struct" <blank> <word> <blank> "{" <members> "}" <blank> ";"
<blank> ::= "\t" <blank> | " " <blank> | "\n" <blank> | <comment> <blank> | " " | "\t" | "\n"
<spaces-tabs> ::= "\t" <spaces-tabs> | " " <spaces-tabs> | ""
<members> ::= <variable-dec> <blank> <members> | <function-dec> <members> | <blank>
<variable-dec> ::= <word> <blank> <word> ";"
<function-dec> ::= <word> <blank> <word> <blank> "(" <anything> ")" <anything> ";"		As we don't care about function, just match it loosely.
]]--

local function 

local function parse(context, lex) 
	lex:next
end

local function link(context)

end


--Execution block begins here
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

	local context = {}
	--[[
	Format
		--metadata for struct
		[
			{
				typename
				members = [{typename, name}]
				defined_file
				defined_line
			}
		]
	]]--
	
	--Start scan
	local buffer = {}
	scan(source_dir, 
	function(filepath)
		local f = assert(io.open(filepath, "r"))
		--Cache the file
		print("Start processing " .. filepath)
		local buffer = {}
		for line in f:lines() do
			table.insert(buffer, line)
		end
		f:close()
		
		local lex = lexer.create(table.concat(buffer))
		print("Parsing...")
		parse(context, lex)
		print("Linking...")
		link(context)

		print("Complete processing " .. filepath)
	end)
end






