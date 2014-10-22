#include "shader_typeinfo_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

#include <glog/logging.h>

namespace s2 {

namespace {

class ShaderTypeInfoScalar : public TypeInfo   {
public:
	ShaderTypeInfoScalar(ShaderTypeInfoManager *_manager, const s2string &_name, unsigned int _size) :
		manager(_manager), name(_name), size(_size){}

	virtual const TypeInfo & GetMemberType(unsigned int index) const {
		CHECK(false)<<"This is primitive type. No member.";
		return *(new ShaderTypeInfoScalar(0, "",0));
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
	ShaderTypeInfoScalar(const ShaderTypeInfoScalar &);
	ShaderTypeInfoScalar & operator=(const ShaderTypeInfoScalar &);
	
private:
	ShaderTypeInfoManager *manager;
	s2string name;
	unsigned int size;
};

class ShaderTypeInfoStruct : public TypeInfo {
public:
	ShaderTypeInfoStruct(ShaderTypeInfoManager *_manager, const s2string &new_name, unsigned int new_size, const Members &new_members) 
			: manager(_manager), name(new_name), size(new_size), members(new_members) {}

	virtual const TypeInfo & GetMemberType(unsigned int index) const {
		return manager->GetTypeInfo(members[index].type_name);
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
	ShaderTypeInfoStruct(const ShaderTypeInfoStruct &);
	ShaderTypeInfoStruct & operator=(const ShaderTypeInfoStruct &);
	
private:
	/**
	 * Assume members size will be small, so use a vector should be fine.
	 * As makeing assumption about initialization order(register with TypeInfoManager) is 
	 * usually a bad practice so just store member name and type name.
	 */
	ShaderTypeInfoManager *manager;
	Members members;
	s2string name;
	unsigned int size;
};

unsigned int Pack16Byte(unsigned int size) {
	return size%16==0 ? size : (size/16+1)*16;
}

unsigned int Pack4Byte(unsigned int size) {
	return size%4==0 ? size : (size/4+1)*4;
}

class ShaderTypeInfoArray  : public TypeInfo {
public:
	ShaderTypeInfoArray(ShaderTypeInfoManager *_manager, const s2string &shader_typename, const s2string &element_name, unsigned int count) 
			: manager(_manager), name(shader_typename), e_typename(element_name), e_count(count) {
	}

	virtual const TypeInfo & GetMemberType(unsigned int index) const {
		return manager->GetTypeInfo(e_typename);
	}
	
	virtual unsigned int GetMemberOffset(unsigned int index) const {
		return Pack16Byte(GetMemberType(index).GetSize())*index;
	}
	
	virtual s2string GetMemberName(unsigned int index) const {
		CHECK(index < e_count)<<"Try index "<<index<<" while max index is "<<e_count-1;;
		char buf[512];
		sprintf(buf, "%d", index);
		return buf;
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
		unsigned int member_size = GetMemberType(0).GetSize();
		return member_size + Pack16Byte(member_size)*(e_count-1);
	}
	
private:
	ShaderTypeInfoArray(const ShaderTypeInfoArray &);
	ShaderTypeInfoArray & operator=(const ShaderTypeInfoArray &);
	
private:
	ShaderTypeInfoManager *manager;
	s2string name;
	s2string e_typename;
	unsigned int e_count;
};

class ShaderTypeInfoVector  : public TypeInfo {
public:
	ShaderTypeInfoVector(ShaderTypeInfoManager *_manager, const s2string &shader_typename, const s2string &primitive_typename, unsigned int _count) 
			: manager(_manager), name(shader_typename), s_typename(primitive_typename), count(_count) {
		
	}

	virtual const TypeInfo & GetMemberType(unsigned int index) const {
		const TypeInfo &p = manager->GetTypeInfo(s_typename);
		CHECK(p.GetMemberSize() == 0)<<"Vector can only be associated with a primitive.";
		return p;
	}
	
	virtual unsigned int GetMemberOffset(unsigned int index) const {
		return GetMemberType(index).GetSize()*index;
	}
	
	virtual s2string GetMemberName(unsigned int index) const {
		CHECK(index < count)<<"Try index "<<index<<" while max index is "<<count-1;;
		char buf[512];
		sprintf(buf, "%d", index);
		return buf;
	}
	
	virtual unsigned int GetMemberIndex(const s2string &member_name) const {
		unsigned int result = atoi(member_name.c_str());
		CHECK(result<count)<<member_name<<" must be a number smaller than "<<count;
		return result;
	}
	
	virtual unsigned int GetMemberSize() const {
		return count;
	}
	
	virtual bool HasMember(const s2string &member_name) const {
		unsigned int index = atoi(member_name.c_str());
		return index>=0 && index<count;
	}

	virtual s2string GetName() const {
		return name;
	}
	
	virtual unsigned int GetSize() const {
		return count * Pack4Byte(GetMemberType(0).GetSize());
	}
	
private:
	ShaderTypeInfoVector(const ShaderTypeInfoVector &);
	ShaderTypeInfoVector & operator=(const ShaderTypeInfoVector &);
	
private:
	ShaderTypeInfoManager *manager;
	s2string name;
	s2string s_typename;
	unsigned int count;
};

//Column major matrix
class ShaderTypeInfoMatrix  : public TypeInfo {
public:
	ShaderTypeInfoMatrix(ShaderTypeInfoManager *_manager, const s2string &shader_typename, const s2string &primitive_typename, unsigned int _rows, unsigned int _columns) 
			: manager(_manager), name(shader_typename), s_typename(primitive_typename), rows(_rows), columns(_columns) {
		
	}

