local lexer = {methods={}}

--[[
Returns the start and end indices if any and all the captures if any. If no match then return nil
The result does not have to start at the current pos.
]]--
function lexer.methods.next(self, pattern)
	local value = {self.data:find(pattern, self.pos)}
	if value[2] ~= nil then
		self.pos = value[2]+1
	end

	return table.unpack(value) 
end

--[[
The expect semantic is essentially the same as next expect that 
it requires the match to start at the current pos.
]]--
function lexer.methods.expect(self, pattern)
	if pattern:sub(1,1) ~= "^" then
		pattern = "^" .. pattern
	end
	assert(pattern:sub(1,1) == "^", "Pattern must begin with ^")
	return self:next(pattern)
end

function lexer.methods.ignore(self, pattern)
	while true do
		if self:expect(pattern) == nil then
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
		local chain_decorator = 
			function(func) 
				return 
					function(self)
						local result = {}
						for k, v in ipairs(pattern) do
							result = {func(self,v)}
							
							if #result ~= 0 then
								break
							end
						end
						return table.unpack(result)
					end
			end
			
		lexer.methods[next_name] = chain_decorator(
		function(self, p)
			return self:next(p)
		end)
		
		lexer.methods[expect_name] = chain_decorator(
		function(self, p)
			return self:expect(p)
		end)
		
		lexer.methods[ignore_name] = chain_decorator(
		function(self, p)
			return self:ignore(p)
		end)
	end
end

local patterns = {
	annotation = "^[%s\t]*//%[%[[%s\t]*(%w+)[%s\t]*%]%]//",
	line = {".-\n", "(.+)$"},
	word = "%a%w*",
	line_comment = {"//.-\n", "//.-$"},
	block_comment = "/%*.-%*/"
}
do
	--Generate all the pattern functions.
	for k,v in pairs(patterns) do
		lexer.define_pattern(k, v)
	end
end

--Assume the input is small so store a string directly.
function lexer.create(str)
	local t = {data=str, pos=1, old_pos=-1}
	for k,v in pairs(lexer.methods) do
		t[k] = v
	end
	
	return t
end

return lexer
















