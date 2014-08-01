#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <string>

namespace s2 {

class Resource {
public:
	Resource ();
	virtual ~Resource();
	
	unsigned int 			GetID() const;
	std::string 			GetName() const;
	Resource & 				SetName(const std::string &new_name);
	
private:
	static unsigned int & 	GetCurrentID();
	Resource(const Resource &);
	Resource & operator=(const Resource &);
	
private:
	unsigned int 	id;
	std::string 	name;
};

}

#endif		//RESOURCE_H_