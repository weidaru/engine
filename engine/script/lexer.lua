local class = require("class")

local lexer = class.create()

--[[
Returns the start and end indices if any and all the captures if any. If no match then return nil
The result does not have to start at the current pos.
]]--
function lexer.methods.next(self, ...)
	for i=1,select("#",...) do
		local pattern = select(i, ...)
		local value = {self.data:find(pattern, self.pos)}
		if value[2] ~= nil then
			self.pos = value[2]+1
			return table.unpack(value) 
		end
	end

	return nil
end

--[[
The expect semantic is essentially the same as next expect that 
it requires the match to start at the current pos.
]]--
function lexer.methods.expect(self, ...)
	local patterns = {...}
	for i=1,select("#",...) do
		local pattern = patterns[i]
		if pattern:sub(1,1) ~= "^" then
			pattern = "^" .. pattern
		end
		assert(pattern:sub(1,1) == "^")
		patterns[i] = pattern
	end
	
	return self:next(table.unpack(patterns))
end

function lexer.methods.ignore(self, ...)
	while true do
		if self:expect(...) == nil then
			break
		end
	end
end

--Use checkpoint and rollback to perform lookahead
function lexer.methods.checkpoint(self)
	assert(self.old_pos == -1, 	[[checkpoint and rollback should always come into pairs. 
								Use either of them consecutively will raise an error.]])
	self.old_pos = self.pos
end

function lexer.methods.rollback(self)
	assert(self.old_pos ~= -1,	[[checkpoint and rollback should always come into pairs. 
								Use either of them consecutively will raise an error.]])
	self.pos = self.old_pos
	self.old_pos = -1
end

--Do some cache if possible in the future. This function should not be used frequently.
function lexer.methods.linenumber(self)
	local count = 1
	for i=1,self.pos do
		if self.data:sub(i,i) == "\n" then
			count = count+1
		end
	end
	return count
end

local function check_method(l, name)
	assert(lexer.methods[name]==nil, string.format("Try to define a function %s which exists.", name))
	return name
end

--Just do some forwarding, lexer itself will be part of the closure
function lexer.define_pattern(name, pattern)
	local next_name = "next_" .. name
	check_method(next_name)
	local expect_name = "expect_" .. name
	check_method(expect_name)
	local ignore_name = "ignore_" .. name
	check_method(ignore_name)
	
	if type(pattern) == "string" then
		lexer.methods[next_name] = function(self) return self:next(pattern) end
		lexer.methods[expect_name] = function(self) return self:expect(pattern) end
	elseif type(pattern) == "table" then
		lexer.methods[next_name] = function(self) return self:next(table.unpack(pattern)) end
		
		lexer.methods[expect_name] = function(self) return self:expect(table.unpack(pattern)) end
		
		lexer.methods[ignore_name] = function(self) return self:ignore(table.unpack(pattern)) end
	end
end

lexer.patterns = {
	annotation = "^[%s\t]*//%[%[[%s\t]*(%w+)[%s\t]*%]%]//",
	line = {".-\n", "(.+)$"},
	word = "[%a_][%w_]*",
	line_comment = {"//.-\n", "//.-$"},
	block_comment = "/%*.-%*/" 
}
	
lexer.patterns.blank = {"[%s\n\t]", lexer.patterns.block_comment, table.unpack(lexer.patterns.line_comment)}

setmetatable(lexer.patterns,{
	__index = function(t,k)
		local temp = rawget(t,k)
		if type(temp) == "string" then
			return temp
		elseif type(temp) == "table" then
			return table.unpack(temp)
		else
			return nil
		end
	end,
	__newindex = function(t,k)
		assert(false, "Write to the table is not allowed.")
	end
})

do
	--Generate all the pattern functions.
	for k,v in pairs(lexer.patterns) do
		lexer.define_pattern(k, v)
	end
end

--Assume the input is small so store a string directly.
function lexer.methods.init(self, str, file)
	self.data = str
	self.pos = 1
	self.old_pos = -1
	self.file = file or ""
	
	return self
end

return lexer
















