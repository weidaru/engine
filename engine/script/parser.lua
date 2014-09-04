local m = {}

local lexer = require("lexer")
local context_class = require("context")

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

local function assert_help(lex, message, func, ...)
	local temp = {func(lex, ...)}
	if temp[0] == nil then
		local buffer = {}
		table.insert(buffer, lex.filename)
		table.insert(buffer, " line: ")
		table.insert(buffer, lex.linenumber())
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

--We do not care about function dec so just do syntax match, no sub-program here.
local function parse_function_dec(context, lex)
	assert_help(lex, "Expect a typename ", lex.expect_word)
	lex:ignore_blank()
	assert_help(lex, "Expect a name ", lex.expect_word)
	lex:ignore_blank()
	--I am being lazy here. As in c/c++ parameter declaration will contain no parenthesis, I simply try to find the pair of "(" ")"
	assert_help(lex, "Expect a ( ", lex.expect, "(")
	assert_help(lex, "Cannot find matching )", lex.next, ")")
	lex:ignore_blank()
	assert_help(lex, "Expect a ;", lex.expect, ";")
end

local function parse_variable_dec(context, lex)
	local typename = assert_help(lex, "Expect a typename ", lex.expect_word)
	lex:ignore_blank()
	local name = assert_help(lex, "Expect a name ", lex.expect_word)
	lex:ignore_blank()
	assert_help(lex:expect(";"), "Expect a ;")
	
	--Sub-program for variable declaration
	context.__head.member.append(typename, name)
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
		assert_help(lex, "Expect a typename ", lex.expect_word)
		lex:ignore_blank()
		assert_help(lex,, "Expect a name ", lex.expect_word)
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
	assert_help(lex, "Expect struct declaration ", lex.expect, "struct")
	lex:ignore_blank()
	local typename = assert_help(lex, "Expect a typename", lex.expect_word)
	context.__head = context_class.create_struct_info()							--__head is used to store current parsing unit.
	context.__head.typename = typename
	lex:ignore_blank()
	assert_help(lex, "Expect { ", lex.expect, "}")
	lex.ignore_blank()
	
	while lex.expect("}")==nil and lex.expect("$")==nil  do
		lex:ignore_blank()
		parse_member(context, lex)
	end
	
	lex:ignore_blank()
	assert_help(lex, "Expect ; ", lex.expect, ";")
	if context[context.__head.typename] != nil 
		local lhs = context[context.__head.typename]
		local rhs = context.__head
		assert(context_class.is_type_equal(lhs, rhs), string.format(
				"Try to declared a different type with same typename %s. Dumping:\n%s\n%s", 
				context.__head.typename,
				context.dump_entry(lhs)),
				context.dump_entry(rhs))
	end
	context[context.__head.typename] = context.__head
	context.__head = nil
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

function m.parse(context, lex) 
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

function m.link(context)
--STUB
end

return m



