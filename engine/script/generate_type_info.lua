local option_parser = require("option_parser")
local lexer = require("lexer")
local context_class = require("context")
local parser = require("parser")
local fs = require("fs")

local source_dir
local dest

local rule = option_parser.create_rule()
rule[{"--source_dir", "-s", "Set the directory which needs scanning, all the .h .hpp .cpp .cc files will be scanned. Use absolute path!!"}] = 
function(_, v) 
	source_dir = v
end

rule[{"--dest", "-d", "The file path that will be gnerated."}] = 
function(_, v)
	dest = v
end

if not pcall(rule.parse, rule, ...) then
	print(rule:to_string())
end
assert(source_dir~=nil, "Soruce Dir is nil!")
assert(dest~=nil, "Dest Dir is nil!")

local context = context_class.new()

--Start scan
fs.scan_dir(source_dir, {"*.h", "*.hpp", "*.cpp", "*.c", "*.cc"}, 
function(filepath)
	local f = assert(io.open(filepath, "r"))
	--Cache the file
	local buffer = {}
	for line in f:lines() do
		table.insert(buffer, line)
	end
	f:close()
	
	local lex = lexer.new(table.concat(buffer))
	print("===========Parsing " .. filepath)
	parser.parse(context, lex)
	print("===========Parsing complete")
end)

print("===========Linking...")
parser.link(context)
print("===========Linking complete.")






