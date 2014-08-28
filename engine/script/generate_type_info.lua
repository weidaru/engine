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

local function make_static(t) 
	local meta = {
		__newindex = function(t,k,v)
			assert("New member is not allowed for static table.")
		end
	}
	return setmetatable(t, meta)
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
<variable-dec> ::= <word> <blank> <word> <blank> ";"
<function-dec> ::= <word> <blank> <word> <blank> "(" <anything> ")" <anything> ";"		As we don't care about function, just match it loosely.
]]--

local function assert_help(expression, message, lex)
	if not expression then
		local buffer = {}
		table.insert(buffer, lex.filename)
		table.insert(buffer, " line: ")
		table.insert(buffer, lex.linenumber())
		table.insert(buffer, "\n")
		table.insert(buffer, message)
	end
end

local function parse_function_dec
	assert_help(lex:expect_word(), "Expect a typename ")
	lex:ignore_blank()
	assert_help(lex:expect_word(), "Expect a name ")
	lex:ignore_blank()
	--I am being lazy here. As in c/c++ parameter declaration will contain no parenthesis, I simply try to find the pair of "(" ")"
	assert_help(lex:expect("("), "Expect a ( ")
	assert_help(lex:next(")", "Cannot find matching )"))
	lex:ignore_blank()
	assert_help(lex:expect(";"), "Expect a ;")
end

local function parse_variable_dec(context, lex)
	assert_help(lex:expect_word(), "Expect a typename ")
	lex:ignore_blank()
	assert_help(lex:expect_word(), "Expect a name ")
	lex:ignore_blank()
	assert_help(lex:expect(";"), "Expect a ;")
end

local function parse_member(context, lex)
	--Lookahead for }
	lex:checkpoint()
		local should_end = (lex:expect("}")!=nil)
	lex:rollback()
	
	if should_end then
		return
	end
	
	--Lookahead to determine variable-dec oor function-dec
	lex:checkpoint()
		assert_help(lex:expect_word(), "Expect a typename ")
		lex:ignore_blank()
		assert_help(lex:expect_word(), "Expect a name ")
		lex:ignore_blank()
		local is_variable = (lex:expect(";") != nil)
	lex:rollback()
	if is_variable then
		parse_variable_dec(context, lex)
	else
		parse_function_dec(context, lex)
	end
end

local function parse_struct(context, lex)
	assert_help(lex:expect("struct"), "Expect struct declaration ", lex)
	lex:ignore_blank()
	local typename = lex:str:sub(lex:expect_word())
	assert_help(typename, "Expect a typename", lex)
	context.__head.typename = typename
	lex:ignore_blank()
	assert_help(lex:expect("{"), "Expect { ", lex)
	lex.ignore_blank()
	
	while lex.expect("}")==nil and lex.expect("$")==nil  do
		lex:ignore_blank()
		parse_member(context, lex)
	end
	
	lex:ignore_blank()
	assert_help(lex:expect(";"), "Expect ; ", lex)
end

local function parse_annotation(context, lex)
	local _,_,annotation = lex.expect_annotation()
	if annotation == "TypeInfo"
		lex:ignore_blank()
		parse_struct(context, lex)
	else
		assert(false, "Unknown annotation " .. annotation)
	end
end

local function 

local function parse(context, lex) 
	while not lex.expect("$") do
		lex:ignore("[%s\t\n]", lexer.patterns.block_comment)
		--Lookahead for annotation.
		lex:checkpoint()
		_,_,annotation = lex.expect_annotation()
		lex:rollback()
		
		if annotation then
			parse_annotation(context, lex)
		end
		if(parse_annotation())
		lex:next_line()
	end
end

local function link(context)

end


local function make_struct_info()
	local t = {typename="", members={}, file="", line=-1}
	return make_static(t)
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

	local context = {__head={}}		--Head is used to store current parsing unit.
	--[[
	Format
		--metadata for struct
		{
			@typename:={
				"typename" :=""
				"members" := [{"typename":="", "name":=""}]
				"file" := ""
				"line" := ""
			}
		}
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






