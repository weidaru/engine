#ifndef INDEX_BUFFER_H_
#define INDEX_BUFFER_H_

#include "resource.h"
#include "utils/s2string.h"

namespace s2 {

class IndexBuffer : public Resource {
public:
	typedef unsigned int InputType;

public:
	virtual 					~IndexBuffer() {}
	virtual void 			Initialize(unsigned int element_count, const InputType *data, bool is_dynamic) = 0;
	virtual bool 			IsDynamic() const = 0;
	virtual unsigned int GetElementCount() const = 0;
	virtual void * 			Map() = 0;
	virtual void 			UnMap() = 0;
};

}

#endif		//INDEX_BUFFER_H_