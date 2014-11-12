local parser = require("parser")
local lexer = require("lexer")
local context = require("context")

local inmemory_data = 
[=[This line will be ignored.
Ignored.

Ignored
Messed up with indentation.
			//[[TypeInfo]]//
  struct Bar		
  {
	  float* bar_a; int   bar_array[5]; 	char *bar_ptr;
	  //This will be ignored.
			double bar_b;};

	//[[TypeInfo]]//
	struct Foo {
		//[[CoreData]]//
		int foo_a[2][3];
		Bar foo_b;
		Foo();
		~Foo() {}
		int GetFoo(arbitrary things in between);  //This will be ignored 
		unsigned long long int Shit() { { } }
		const char * Get(asfoasdfwenfocjnv sdfsdf  ,, &#$5oiserfewfnaoscvnioiwev) const;
	};
	
asdfsdf]=]


do
	--Test parsing.
	local lex = lexer.new(inmemory_data, "inmemory")
	
	print(lex.data)
	print()
	local cont = context.new()
	parser.parse(cont, lex)
	
	local target_bar =
[[{
	"Bar":={
		"typename":="Bar",
		"members":=[
			{"typename":="pointer", "name":="bar_a"},
			{"typename":="int[5]", "name":="bar_array"},
			{"typename":="pointer", "name":="bar_ptr"},
			{"typename":="double", "name":="bar_b"}
		],
		"file":="inmemory",
		"line":="6"
	}
}]]

	local target_foo = 
[[{
	"Foo":={
		"typename":="Foo",
		"members":=[
			{"typename":="int[2][3]", "name":="foo_a"},
			{"typename":="Bar", "name":="foo_b"}
		],
		"file":="inmemory",
		"line":="13"
	}
}]]
	local source_bar = cont["Bar"]:dump()
	local source_foo = cont["Foo"]:dump()
	
	print("\n\n\n")
	print(string.format("Bar:\n%s\n%s\n\n", target_bar, source_bar))
	print(string.format("Foo:\n%s\n%s\n\n", target_foo, source_foo))
	assert(source_bar == target_bar and source_foo == target_foo)
	
	assert(cont["Foo"].custom["CoreData"] == 0, string.format("Expect %s but got %s", 0, cont["Foo"].custom["CoreData"]))
	--Test linking
	parser.link(cont)
end











