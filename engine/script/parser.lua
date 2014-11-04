local m = {}

local lexer = require("lexer")
local context_class = require("context")

local function assert_help(lex, message, func, ...)
	local temp = {func(lex, ...)}

	if temp[1] == nil then
		local buffer = {}
		table.insert(buffer, "\nSyntax Error at ")
		table.insert(buffer, lex.name)
		table.insert(buffer, " line: ")
		table.insert(buffer, lex:linenumber())
		table.insert(buffer, "\n")
		table.insert(buffer, message)
		assert(false, table.concat(buffer))
	end
	local result = {lex.data:sub(temp[1], temp[2])}
	for i=3, #result do
		table.insert(result, temp[i])
	end
	
	return table.unpack(result)
end

local function parse_type(context, lex)
	local typename = assert_help(lex, "Expect a word", lex.expect_word)
	lex:ignore_blank()
	if lex:expect("*") then 
		typename = "pointer"
	end
	
	return typename
end

--We do not care about function dec so just do syntax match, no sub-program here.
local function parse_function_dec(context, lex)
	parse_type(context, lex)
	lex:ignore_blank()
	assert_help(lex, "Expect a word ", lex.expect_word)
	lex:ignore_blank()
	--I am being lazy here. As in c/c++ parameter declaration will contain no parenthesis, I simply try to find the pair of "(" ")"
	assert_help(lex, "Expect a ( ", lex.expect, "%(")
	assert_help(lex, "Cannot find matching )", lex.next, "%)")
	lex:ignore_blank()
	assert_help(lex, "Expect a ;", lex.expect, ";")
end

local function parse_variable_dec(context, lex)
	local typename = parse_type(context, lex)
	lex:ignore_blank()
	local name = assert_help(lex, "Expect a word ", lex.expect_word)
	lex:ignore_blank()
	if lex:expect("%[") ~= nil then
		lex:ignore_blank()
		local length = tonumber(assert_help(lex, "Expect a positive number", lex.expect, "[1-9]%d*"))
		lex:ignore_blank()
		assert_help(lex, "Expect a ]", lex.expect, "%]")
		typename = string.format("%s[%d]", typename, length)
		lex:ignore_blank()
	end
	assert_help(lex, "Expect a ;", lex.expect, ";")
	
	--Sub-program for variable declaration
	context.__head.members:append(typename, name)
end

local function parse_member(context, lex)
	--Lookahead for }
	lex:checkpoint()
		local should_end = (lex:expect("}")~=nil)
	lex:rollback()
	
	if should_end then
		return
	end
	
	--Lookahead to determine variable-dec or function-dec
	lex:checkpoint()
		parse_type(context, lex)
		lex:ignore_blank()
		assert_help(lex, "Expect a name ", lex.expect_word)
		lex:ignore_blank()
		local is_variable = (lex:expect(";") ~= nil)
		is_variable = is_variable or (lex:expect("%[") ~= nil)
	lex:rollback()
	if is_variable then
		parse_variable_dec(context, lex)
	else
		parse_function_dec(context, lex)
	end
end

local function parse_struct(context, lex)
	context.__head = context_class.create_struct_info()							--__head is used to store current parsing unit.
	context.__head.file = lex.file
	context.__head.line = lex:linenumber()
	local text_start = lex.pos
	assert_help(lex, "Expect struct declaration ", lex.expect, "struct")
	lex:ignore_blank()	
	local typename = assert_help(lex, "Expect a typename", lex.expect_word)
	context.__head.typename = typename
	lex:ignore_blank()
	assert_help(lex, "Expect { ", lex.expect, "{")
	lex:ignore_blank()
	
	while lex:expect("}")==nil and lex:expect("$")==nil  do
		lex:ignore_blank()
		parse_member(context, lex)
	end
	
	lex:ignore_blank()
	assert_help(lex, "Expect ; ", lex.expect, ";")
	local text_end = lex.pos-1
	
	context.__head.text = lex.data:sub(text_start, text_end)
	
	if context[context.__head.typename] ~= nil then
		local lhs = context[context.__head.typename]
		local rhs = context.__head
		assert(context_class.is_typeinfo_equal(lhs, rhs), 
				string.format(
					"Try to declared a different type with same typename %s. Dumping:\n%s\n%s", 
					context.__head.typename,
					lhs:dump(),
					rhs:dump()))
		print("Duplicated. Ignore")
	else
		context[context.__head.typename] = context.__head
	end
end

local function parse_annotation(context, lex)
	local _,_,annotation = lex:expect_annotation()
	if annotation == "TypeInfo" then
		lex:ignore_blank()
		parse_struct(context, lex)
		print(string.format("Successfully paring a struct. Dumping:\n%s", context.__head:dump()))
	else
		assert(false, "Unknown annotation " .. annotation)
	end
end


--[=[
parse looks for certain "annotation" which is defined below. Right now only "TypeInfo" is supported.
It does a partial C/C++ syntax analysis. In addition, it does part of the work for linking, different compiled
struct will be required to have identical typeinfo metadata. See context.lua for the definition of identical typeinfo.
And it does not work with namespace!!

Here is the rules for parsing.
The struct should be plain old c style except that it allows function declaration.
It should have no virtual things or any public private declaration.
Actually implementation may just use pattern matching.]

<what-we-care> ::= <annotation-warp> <struct>
<annotation-warp> ::= <SOL> <space-tab> "//[[" <annotation> "]]//" <space-tab> <EOL>
<annotation-warp> ::= <SOL> <space-tab> "//[[" <annotation> "]]//" <space-tab> <EOL>
<annotation> ::= "TypeInfo"
<comment> ::= "//" <text> <EOL> | "/*" <text> "*/"
<struct> ::= "struct" <blank> <word> <blank> "{" <members> "}" <blank> ";"
<blank> ::= "\t" <blank> | " " <blank> | "\n" <blank> | <comment> <blank> | " " | "\t" | "\n"
<spaces-tabs> ::= "\t" <spaces-tabs> | " " <spaces-tabs> | ""
<members> ::= 	<variable-dec> <blank> <members> | 
							<function-dec> <members> |  
							<blank>
<variable-dec> ::=  	<type> <blank> <word> <blank> ";" |
								<type> <blank> <word> <blank> "[" <blank> <positive_number> <blank> "]"
<function-dec> ::= <type> <blank> <word> <blank> "(" <anything> ")" <anything> ";"		As we don't care about function, just match it loosely.
<type> ::= <word> <blank> "*" | <word> "*"
]=]
function m.parse(context, lex) 
	while not lex:expect("$") do
		lex:ignore("[%s\t\n]", lexer.patterns.block_comment)
		--Lookahead for annotation.
		lex:checkpoint()
		_,_,annotation = lex:expect_annotation()
		lex:rollback()
		
		if annotation then
			print(string.format("Annotation found! File: %s at %s line: %d", annotation, lex.file, lex:linenumber()))
			parse_annotation(context, lex)
		end

		lex:next_line()
	end
end

--[[
Parts of the link is done in parse.
Here we only check whether each type has a place to reference.
]]
function m.link(context)
	for _,typeinfo in pairs(context) do
		for _, v in ipairs(typeinfo.members) do
			local typename = context_class.peel_arrayinfo(v.typename)
			 if context[typename] == nil then
				assert(false, string.format(
[[
Cannot find typeinfo %s (could be array) as a member of %s at 
%s line %d
Dump context: %s
]], 
						typename, typeinfo.typename, typeinfo.file, typeinfo.line, context:dump()))
			 end
		end
	end
end

return m











