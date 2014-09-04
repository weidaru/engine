local context = require("context")

local con = context.new()
do
	--[[
	Entry1:
		{
			"entry1":={
				"typename" :="entry1",
				"members" := [
					{"typename":="foo", "name":="foo_name"},
					{"typename":="bar", "name":="bar_name"}
				],
				"file" := "entry1.h",
				"line" := "10"
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
				"typename" :="entry2",
				"members" := [
					{"typename":="foo", "name":="foo_name"},
				],
				"file" := "entry2.h",
				"line" := "10"
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
	Entry1 dup:
		{
			"entry1":={
				"typename" :="entry1",
				"members" := [
					{"typename":="foo", "name":="foo_same_name"},
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
	
	assert(not entry==con[entry.typename])
end
do
	--[[
	Entry1 dup:
		{
			"entry1":={
				"typename" :="entry1",
				"members" := [
					{"typename":="foo", "name":="foo_same_name"},
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
	entry.members:append("foo", "foo_name")
	entry.members:append("bar", "bar_name")
	
	assert(entry == con[entry.typename])
end
do
	--[[
	Entry1 dup:
		{
			"entry1":={
				"typename" :="entry1",
				"members" := [
					{"typename":="foo", "name":="foo_same_name"},
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
	entry.members:append("foo", "foo_name")
	entry.members:append("bar", "bar_name")
	
	assert(not pcall(getmetatable(con).__index, con, entry.typename, entry))
end

do
	local entry1_target= 
[[{
	"entry1":={
		"typename" :="entry1",
		"members" := [
			{"typename":="foo", "name":="foo_name"},
			{"typename":="bar", "name":="bar_name"}
		],
		"file" := "entry1.h",
		"line" := "10"
	}
}]]
	local entry1_dump = con["entry1"]:dump()
	print(entry1_dump)
	assert(entry1_dump == entry1_target)
	local entry2_dump = con["entry2"]:dump()
	print(entry2_dump)
	assert(entry1_dump ~= entry2_dump)
end

do
	local target = 
[[{
	"entry1":={
		"typename" :="entry1",
		"members" := [
			{"typename":="foo", "name":="foo_name"},
			{"typename":="bar", "name":="bar_name"}
		],
		"file" := "entry1.h",
		"line" := "10"
	},
	"entry2":={
		"typename" :="entry2",
		"members" := [
			{"typename":="foo", "name":="foo_name"}
		],
		"file" := "entry2.h",
		"line" := "10"
	}
}]]
	local dump_str = con:dump()
	print(dump_str)
	assert(dump_str == target)
end










