#ifndef INDEX_BUFFER_H_
#define INDEX_BUFFER_H_

#include "resource.h"
#include "utils/s2string.h"
#include "mappable.h"

namespace s2 {

class IndexBuffer : public Resource, public Mappable {
public:
	typedef unsigned int InputType;

public:
	virtual 					~IndexBuffer() {}
	virtual void 			Initialize(unsigned int element_count, const InputType *data, GeneralEnum::MapBehavior map_behavior) = 0;
	virtual GeneralEnum::MapBehavior GetMapBehavior() const = 0;
	virtual unsigned int GetElementCount() const = 0;
	void Update(unsigned int offset, InputType *data, unsigned int array_size) {
		this->Update(offset, (void *)data, sizeof(InputType)*array_size);
	}
};

}

#endif		//INDEX_BUFFER_H_