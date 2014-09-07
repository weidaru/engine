local parser = require("parser")
local lexer = require("lexer")
local context = require("context")

do
	local lex = lexer.new(
[=[This line will be ignored.
Ignored.

Ignored
Messed up with indentation.
			//[[TypeInfo]]//
  struct Bar		
  {
	  float bar_a;		
	  //This will be ignored.
			double bar_b;};

	//[[TypeInfo]]//
	struct Foo {
		int foo_a;
		Bar foo_b;
		int GetFoo(arbitrary things in between);  //This will be ignored 
	};
	
asdfsdf]=], "inmemory")
	
	print(lex.data)
	print()
	local cont = context.new()
	parser.parse(cont, lex)
	
	local target_bar =
[[{
	"Bar":={
		"typename":="Bar",
		"members":=[
			{"typename":="float", "name":="bar_a"},
			{"typename":="double", "name":="bar_b"}
		],
		"file":="inmemory",
		"line":="7"
	}
}]]

	local target_foo = 
[[{
	"Foo":={
		"typename":="Foo",
		"members":=[
			{"typename":="int", "name":="foo_a"},
			{"typename":="Bar", "name":="foo_b"}
		],
		"file":="inmemory",
		"line":="14"
	}
}]]
	local source_bar = cont["Bar"]:dump()
	local source_foo = cont["Foo"]:dump()
	
	print("\n\n\n")
	print(string.format("Bar:\n%s\n%s\n\n", target_bar, source_bar))
	print(string.format("Foo:\n%s\n%s\n\n", target_foo, source_foo))
	assert(source_bar == target_bar and source_foo == target_foo)
end












