local class = require("class")

local context = class.create()

local function make_static(_t) 
	local meta = {
		__newindex = function(t,k,v)
			assert("New member is not allowed for static table.")
		end
	}
	return setmetatable(_t, meta)
end

local function make_no_override(_t)
	local meta = getmetatable(_t)
	meta.__newindex = function(t,k,v)
			if t[k] == nil then
				rawset(t,k,v)
			else
				assert("Override is not allowed for this table.")
			end
		end
	return _t
end

function context.create_struct_info()
	local t = {typename="", members={}, file="", line=-1}
	return make_static(t)
end

function context.is_typeinfo_equal(lhs, rhs)
	if lhs.file == rhs.file and lhs.line == rhs.line then
		return true
	end
	
	if lhs.typename ~= rhs.typename then
		return false, string.format("Matching typename %s and %s... Failed.", lhs.typename, rhs.typename)
	end	
	
	if #lhs.members ~= #rhs.members then
		return false, string.format("Matching member size %d and %d... Failed.", #lhs.members, #rhs.members)
	end
	
	for i=1,#lhs.members do
		local lhs_mem = lhs.members[i]
		local rhs_mem = rhs.members[i]
		
		if lhs_mem.typename ~= rhs_mem.typename then
			return false, string.format("Matching member type #%d %s and %s... Failed.", i-1, lhs_mem.typename, rhs_mem.typename)
		end
		if lhs_mem.name ~= rhs_mem.name then
			return false, string.format("Matching member name #%d %s and %s... Failed.", i-1, lhs_mem.name, rhs_mem.name)
		end
	end
	
	return true
end

--Level start at 1, dump as JSON

local function dump_entry_internal(entry, buffer, level) 
	local dump_help = function(data,l)
		for i=2,level+l do
			table.insert(buffer, "\t")
		end
		table.insert(buffer, data)
		table.insert(buffer, "\n")
	end

	dump_help(string.format('"%s":={', entry.typename), 0)
		dump_help(string.format('"typename":="%s"', entry.typename), 1)
		dump_help('"members":=[', 1)
			for i=1,#entry.members-1 do
				local v = entry.members[i]
				dump_help(string.format('{"typename":="%s", "name":="%s},"', v.typename, v.name), 2)
			end
			dump_help(string.format('{"typename":="%s", "name":="%s}"', v.typename, v.name), 2)
		dump_help(']', 1)
		dump_help(string.format('"file":="%s"', entry.file), 1)
		dump_help(string.format('"line:="%s""'), 1)
	dump_help("}",0, true)
end

local context.dump_entry(entry, level) 
	level = level or 1
	local buffer = {}
	local dump_help = function(data,l)
		for i=2,level+l do
			table.insert(buffer, "\t")
		end
		table.insert(buffer, data)
		table.insert(buffer, "\n")
	end
	
	dump_help("{", level)
	dump_entry_internal(entry, buffer, level+1)
	dump_help("}", level)
	
	return table.concat(buffer)
end

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
function context.methods.init(self)
	self.typename = ""
	self.members={}
	self.line = -1
	return make_no_override(self)
end

function context.methods.dump(self, level)
	level = level or 1
	local buffer = {}
	local dump_help = function(data,l)
		for i=2,level+l do
			table.insert(buffer, "\t")
		end
		table.insert(buffer, data)
		table.insert(buffer, "\n")
	end
	
	dump_help("{", level)
	for k,v in pairs(self) do
		if k ~= "__head" then
			dump_entry_internal(v, buffer, level+1)
			--We know there is a \n at the rear, bypass it
			buffer[-1] = ",\n"
		end	
	end
	--Bypass again as , is not needed for the last entry.
	buffer[-1]="\n"
	dump_help("}",level)
end















