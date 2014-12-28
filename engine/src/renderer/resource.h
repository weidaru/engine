#ifndef RESOURCE_H_
#define RESOURCE_H_

#include "utils/s2string.h"

namespace s2 {

class Resource {
public:
	Resource ();
	virtual ~Resource();
	
	unsigned int 			GetID() const;
	s2string 				GetName() const;
	s2string				GetIDAndName() const;
	/**
	 * Yes, name can change. Use it for debug.
	 */
	Resource & 				SetName(const s2string &new_name);
	
private:
	static unsigned int & 	GetCurrentID();
	Resource(const Resource &);
	Resource & operator=(const Resource &);
	
private:
	unsigned int 	id;
	s2string 	name;
};

}

#endif		//RESOURCE_H_