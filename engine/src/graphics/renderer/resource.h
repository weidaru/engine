#ifndef RESOURCE_H_
#define RESOURCE_H_

#include "utils/s2string.h"
#include <stdint.h>

namespace s2 {

class Resource {
public:
	Resource ();
	virtual ~Resource();
	
	uint32_t 			GetID() const;
	s2string 				GetName() const;
	s2string				GetIDAndName() const;
	/**
	 * Yes, name can change. Use it for debug.
	 */
	Resource & 				SetName(const s2string &new_name);
	
private:
	static uint32_t & 	GetCurrentID();
	Resource(const Resource &);
	Resource & operator=(const Resource &);
	
private:
	uint32_t 	id;
	s2string 	name;
};

}

#endif		//RESOURCE_H_