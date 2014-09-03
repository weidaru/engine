local context = require("context")

local con = context.new()
do
	--[[
	Entry1:
		{
			"entry1":={
				"typename":="entry1",
				"members":=[
					{"typename":="foo", "name":="foo_name"},
					{"typename":="bar", "name":="bar_name"}
				],
				"file":="entry1.h",
				"line":="10"
			}
		}
	]]--
	local entry = context.create_struct_info()
	entry.typename = "entry1"
	entry.file = "entry1.h"
	entry.line = "10"
	entry.members:append("foo", "foo_name")
	entry.members:append("bar", "bar_name")
	con[entry.typename] = entry
end
do
	--[[
	Entry2:
		{
			"entry2":={
				"typename":="entry2",
				"members":=[
					{"typename":="foo", "name":="foo_name"},
				],
				"file":="entry2.h",
				"line":="10"
			}
		}
	]]--
	local entry = context.create_struct_info()
	entry.typename = "entry2"
	entry.file = "entry2.h"
	entry.line = "10"
	entry.members:append("foo", "foo_name")
	con[entry.typename] = entry
end
do
	--[[
	Entry1 dup:
		{
			"entry1":={
				"typename" :="entry1",
				"members" := [
					{"typename":="foo", "name":="foo_diff_name"},
					{"typename":="bar", "name":="bar_name"}
				],
				"file" := "entry1.h",
				"line" := "30"
			}
		}
	]]--
	local entry = context.create_struct_info()
	entry.typename = "entry1"
	entry.file = "entry1.cpp"
	entry.line = "20"
	entry.members:append("foo", "foo_diff_name")
	entry.members:append("bar", "bar_name")
	
	assert(entry~=con[entry.typename])
end
do
	--[[
	Entry1 dup:
		{
			"entry1":={
				"typename" :="entry1",
				"members" := [
					{"typename":="foo", "name":="foo_name"},
					{"typename":="bar", "name":="bar_name"}
				],
				"file":="entry1.h",
				"line":="30"
			}
		}
	]]--
	local entry = context.create_struct_info()
	entry.typename = "entry1"
	entry.file = "entry1.cpp"
	entry.line = "20"
	entry.members:append("foo", "foo_name")
	entry.members:append("bar", "bar_name")
	
	assert(entry == con[entry.typename])
end
do
	--[[
	Entry1 dup:
		{
			"entry1":={
				"typename":="entry1",
				"members":=[
					{"typename":="foo", "name":="foo_name"},
					{"typename":="bar", "name":="bar_name"}
				],
				"file":="entry1.h",
				"line":="30"
			}
		}
	]]--
	local entry = context.create_struct_info()
	entry.typename = "entry1"
	entry.file = "entry1.cpp"
	entry.line = "20"
	entry.members:append("foo", "foo_name")
	entry.members:append("bar", "bar_name")
	
	local result = {pcall(getmetatable(con).__newindex, con, entry.typename, entry)}
	assert(not result[1], select(2, table.unpack(result)))
end

do
	local entry1_target= 
[[{
	"entry1":={
		"typename":="entry1",
		"members":=[
			{"typename":="foo", "name":="foo_name"},
			{"typename":="bar", "name":="bar_name"}
		],
		"file":="entry1.h",
		"line":="10"
	}
}]]
	local entry1_dump = con["entry1"]:dump()
	print(entry1_dump)
	assert(entry1_dump == entry1_target, string.format("Dump:\n%s\nTarget:\n%s", entry1_dump, entry1_target))
	local entry2_dump = con["entry2"]:dump()
	print(entry1_dump)
	assert(entry1_dump ~= entry2_dump, string.format("Entry1:\n%s\nEntry2:\n%s", entry1_dump, entry2_dump))
end

do
	local target = 
[[{
	"entry1":={
		"typename":="entry1",
		"members":=[
			{"typename":="foo", "name":="foo_name"},
			{"typename":="bar", "name":="bar_name"}
		],
		"file":="entry1.h",
		"line":="10"
	},
	"entry2":={
		"typename":="entry2",
		"members":=[
			{"typename":="foo", "name":="foo_name"}
		],
		"file":="entry2.h",
		"line":="10"
	}
}]]
	local dump_str = con:dump()
	print(dump_Str)
end










