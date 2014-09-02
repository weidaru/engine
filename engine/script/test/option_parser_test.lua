local option_parser = require("option_parser")


local rule = option_parser.new()
rule[{"--print", "-p", "print the string"}] = print
rule[{"--print2", "-p2", "print the string2"}] = print

print(rule:to_string())
rule:parse("--print", "Hello", "-p2", "World")