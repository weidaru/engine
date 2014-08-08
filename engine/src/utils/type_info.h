#ifndef TYPE_INFO_H_
#define TYPE_INFO_H_

#include <map>
#include <vector>

#include "s2string.h"
#include "singleton.h"


namespace s2 {


class TypeInfo {
private:
	typedef std::vector<std::pair<s2string, s2string> > Members;

public:
	const TypeInfo &		GetMember(unsigned int index, s2string *member_name);
	const TypeInfo & 		GetMember(const s2string &member_name);
	

	s2string 				GetName();
	unsigned int 			GetSize();
	
private:
	TypeInfo(const TypeInfo &);
	TypeInfo & operator=(const TypeInfo &);
	
private:
	/**
	 * Assume members size will be small, so use a vector should be fine.
	 * As makeing assumption about initialization order(register with TypeInfoManager) is usually a bad practice
	 * so just store member name and type name.
	 */
	Members members;
	s2string name;
	unsigned int size;
};

class TypeInfoManager : public Singleton{
private:
	typedef std::map<s2string, TypeInfo *> Data;

public:
	TypeInfoManager();
	virtual ~TypeInfoManager();
	
	const TypeInfo & 	Get(const s2string &name);
	void 				Add(TypeInfo *type);
	
private:
	Data data;
};


}

#endif		//TYPE_INFO_H_