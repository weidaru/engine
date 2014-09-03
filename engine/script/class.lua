--[[
	A simple class module wrap around lua table. Do what you can imagine and feel suitable.
	The only requirement is having an "init" method inside class.methods
	User can override the metatable as they want. The __index function this module use can also be referenced as m.index 
]]--


local m = {}

function m.index(t,k)
	local temp = rawget(t, k)
	if temp then
		return temp
	else
		return lexer.methods[k]
	end
end

function m.create()
	local class = {methods={}}
	
	class.new = function(...)
		local obj = {}
		assert(class.method.init != nil, "Have to specify an init method for this class.")
		
		setmetatable(obj, {__index=m.index})
		class.method.init(obj, ...)
		return obj
	end
	
	return t
end

return m