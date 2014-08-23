package.path = './../?.lua;' .. package.path 
local lexer = require("lexer")

local function print_and_check(expect, source)
	print("Expect = ", expect, "\tSource= ", source)
	assert(expect == source)
end

local lex = lexer.create(
[=[		/*abcasdfasdfsd*/
/*dfc*///[[Info]]//
word]=]
)

print("Data length = ", lex.data:len())

lex:checkpoint()
print_and_check(lex.data:sub(lex:next_block_comment()), "/*abcasdfasdfsd*/")
lex:ignore("[\n%s\t]")
print_and_check(lex.data:sub(lex:expect_block_comment()), "/*dfc*/")
print_and_check(select(3, lex:expect_annotation()), "Info")
lex:ignore("[\n%s\t]")
print_and_check(lex.data:sub(lex:expect_word()), "word")
lex:rollback()


