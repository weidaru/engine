local class = require("class")

local m = class.create()

function m.methods.parse(rule, ...)
	local argc = select("#", ...)
	assert(argc%2==0, "Invalid number of arguments. " .. argc)
	for i=0,argc/2-1,1 do
		local key = select(i*2+1,...)
		local value = select(i*2+2,...)
		local func = rule[key].func
		assert(func~=nil, "Bad option " .. key)
		func(key, value)
	end
end

function m.methods.to_string(rule)
	local buffer = {"Arguments:\n"}
	local explored = {}
	for _,v in pairs(rule) do
		if type(v)=="table" and explored[v] == nil then
			table.insert(buffer, "  ")
			table.insert(buffer, v.k1)
			table.insert(buffer, "  ")
			table.insert(buffer, v.k2)
			table.insert(buffer, "\t")
			table.insert(buffer, v.doc)
			table.insert(buffer, "\n")
		end
		explored[v] = true
	end
	return table.concat(buffer)
end

function m.methods.init(self)
	local meta = getmetatable(self)
	meta.__newindex = function(t,k,v)
		assert(type(v)=="function", "Value must be a function.")
		assert(string.match(k[1], "--[%w]+")~=nil, "Bad first key " .. k[1] .. "\t It should be '--[%w]+'")
		assert(string.match(k[2], "-[%w]+")~=nil, "Bad second key " .. k[2] .. "\t It should be '-[%w]+'")
		assert(t[k[1]] == nil, "First key " .. k[1] .. " already exists.")
		assert(t[k[2]] == nil, "Second key " .. k[2] .. " already exists.")
		local _doc = k[3]
		if _doc == nil then
			_doc = ""
		end
		local value = {k1=k[1], k2=k[2], doc=_doc, func=v}
		rawset(t, k[1], value)
		rawset(t, k[2], value)
	end
	return self
end


return m















