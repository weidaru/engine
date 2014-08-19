local lexer = {}
--Assume the input is small so store a string directly.
function lexer.create(str)
	local t = {data=str, pos=1, old_pos=-1}
	return t
end

--[[
Returns the matched sub-string if any and all the captures if any.
The result does not have to start at the current pos.
]]--
function lexer.next(self, pattern)
	local value = {self.str:find(pattern, pos)}
	local result = self.str:sub(value[1],value[2])
	self.pos = value[2]+1
	local capture = {}
	for i=3,select("#",value) do
		table.insert(capture, value[i])
	end
	return result, table.unpack(capture) 
end

--[[
The expect semantic is essentially the same as next expect that 
it requires the match to start at the current pos.
]]--
function lexer.expect(self, pattern)
	if pattern:sub(1,1) != "^" then
		pattern = "^" .. pattern
	end
	assert(pattern:sub(1,1) == "^", "Pattern must begin with ^")
	return self:next(self, pattern)
end


function lexer.expect_annotaion(self)
	return lexer:expect("^[%s\t]*//%[%[[%s\t]*(%w+)[%s\t]*%]%]//.*\n")
end

function lexer.expect_line(self)
	return self:expect("^.-\n")
end

function lexer.expect_word(self)
	return self:expect("^%a%w*")
end

function loop_decorator(self, func) 
	local old_pos = self.pos
	
	func(self)
	
	local result = nil
	if self.pos > old_pos then
		result = self.str:sub(old_pos, self.pos-1)
	end
	return result, self.pos
end

function lexer.expect_comment(_self)
	return loop_decorator(_self, 
	function(self)
		while self.pos <= self.str.len() do
			local continue = nil
			continue = self:expect("^//.-\n") or self:expect("/%*.-%*/")
			if not continue then
				break
			end
		end
	end)
end

function lexer.expect_blank(_self)
	return loop_decorator(_self, 
	function(self)
		while self.pos <= self.str.len() do
			_, self.pos = self:expect("^[ \t\n]+")
			local comment
			comment = self:expect_comment()
			if comment == nil then
				break
			end
		end
	end)
end

function lexer.expect_ignore_blank(self, pattern)
	while true do
		if lexer.expect_blank(self, pattern) == nil then
			break
		end
	end
	return lexer.expect(self, pattern)
end

--Use checkpoint and rollback to perform lookahead
function lexer.checkpoint(self)
	assert(self.old_pos == -1, 	[[checkpoint and rollback should always come into pairs. 
								Use either of them consecutively will raise an error.]])
	self.old_pos = self.pos
end

function
	assert(self.old_pos ~= -1,	[[checkpoint and rollback should always come into pairs. 
								Use either of them consecutively will raise an error.]])
	self.pos = self.old_pos
	self.old_pos = -1
end

return lexer
















