#include "type_info.h"
#include "type_info_helper.h"

#include <glog/logging.h>
#include <stdio.h>
#include <stdlib.h>

namespace s2 {

namespace {

class TypeInfoStruct : public TypeInfo {
public:
	TypeInfoStruct(const s2string &new_name, unsigned int new_size, const Members &new_members) 
			: name(new_name), size(new_size), members(new_members) {}

	virtual const TypeInfo & GetMemberType(unsigned int index) const {
		return TypeInfoManager::GetSingleton()->Get(members[index].type_name);
	}
	
	virtual unsigned int GetMemberOffset(unsigned int index) const {
		return members[index].offset;
	}
	
	virtual s2string GetMemberName(unsigned int index) const {
		return members[index].name;
	}
	
	virtual unsigned int GetMemberIndex(const s2string &member_name) const {
		for(unsigned int i=0; i<members.size(); i++) {
			if(members[i].name == member_name)
				return i;
		}
		LOG(FATAL)<<"Cannot find the member ["<<member_name<<"] in side type ["<<name<<"]";
		return 0;
	}
	
	virtual unsigned int GetMemberSize() const {
		return members.size();
	}
	
	virtual bool HasMember(const s2string &member_name) const {
		for(Members::const_iterator it=members.begin(); it != members.end(); it++) {
			if(it->name == member_name)
				return true;
		}
		return false;
	}

	virtual s2string GetName() const {
		return name;
	}
	
	virtual unsigned int GetSize() const {
		return size;
	}
	
private:
	TypeInfoStruct(const TypeInfoStruct &);
	TypeInfoStruct & operator=(const TypeInfoStruct &);
	
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

class TypeInfoPrimitive : public TypeInfo   {
public:
	TypeInfoPrimitive(const s2string &_name, unsigned int _size) :
		name(_name), size(_size){}

	virtual const TypeInfo & GetMemberType(unsigned int index) const {
		CHECK(false)<<"This is primitive type. No member.";
		return *(new TypeInfoPrimitive("",0));
	}
	
	virtual unsigned int GetMemberOffset(unsigned int index) const {
		CHECK(false)<<"This is primitive type. No member.";
		return 0;
	}
	virtual s2string GetMemberName(unsigned int index) const {
		CHECK(false)<<"This is primitive type. No member.";
		return *(new s2string());
	}
	virtual unsigned int GetMemberIndex(const s2string &member_name) const {
		CHECK(false)<<"This is primitive type. No member.";
		return 0;
	}
	
	virtual unsigned int GetMemberSize() const {
		return 0;
	}
	
	virtual bool HasMember(const s2string &member_name) const {
		return false;
	}

	virtual s2string GetName() const {
		return name;
	}
	
	virtual unsigned int GetSize() const {
		return size;
	}
	
private:
	TypeInfoPrimitive(const TypeInfoPrimitive &);
	TypeInfoPrimitive & operator=(const TypeInfoPrimitive &);
	
private:
	s2string name;
	unsigned int size;
};

class SimpleLexer {
public:
	SimpleLexer(const s2string &_data) : data(_data) {
		start = data.c_str();
		cur = start;
	}

	bool ExpectTypename(s2string *result) {
		const char *type_start = cur;
		bool succeed = false;
		while(ExpectWord(0)) {
			succeed = true;
			Expect(" ");
		}
		if(result)
			*result=s2string(type_start, cur-type_start);
		return succeed;
	}

	bool ExpectWord(s2string *result) {
		if(!IsAlphabetic(cur))
			return false;
		const char *word_start = cur;
		cur++;
		while(cur!='\0' && IsAlphanumeric(cur)) {
			cur++;
		}
		if(result)
			*result = s2string(word_start, cur-word_start);
		return true;
	}
	
	//return 0 as an error code.
	unsigned int ExpectPositveNumber() {
		return strtol(cur, (char **)&cur, 10);
	}
	
	bool Expect(const s2string &expected) {
		const char *backup = cur;
		const char *c = expected.c_str();
		while(*cur!='\0' && *c!='\0') {
			if(*cur != *c) {
				cur = backup;
				return false;
			}
			cur++;
			c++;
		}
		if(*c=='\0')
			return true;
		else {
			cur = backup;
			return false;
		}
	}
	
	bool IsEnd() {
		return *cur == '\0';
	}
	
private:
	static bool IsAlphanumeric(const char *c) {
		return IsAlphabetic(c) || IsNumeric(c);
	}
	
	static bool IsAlphabetic(const char *c) {
		return (*c>='a' && *c<='z') || (*c>='A' && *c<='Z');
	}
	
