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

function context.methods.init(self)
	self.typename = ""
	self.members={}
	self.line = -1
	return make_no_override(self)
end