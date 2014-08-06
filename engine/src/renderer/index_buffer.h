#ifndef INDEX_BUFFER_H_
#define INDEX_BUFFER_H_

#include "resource.h"

namespace s2 {

class IndexBuffer : public Resource {
public:
	virtual 				~IndexBuffer() {}
	virtual void 			Initialize(unsigned int size, const void *data, bool is_dynamic) = 0;
	virtual bool 			IsDynamic() = 0;
	virtual unsigned int 	GetSize() = 0;
	virtual void * 			Map() = 0;
	virtual void 			UnMap() = 0;
};

}

#endif		//INDEX_BUFFER_H_