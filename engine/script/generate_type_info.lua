local option_parser = require("option_parser")
local lexer = require("lexer")
local context_class = require("context")
local parser = require("parser")
local fs = require("fs")

--[[
The generated file is generally small, so we can generate all and dump to file.
But things can change. So the signature require a second parameter file for flexibility.
]]
local function generate(context, file)
	local buffer = {}
	local indent = 0
	
	local function start_scope() indent=indent+1 end
	local function end_scope() indent=indent-1 end
	
	local function append_help(data, nolinebreak)
		for i,indent do
			table.insert(buffer, "\t")
		end
		table.insert(buffer, data)
		if nolinebreak == nil then
			table.insert(buffer, "\n")
		end
	end
	
	local function generate_entry(e)
		append_help(string.format('DeclareTypeInfo(%s)', e.typename))
		append_help(string.format('DefineTypeInfo(%s) {', e.typename))
		start_scope()
			append_help('TypeInfo::Members members;')
			for _,v in ipairs() do
				append_help(string.format('TypeInfo::Member member;'))
				append_help(string.format('member.typen_name="%s";', v.typename))
				append_help(string.format('member.name="%s";', v.name))
				append_help(string.format('member.offset=offset(%s,%s);', e.typename, v.name))
				append_help('members.push_back(member)')
			end
			append_help(string.format('TypeInfoManager::GetSingleton()->Create(TypeInfoBind<%s>::GetName(), sizeof(%s), members);', e.typename, e.typename))
		end_scope()
		append_help('}', e.typename)
	end
	
	append_help('#include <utility>')
	append_help('#include "type_info.h"')
	append_help('#include "type_info_helper.h"')
	append_help('')
	
	append_help('namespace s2 {')
	start_scope()
		for _,v in pairs(context) do
			generate_entry(v, append_help, start_scope, end_scope)
			append_help("")
		end
	end_scope()
	append_help("}")
	assert(indent == 0, string.format("Bad indentation %s. It should be 0.", indent))
	
	file:write(table.concat(buffer))
end


local source_dir = nil
local dest = nil

local rule = option_parser.new()
rule[{"--source_dir", "-s", "Set the directory which needs scanning, all the .h .hpp .cpp .cc files will be scanned. Use absolute path!!"}] 
	= function(_, v) source_dir = v end

rule[{"--dest", "-d", "The file path that will be generated."}] 
	= function(_, v) dest = v end	
	
local outputfile = io.open(dest)
assert(outputfile, string.format("Cannot open dest file %s", outputfile))

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
	
	local lex = lexer.new(table.concat(buffer), filepath)
	print("===========Parsing " .. filepath)
	parser.parse(context, lex)
	print("===========Parsing complete")
end)

print("===========Linking...")
parser.link(context)
print("===========Linking complete.")

print("===========Generating...")
generate(context, outputfile)
print("===========Generating complete.")


outputfile:close()

