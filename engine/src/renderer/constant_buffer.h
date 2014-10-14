#ifndef CONSTANT_BUFFER_H_
#define CONSTANT_BUFFER_H_

#include "resource.h"
#include "utils/s2string.h"

namespace s2 {

class ConstantBuffer : public Resource {
public:
	virtual					~ConstantBuffer() {}
	virtual void 			Initialize(unsigned int size, const void *data) = 0;
	virtual unsigned int GetSize() = 0;
 	virtual void * 			Map() = 0;
 	virtual void 			UnMap() = 0;
};

}

#endif 		//CONSTANT_BUFFER_H_