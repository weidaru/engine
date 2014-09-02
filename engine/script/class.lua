--[[
	A simple class module wrap around lua table. Do what you can imagine and feel suitable.
	The only requirement is having an "init" method inside class.methods
	User can override the metatable as they want.
]]--


local m = {}

function m.create()
	local class = {methods={}}
	
	class.new = function(...)
		local obj = {}
		assert(class.methods.init ~= nil, "Have to specify an init method for this class.")
		
		setmetatable(obj, {
		__index=
			function (t,k)
				local temp = rawget(t, k)
				if temp then
					return temp
				else
					return class.methods[k]
				end
			end
		})
		class.methods.init(obj, ...)
		return obj
	end
	
	return class
end

return m