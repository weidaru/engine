#include "type_info.h"
#include "type_info_helper.h"

#include <glog/logging.h>

namespace s2 {

TypeInfo::TypeInfo(const s2string &new_name, unsigned int new_size, const Members &new_members)
	: name(new_name), size(new_size), members(new_members) {}

const TypeInfo & TypeInfo::GetMember(unsigned int index) const {
	return TypeInfoManager::GetSingleton()->Get(members[index].second);
}

/**
 * Get member by name is a linear search. As the member size should be small,
 * I won't expect any performances issue.
 */
const TypeInfo & TypeInfo::GetMember(const s2string &member_name) const {
	for(Members::const_iterator it=members.begin(); it != members.end(); it++) {
		if(it->first == member_name)
			return TypeInfoManager::GetSingleton()->Get(it->second);
	}
	LOG(ERROR)<<"Cannot find the member ["<<member_name<<"] in side type ["<<name<<"]";
	return TypeInfoManager::GetSingleton()->Get(null_typename);
}

unsigned int TypeInfo::GetMemberIndex(const s2string &member_name) const {
	for(unsigned int i=0; i<members.size(); i++) {
		if(members[i].first == member_name)
			return i;
	}
	LOG(ERROR)<<"Cannot find the member ["<<member_name<<"] in side type ["<<name<<"]";
	return 0;
}

bool TypeInfo::HasMember(const s2string &member_name) const {
	for(Members::const_iterator it=members.begin(); it != members.end(); it++) {
		if(it->first == member_name)
			return true;
	}
	return false;
}

const s2string & TypeInfo::GetMemberName(unsigned int index) const {
	return members[index].first;
}

unsigned int TypeInfo::GetMemberSize() const {
	return members.size();
}

s2string TypeInfo::GetName() const {
	return name;
}

unsigned int TypeInfo::GetSize() const {
	return size;
}

TypeInfoManager::~TypeInfoManager() {
	for(Data::iterator it=data.begin(); it!=data.end(); it++) {
		delete it->second;
	}
	data.clear();
}

const TypeInfo & TypeInfoManager::Get(const s2string &name) const {
	return *(data.at(name));
}

const bool TypeInfoManager::Has(const s2string &name) const {
	return data.find(name) != data.end();
}

const TypeInfo & TypeInfoManager::Create(const s2string &name, unsigned int size, const TypeInfo::Members &new_members) {
	CHECK(Has(name) == false)<<"Try to create a typeinfo ["<<name<<"] that exists.";
	TypeInfo *new_typeinfo = new TypeInfo(name, size, new_members);
	data[name] = new_typeinfo;
	return *new_typeinfo;	
}

#define DefinePrimitive(Name) \
DeclareTypeInfo(Name) \
DefineTypeInfo(Name) { \
	TypeInfoManager::GetSingleton()->Create( \
			TypeInfoBind<Name>::GetName(), \
			sizeof(Name), \
			TypeInfo::Members()); \
}

/**
 * Define all the primitive type here. These are the things get supported now.
 */
DefinePrimitive(int)
DefinePrimitive(unsigned int)
DefinePrimitive(char)
DefinePrimitive(unsigned char)
DefinePrimitive(float)
DefinePrimitive(double)
DefinePrimitive(bool)
 
#undef DefinePrimitive

namespace internal {
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

}