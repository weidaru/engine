#ifndef VERTEX_BUFFER_H_
#define VERTEX_BUFFER_H_

#include "resource.h"

namespace s2 {

class VertexBuffer : public Resource {
public:
	virtual 				~VertexBuffer() {}
	virtual void 			Initialize(int size, const void *data, bool is_dynamic) = 0;
	virtual bool 			IsDynamic() = 0;
	virtual unsigned int 	GetSize() = 0;
	virtual void * 			Map() = 0;
	virtual void 			UnMap() = 0;
};

}

#endif 	//VERTEX_BUFFER_H_