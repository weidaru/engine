local lexer = {}
--Assume the input is small so store a string directly.
function lexer.create(str)
	local t = {data=str, pos=1}
	return t
end

--"^[%s\t]*//%[%[[%s\t]*(%w+)[%s\t]*%]%]//.*\n"		For annoation



local function lookforward_decorator(self, lookforward, func) 
	local old_pos = self.pos
	
	func(self)
	
	local new_pos = self.pos
	if lookforward then
		self.pos = old_pos
	end
	
	local result = nil
	if new_pos > old_pos then
		result = self.str:sub(old_pos, new_pos-1)
	end
	return result, new_pos
end

function lexer.next(_self, pattern, lookforward)
	return lookforward_decorator(_self, lookforward, 
	function(self)
		self.pos = self.pos + self.str:match(pattern, pos)
	end) 
end

function lexer.next_line(self, lookforward)
	return self:next_line("^.-\n", lookforward)
end

function lexer.next_word(self, lookforward)
	return self:next_line("^%a%w*", lookforward)
end

function lexer.next_blank(_self, lookforward)
	return lookforward_decorator(_self, lookforward, 
	function(self)
		while self.pos <= self.str.len() do
			_, self.pos = self:next("^[ \t\n]+")
			local comment
			comment = self:next_comment()
			if comment == nil then
				break
			end
		end
	end)
end

function lexer.next_comment(_self, lookforward)
	return lookforward_decorator(_self, lookforward, 
	function(self)
		while self.pos <= self.str.len() do
			local continue = nil
			continue = self:next("^//.-\n") or self:next("/%*.-%*/")
			if not continue then
				break
			end
		end
	end
end

return lexer