	static bool IsNumeric(const char *c) {
		return *c>='0' && *c<='9';
	}
	
private:
	s2string data;
	const char *start;
	const char *cur;
};


class TypeInfoArray  : public TypeInfo {
public:
	TypeInfoArray(const s2string &_name) : name(_name) {
		SimpleLexer lexer(name);
		
		CHECK(lexer.ExpectTypename(&e_name))<<"Bad array typename "<<name;
		e_count = 1;
		while(!lexer.IsEnd()) {
			CHECK(lexer.Expect("["))<<"Bad array typename "<<name;
			unsigned int count = lexer.ExpectPositveNumber();
			e_count *= count;
			CHECK(lexer.Expect("]"))<<"Bad array typename "<<name;
		}
	}

	virtual const TypeInfo & GetMemberType(unsigned int index) const {
		return TypeInfoManager::GetSingleton()->Get(e_name);
	}
	
	virtual unsigned int GetMemberOffset(unsigned int index) const {
		return GetMemberType(index).GetSize()*index;
	}
	
	virtual s2string GetMemberName(unsigned int index) const {
		CHECK(index<e_count)<<"Try index "<<index<<" while max index is "<<e_count-1;
		s2string buf;
		S2StringFormat(&buf, "%d", index);
		return e_name;
	}
	
	virtual unsigned int GetMemberIndex(const s2string &member_name) const {
		unsigned int result = atoi(member_name.c_str());
		CHECK(result<e_count)<<member_name<<" must be a number smaller than "<<e_count;
		return result;
	}
	
	virtual unsigned int GetMemberSize() const {
		return e_count;
	}
	
	virtual bool HasMember(const s2string &member_name) const {
		unsigned int index = atoi(member_name.c_str());
		return index>=0 && index<e_count;
	}

	virtual s2string GetName() const {
		return name;
	}
	
	virtual unsigned int GetSize() const {
		return e_count * GetMemberType(0).GetSize();
	}
	
	static bool IsArrayTypename(const s2string &name) {
		SimpleLexer lexer(name);
		if(!lexer.ExpectTypename(0)) {
			return false;
		}
		bool is_array = false;
		while(!lexer.IsEnd()) {
			if(!lexer.Expect("[")) {
				return false;
			}
			lexer.ExpectPositveNumber();
			if(lexer.Expect("]")) {
				return false;
			}
			is_array = true;
		}
		return is_array;
	}
	
private:
	TypeInfoArray(const TypeInfoArray &);
	TypeInfoArray & operator=(const TypeInfoArray &);
	
private:
	s2string name;
	s2string e_name;
	unsigned int e_count;
};

}


TypeInfoManager::~TypeInfoManager() {
	for(Data::iterator it=data.begin(); it!=data.end(); it++) {
		delete it->second;
	}
	data.clear();
}

const TypeInfo & TypeInfoManager::Get(const s2string &name) const {
	//It could be an array.
	if(name.find('[') != std::string::npos && !Has(name)) {
		TypeInfo *new_typeinfo = new TypeInfoArray(name);
		s2string element_typename = new_typeinfo->GetMemberName(0);
		CHECK(Has(element_typename))<<
					"Element type "<<element_typename<<" cannot be found for "<<name;
		data[name] = new_typeinfo;
	}

	return *(data.at(name));
}

const bool TypeInfoManager::Has(const s2string &name) const {
	//It could be an array.
	if(data.find(name)==data.end() && TypeInfoArray::IsArrayTypename(name)) {
		TypeInfo *new_typeinfo = new TypeInfoArray(name);	
		s2string element_typename = new_typeinfo->GetMemberName(0);
		if(data.find(element_typename)!=data.end()) {
			data[name] = new_typeinfo;
		}
	}
	return data.find(name) != data.end();
}

const TypeInfo & TypeInfoManager::CreateStruct(const s2string &name, unsigned int size, const TypeInfo::Members &new_members) {
	CHECK(Has(name) == false)<<"Try to create a typeinfo ["<<name<<"] that exists.";
	TypeInfo *new_typeinfo = new TypeInfoStruct(name, size, new_members);
	data[name] = new_typeinfo;
	return *new_typeinfo;	
}

const TypeInfo & TypeInfoManager::CreatePrimitive(const s2string &name, unsigned int size) {
	CHECK(Has(name) == false)<<"Try to create a typeinfo ["<<name<<"] that exists.";
	TypeInfo *new_typeinfo = new TypeInfoPrimitive(name, size);
	data[name] = new_typeinfo;
	return *new_typeinfo;	
}

}