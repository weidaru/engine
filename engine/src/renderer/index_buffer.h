#ifndef INDEX_BUFFER_H_
#define INDEX_BUFFER_H_

#include "resource.h"
#include "utils/s2string.h"
#include "renderer_enum.h"

namespace s2 {

class IndexBuffer : public Resource {
public:
	typedef unsigned int InputType;

public:
	virtual 					~IndexBuffer() {}
	virtual void 			Initialize(unsigned int element_count, const InputType *data, RendererEnum::ResourceWrite resource_write) = 0;
	virtual unsigned int GetElementCount() const = 0;
	
	virtual RendererEnum::ResourceWrite GetResourceWrite() const = 0;
	virtual void WriteMap(bool is_partial_map) = 0;
	virtual void Write(unsigned int index, const InputType *data, unsigned int array_size) = 0;
	virtual void WriteUnmap() = 0;
	
	virtual void Update(unsigned int index, const InputType *data, unsigned int array_size) = 0;
};

}

#endif		//INDEX_BUFFER_H_