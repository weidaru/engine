#ifndef INDEX_BUFFER_H_
#define INDEX_BUFFER_H_

#include "resource.h"
#include "utils/s2string.h"
#include "general_enum.h"

namespace s2 {

class IndexBuffer : public Resource {
public:
	typedef unsigned int InputType;

public:
	virtual 					~IndexBuffer() {}
	virtual void 			Initialize(unsigned int element_count, const InputType *data, GeneralEnum::MapBehavior map_behavior) = 0;
	virtual unsigned int GetElementCount() const = 0;
	
	virtual GeneralEnum::MapBehavior GetMapBehavior() const = 0;
	virtual void Map(bool is_partial_map) = 0;
	virtual void Write(unsigned int index, const InputType *data, unsigned int array_size) = 0;
	virtual const IndexBuffer::InputType * Read(unsigned int index) const = 0;
	virtual void UnMap() = 0;
	
	virtual void Update(unsigned int index, const InputType *data, unsigned int array_size) = 0;
};

}

#endif		//INDEX_BUFFER_H_