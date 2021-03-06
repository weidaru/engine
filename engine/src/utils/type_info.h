#ifndef TYPE_INFO_H_
#define TYPE_INFO_H_

#include <map>
#include <vector>
#include <stdint.h>

#include "s2string.h"
#include "singleton.h"
#include "type_info_bind.h"

namespace s2 {

/**
 * I am trying to be pessimistic about the TypeInfo system. Any invalid getter(e.g. ask for the TypeInfo by name) 
 * or setter(e.g. create a typeinfo form manager with already existed name) to TypeInfo and TypeInfoManager will result
 * in an assertion or error. Use corresponding Has* API for querying existence.
 *
 * Supported primitive type are
 * int, unsinged int, char, unsigned char, float, double, bool
 * All pointer types have same TypeInfo with name "pointer" and all reference types have same TypeInfo with name "reference"
 * 
 * The size of reference really means the size of memory in byte to store a reference.
 * MSVC c++ 2010 treats sizeof(T &) same as sizeof(T), which is not what we want.
 * Need to verify whether that is the standard behavior.
 */
 
class TypeInfoManager;
 
 /**
  * Interface for supporting runtime typeinfo. 
  *
  * In general case, TypeInfo initialization is handled by script through annotation tag, for example
  * //[[TypeInfo]]//
  * struct Foo {
  * 	int a;
  *		float b;
  * }
  * This allows the script to generate type info for run-time. See more in the script/generate_type_info.lua
  * All the type name and member name will be exact the same as the struct declaration.
  * Padding won't be a problem as all the information is told by the compiler, sizeof etc.
  *
  * Things won't work now:
  * 	It won't work for struct having virtual functions as its implementation varies from compiler to compiler. 
  */
class TypeInfo  {
public:
	struct Member {
		s2string 		type_name;
		s2string 		name;
		uint32_t 	offset;
	};
	typedef std::vector<Member> Members;

public:
	virtual ~TypeInfo() {}
	virtual const TypeInfo & GetMemberType(uint32_t index) const = 0;
	virtual uint32_t GetMemberOffset(uint32_t index) const = 0;
	virtual s2string GetMemberName(uint32_t index) const = 0;
	virtual uint32_t GetMemberIndex(const s2string &member_name) const = 0;
	virtual uint32_t GetMemberSize() const = 0;
	virtual bool HasMember(const s2string &member_name) const = 0;

	virtual s2string GetName() const = 0;
	virtual uint32_t GetSize() const = 0;
	
	const s2string & GetCustomMetadata(const s2string &key) const;
	bool HasCustomMetadata(const s2string &key) const;
	
	void SetCustomMetadata(const s2string &key, const s2string &value);
	
private:
	typedef std::map<s2string, s2string> CustomData;
	CustomData custom_data;
};

class TypeInfoManager : public Singleton<TypeInfoManager>{
private:
	typedef std::map<s2string, TypeInfo *> Data;

public:
	virtual ~TypeInfoManager();
	
	/*
	 * Get also accepts typenames that can be considered as array.
	 * It should matches exactly as  
	 * array_typename ::= typename ("["positive-number"]")
	 * typename ::= word | typename " " word
	 * word ::= !"[a-zA-Z]%w*"
	 * blank ::= !"%s"
	 * positive-number ::= !"[1-9]%d*"
	 */
	const TypeInfo & 	Get(const s2string &name) const;
	template<typename T>
	const TypeInfo &	Get() const {
		return Get(TypeInfoBind<T>::GetName());
	}
	/*
	 * Array is also considered, see @Get
	 */
	const bool			Has(const s2string &name) const;
	
	/**
	 * Don't call the following unless you know what you are doing.
	 */
	TypeInfo & CreateStruct(const s2string &name, uint32_t size, const TypeInfo::Members &new_members);
	TypeInfo & CreatePrimitive(const s2string &name, uint32_t size);
	
private:
	mutable Data data;
};


}

#endif		//TYPE_INFO_H_