#include "type_info.h"
#include "type_info_helper.h"

//#include <glog/logging.h>

namespace s2 {

TypeInfo::TypeInfo(const s2string &new_name, unsigned int new_size, const Members &new_members)
	: name(new_name), size(new_size), members(new_members) {}

const TypeInfo & TypeInfo::GetMemberType(unsigned int index) const {
	return TypeInfoManager::GetSingleton()->Get(members[index].type_name);
}

unsigned int TypeInfo::GetMemberOffset(unsigned int index) const {
	return members[index].offset;
}

unsigned int TypeInfo::GetMemberIndex(const s2string &member_name) const {
	for(unsigned int i=0; i<members.size(); i++) {
		if(members[i].name == member_name)
			return i;
	}
	//LOG(FATAL)<<"Cannot find the member ["<<member_name<<"] in side type ["<<name<<"]";
	return 0;
}

bool TypeInfo::HasMember(const s2string &member_name) const {
	for(Members::const_iterator it=members.begin(); it != members.end(); it++) {
		if(it->name == member_name)
			return true;
	}
	return false;
}

const s2string & TypeInfo::GetMemberName(unsigned int index) const {
	return members[index].name;
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
	//CHECK(Has(name) == false)<<"Try to create a typeinfo ["<<name<<"] that exists.";
	TypeInfo *new_typeinfo = new TypeInfo(name, size, new_members);
	data[name] = new_typeinfo;
	return *new_typeinfo;	
}

}