	virtual const TypeInfo & GetMemberType(unsigned int index) const {
		CHECK(index<rows*columns)<<"Try index "<<index<<" while max index is "<<rows*columns-1;
		const TypeInfo &p = manager->GetTypeInfo(s_typename);
		CHECK(p.GetMemberSize() == 0)<<"Vector can only be associated with a primitive.";
		return p;
	}
	
	virtual unsigned int GetMemberOffset(unsigned int index) const {
		return GetMemberType(index).GetSize()*index;
	}
	
	virtual s2string GetMemberName(unsigned int index) const {
		CHECK(index < rows*columns)<<"Try index "<<index<<" while max index is "<<rows*columns-1;
		char buf[512];
		sprintf(buf, "%d", index);
		return buf;
	}
	
	virtual unsigned int GetMemberIndex(const s2string &member_name) const {
		unsigned int result = atoi(member_name.c_str());
		CHECK(result<rows*columns)<<member_name<<" must be a number smaller than "<<rows*columns;
		return result;
	}
	
	virtual unsigned int GetMemberSize() const {
		return rows*columns;
	}
	
	virtual bool HasMember(const s2string &member_name) const {
		unsigned int index = atoi(member_name.c_str());
		return index>=0 && index<rows*columns;
	}

	virtual s2string GetName() const {
		return name;
	}
	
	virtual unsigned int GetSize() const {
		return rows*columns* Pack4Byte(GetMemberType(0).GetSize());
	}
	
private:
	ShaderTypeInfoMatrix(const ShaderTypeInfoMatrix &);
	ShaderTypeInfoMatrix & operator=(const ShaderTypeInfoMatrix &);
	
private:
	ShaderTypeInfoManager *manager;
	s2string name;
	s2string s_typename;
	unsigned int rows;
	unsigned int columns;
};

}

bool ShaderTypeInfoManager::CheckCompatible(const s2string &shader_typename, const s2string &cpp_type) const {
	if(compatibles.find(shader_typename)!=compatibles.end()) {
		const CompatibleMap::mapped_type &vec = compatibles.at(shader_typename);
		return std::find(vec.begin(), vec.end(), cpp_type) != vec.end(); 
	}
	return false;
}

void ShaderTypeInfoManager::MakeCompatible(const s2string &shader_typename, const s2string &cpp_type) {
	CompatibleMap::mapped_type &vec = compatibles[shader_typename];
	if(std::find(vec.begin(), vec.end(), cpp_type) == vec.end())
		vec.push_back(cpp_type);
}

const TypeInfo & ShaderTypeInfoManager::GetTypeInfo(const s2string &shader_typename) const {
	return *types.at(shader_typename);
}

bool ShaderTypeInfoManager::HasTypeInfo(const s2string &shader_typename) const {
	return types.find(shader_typename)!=types.end();
}

const TypeInfo & ShaderTypeInfoManager::CreateScalar(const s2string &shader_typename, unsigned int size) {
	CHECK(HasTypeInfo(shader_typename) == false)<<"Try to create a typeinfo ["<<shader_typename<<"] that exists.";
	TypeInfo *new_typeinfo = new ShaderTypeInfoScalar(this, shader_typename, size);
	types[shader_typename] = new_typeinfo;
	return *new_typeinfo;	
}

const TypeInfo & ShaderTypeInfoManager::CreateVector(const s2string &shader_typename, const s2string &scalar_typename, unsigned int count) {
	CHECK(HasTypeInfo(shader_typename) == false)<<"Try to create a typeinfo ["<<shader_typename<<"] that exists.";
	TypeInfo *new_typeinfo = new ShaderTypeInfoVector(this, shader_typename, scalar_typename, count);
	types[shader_typename] = new_typeinfo;
	return *new_typeinfo;	
}

const TypeInfo & ShaderTypeInfoManager::CreateMatrix(const s2string &shader_typename, const s2string &scalar_typename, unsigned int rows, unsigned int columns) {
	CHECK(HasTypeInfo(shader_typename) == false)<<"Try to create a typeinfo ["<<shader_typename<<"] that exists.";
	TypeInfo *new_typeinfo = new ShaderTypeInfoMatrix(this, shader_typename, scalar_typename, rows, columns);
	types[shader_typename] = new_typeinfo;
	return *new_typeinfo;	
}

const TypeInfo & ShaderTypeInfoManager::CreateStruct(const s2string &shader_typename, unsigned int size, const TypeInfo::Members &members) {
	CHECK(HasTypeInfo(shader_typename) == false)<<"Try to create a typeinfo ["<<shader_typename<<"] that exists.";
	TypeInfo *new_typeinfo = new ShaderTypeInfoStruct(this, shader_typename, size, members);
	types[shader_typename] = new_typeinfo;
	return *new_typeinfo;	
}

const TypeInfo & ShaderTypeInfoManager::CreateArray(const s2string &shader_typename, const s2string &element_name, unsigned int count) {
	CHECK(HasTypeInfo(shader_typename) == false)<<"Try to create a typeinfo ["<<shader_typename<<"] that exists.";
	TypeInfo *new_typeinfo = new ShaderTypeInfoArray(this, shader_typename, element_name, count);
	types[shader_typename] = new_typeinfo;
	return *new_typeinfo;	
}

}