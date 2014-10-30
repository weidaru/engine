#ifndef VERTEX_BUFFER_H_
#define VERTEX_BUFFER_H_

#include "resource.h"
#include "utils/s2string.h"
#include "utils/type_info.h"

namespace s2 {

class VertexBuffer : public Resource {
public:
	virtual 						~VertexBuffer() {}
	virtual void 				Initialize(unsigned int element_count, unsigned int per_ele_size, const void *data, bool is_dynamic)= 0;
	template <typename T>
	void Initialize(unsigned int element_count, const T *data, bool is_dynamic) {
		Initialize(element_count, TypeInfoManager::GetSingleton()->Get<T>(), (const void *)data, is_dynamic );
	}
	virtual bool 				IsDynamic() = 0;
	virtual unsigned int 	GetElementCount();
	virtual unsigned int 	GetElementBytewidth();
	//VertexBuffer with raw data, not initialized or initialized through 
	//void Initialize(unsigned int size, const void *data, bool is_dynamic)
	//will return empty string.
	virtual s2string 			GetElementTypeName();
	virtual void * 			Map() = 0;
	virtual void 				UnMap() = 0;
	
protected:
	virtual void Initialize(unsigned int element_count, const TypeInfo &type_info, const void *data, bool is_dynamic);
};

}

#endif 	//VERTEX_BUFFER_H_