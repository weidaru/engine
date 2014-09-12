local option_parser = require("option_parser")
local lexer = require("lexer")
local context_class = require("context")
local parser = require("parser")
local fs = require("fs")

local POINTER_REFERENCE_CODE = 
[[namespace internal {
struct PointerRegistration {
	static PointerRegistration registration;

	PointerRegistration() {
		TypeInfoManager::GetSingleton()->Create( 
			pointer_typename, 
			sizeof(char *), 
			TypeInfo::Members()); 
	}
};
PointerRegistration PointerRegistration::registration;

struct Dummy {
	int &a;

	Dummy(int new_a) : a(new_a) {}
};

struct ReferenceRegistration {
	static ReferenceRegistration registration;

	ReferenceRegistration() {
		TypeInfoManager::GetSingleton()->Create( 
			reference_typename, 
			sizeof(Dummy), 
			TypeInfo::Members()); 
	}
};
ReferenceRegistration ReferenceRegistration::registration;

}
]]

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
		for i=1,indent do
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
			for _,v in ipairs(e.members) do
				append_help("{")
				start_scope()
					append_help(string.format('TypeInfo::Member member;'))
					append_help(string.format('member.type_name="%s";', v.typename))
					append_help(string.format('member.name="%s";', v.name))
					append_help(string.format('member.offset=(unsigned int)offset(%s,%s);', e.typename, v.name))
					append_help('members.push_back(member);')
				end_scope()
				append_help("}")
			end
			append_help(string.format('TypeInfoManager::GetSingleton()->Create(TypeInfoBind<%s>::GetName(), sizeof(%s), members);', e.typename, e.typename))
		end_scope()
		append_help('}')
	end
	
	append_help('#include <utility>')
	append_help('#include "utils/type_info.h"')
	append_help('#include "utils/type_info_helper.h"')
	append_help('')
	
	--We need to declare all the struct first.
	append_help([[//All the necessary struct declaration]])
	for k,v in pairs(context) do
		if k:sub(1,2) ~= "__" then
			if context_class.primitive[k] == nil then
				append_help(v.text)
				append_help("")
			end
		end
	end
	
	append_help('namespace s2 {')
	--Do for primitive
	append_help([[//TypeInfo for primitive]])
	for k,_ in pairs(context_class.primitive) do
		if k~="pointer" and k~="reference" then
			append_help(string.format("DefinePrimitive(%s)", k))
		end
	end
	append_help("")
		--Do for pointer and reference
	append_help([[//TypeInfo for pointer and reference]])
	append_help(POINTER_REFERENCE_CODE)
	append_help("")
	--Do for others
	append_help([[//TypeInfo for user defined type]])
	for k,v in pairs(context) do
		if k:sub(1,2) ~= "__" then
			if context_class.primitive[k] == nil then
				generate_entry(v, append_help, start_scope, end_scope)
				append_help("")
			end
		end
	end
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

if not pcall(rule.parse, rule, ...) then
	print(rule:to_string())
end
assert(source_dir~=nil, string.format("Soruce Dir is nil! Usage:\n%s", rule:to_string()))
assert(dest~=nil, string.format("Dest Dir is nil! USage:\n%s", rule:to_string()))

local outputfile = io.open(dest, "w")
assert(outputfile, string.format("Cannot open dest file %s", dest))

local context = context_class.new()

--Start scan
fs.scan_dir(source_dir, {"*.h", "*.hpp", "*.cpp", "*.c", "*.cc"}, 
function(filepath)
	local f = assert(io.open(filepath, "r"))
	--Cache the file
	local buffer = f:read("*a")
	f:close()
	
	local lex = lexer.new(buffer, filepath)
	print("===========Parsing " .. filepath)
	parser.parse(context, lex)
	print("===========Parsing complete\n")
end)

print("===========Linking...")
parser.link(context)
print("===========Linking complete.\n\n")

print(context:dump())

print("===========Generating...")
generate(context, outputfile)
print("===========Generating complete.\n\n")


outputfile:close()

