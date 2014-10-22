#ifndef SHADER_TYPEINFO_MANAGER_H_
#define SHADER_TYPEINFO_MANAGER_H_

#include <vector>
#include <map>

#include "utils/type_info.h"


namespace s2 {

class ShaderTypeInfoManager {
private:
	typedef std::map<s2string, TypeInfo*> TypeMap;
	typedef std::map<s2string, std::vector<s2string> > CompatibleMap;

public:
	//This should only be used after main starts.
	bool CheckCompatible(const s2string &shader_typename, const s2string &cpp_type) const;
	void MakeCompatible(const s2string &shader_typename, const s2string &cpp_type);
	const TypeInfo & GetTypeInfo(const s2string &shader_typename) const;
	bool HasTypeInfo(const s2string &shader_typename) const;
	
	const TypeInfo & CreateScalar(const s2string &shader_typename, unsigned int size);
	const TypeInfo & CreateMatrix(const s2string &shader_typename, const s2string &scalar_typename, unsigned int rows, unsigned int columns);
	const TypeInfo & CreateVector(const s2string &shader_typename, const s2string &scalar_typename, unsigned int count);
	const TypeInfo & CreateStruct(const s2string &shader_typename, unsigned int size, const TypeInfo::Members &members);
	const TypeInfo & CreateArray(const s2string &shader_typename, const s2string &element_name, unsigned int count);
	
private:
	TypeMap types;
	CompatibleMap compatibles;
};

}

#endif			//SHADER_TYPEINFO_MANAGER_H_