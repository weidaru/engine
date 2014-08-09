#ifndef TYPE_INFO_H_
#define TYPE_INFO_H_

#include <map>
#include <vector>

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
 
class TypeInfo {
public:
	//member name and type name pair.
	typedef std::vector<std::pair<s2string, s2string> > Members;

public:
	const TypeInfo &		GetMember(unsigned int index) const;
	const s2string &		GetMemberName(unsigned int index) const;
	const TypeInfo & 		GetMember(const s2string &member_name) const;
	unsigned int 			GetMemberIndex(const s2string &member_name) const;
	unsigned int 			GetMemberSize() const;
	bool					HasMember(const s2string &member_name) const;

	s2string 				GetName() const;
	unsigned int 			GetSize() const;
	
private:
	TypeInfo(const s2string &new_name, unsigned int size, const Members &new_members);
	TypeInfo(const TypeInfo &);
	TypeInfo & operator=(const TypeInfo &);

	friend class TypeInfoManager;
	
private:
	/**
	 * Assume members size will be small, so use a vector should be fine.
	 * As makeing assumption about initialization order(register with TypeInfoManager) is 
	 * usually a bad practice so just store member name and type name.
	 */
	Members members;
	s2string name;
	unsigned int size;
};

class TypeInfoManager : public Singleton<TypeInfoManager>{
private:
	typedef std::map<s2string, TypeInfo *> Data;

public:
	virtual ~TypeInfoManager();
	
	const TypeInfo & 	Get(const s2string &name) const;
	template<typename T>
	const TypeInfo &	Get() const {
		return Get(TypeInfoBind<T>::GetName());
	}
	const bool			Has(const s2string &name) const;
	
	/**
	 * Don't call this unless you know what you are doing.
	 */
	const TypeInfo &	Create(const s2string &name, unsigned int size, const TypeInfo::Members &new_members);
	
private:
	Data data;
};


}

#endif		//TYPE_INFO_H_