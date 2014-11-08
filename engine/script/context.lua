local class = require("class")

local context = class.create()

local function make_no_more_element(_t) 
	local newindex = function(t,k,v)
		if t[k] == nil then
			assert(false, "New member is not allowed for this table.")
		else
			rawset(t,k,v)
		end
	end
	local meta = getmetatable(_t) or {}
	meta.__newindex = newindex
	
	return _t
end

local function make_no_override(_t)
	local meta = getmetatable(_t) or {}
	meta.__newindex = function(t,k,v)
			if t[k] == nil then
				rawset(t,k,v)
			else
				assert(false, "Override is not allowed for this table.")
			end
		end
	return setmetatable(_t, meta)
end

function context.create_struct_info()
	local t = {typename="", members={}, file="", line=-1, text=""}
	t.location = function(self)
		return string.format("File:%s\tLine:%d", t.file, t.line)
	end
	
	t.dump = context.dump_entry
	
	t.members.append = function(self, t, n)
		local member = context.create_member()
		member.typename = t
		member.name = n
		table.insert(self, member)
	end
	
	local meta = {
		__eq= context.is_typeinfo_equal
	}
	t = setmetatable(t, meta)
	return make_no_more_element(t)
end

function context.create_member()
	local t = {typename="", name=""}
	return make_no_more_element(t)
end

--[[
Two typeinfo is equal only if  their name and members are identical.
]]
function context.is_typeinfo_equal(lhs, rhs)
	if lhs.typename ~= rhs.typename then
		return false, string.format("Matching typename %s and %s... Failed. Lhs loc:%s\tRhs loc:%s", 
												lhs.typename, rhs.typename, lhs.location(), rhs.location())
	end	
	
	if #lhs.members ~= #rhs.members then
		return false, string.format("Matching member size %d and %d... Failed. Lhs loc:%s\tRhs loc:%s", 
												#lhs.members, #rhs.members, lhs.location(), rhs.location())
	end
	
	for i=1,#lhs.members do
		local lhs_mem = lhs.members[i]
		local rhs_mem = rhs.members[i]
		
		if lhs_mem.typename ~= rhs_mem.typename then
			return false, string.format("Matching member type #%d %s and %s... Failed. Lhs loc:%s\tRhs loc:%s", 
													i-1, lhs_mem.typename, rhs_mem.typename, lhs.location(), rhs.location())
		end
		if lhs_mem.name ~= rhs_mem.name then
			return false, string.format("Matching member name #%d %s and %s... Failed. Lhs loc:%s\tRhs loc:%s", 
													i-1, lhs_mem.name, rhs_mem.name, lhs.location(), rhs.location())
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
		dump_help(string.format('"typename":="%s",', entry.typename), 1)
		dump_help('"members":=[', 1)
			for i=1,#entry.members-1 do
				local v = entry.members[i]
				dump_help(string.format('{"typename":="%s", "name":="%s"},', v.typename, v.name), 2)
			end
			local v = entry.members[#entry.members]
			if v then 
				dump_help(string.format('{"typename":="%s", "name":="%s"}', v.typename, v.name), 2)
			end
		dump_help('],', 1)
		dump_help(string.format('"file":="%s",', entry.file), 1)
		dump_help(string.format('"line":="%d"', entry.line), 1)
	dump_help("}",0, true)
end

function context.dump_entry(entry, level) 
	level = level or 1
	local buffer = {}
	local dump_help = function(data,l)
		l = l or 0
		for i=2,level+l do
			table.insert(buffer, "\t")
		end
		table.insert(buffer, data)
		table.insert(buffer, "\n")
	end
	
	dump_help("{")
	dump_entry_internal(entry, buffer, level+1)
	dump_help("}")
	table.remove(buffer)
	
	return table.concat(buffer)
end

function context.peel_arrayinfo(typename)
	return select(3,typename:find("^(%w+)%["))  or typename
end

do 
	local primitives = require("primitives")
	context.primitive = {}
	for _,v in ipairs(primitives) do
		context.primitive[v] = v
	end

	setmetatable(context.primitive, {
	__newindex = function(t,k,v)
		assert("Try to manipulate a static table.")
	end
	})
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
]]
function context.methods.init(self)
	make_no_override(self)
	
	--Add some primitive type here.
	for k,v in pairs(context.primitive) do
		assert(k==v, string.format("k=%s, v=%s", k, v))
		local e = context.create_struct_info()
		e.typename = v
		e.file = "primitive"
		e.line = -1
		e.text = ""
		self[e.typename] = e
	end
	
	return self
end

function context.methods.dump(self, level)
	level = level or 1
	local buffer = {}
	local dump_help = function(data,l)
		l = l or 0
		for i=2,level+l do
			table.insert(buffer, "\t")
		end
		table.insert(buffer, data)
		table.insert(buffer, "\n")
	end
	
	dump_help("{")
	for k,v in pairs(self) do
		if k ~= "__head" then
			dump_entry_internal(v, buffer, level+1)
			--We know there is a \n at the rear, bypass it
			buffer[#buffer] = ",\n"
		end	
	end
	--Bypass again as , is not needed for the last entry.
	buffer[#buffer]="\n"
	dump_help("}")
	table.remove(buffer)
	
	return table.concat(buffer)
end

return context












