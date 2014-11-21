#ifndef VERTEX_BUFFER_H_
#define VERTEX_BUFFER_H_

#include "resource.h"
#include "utils/s2string.h"
#include "utils/type_info.h"
#include "mappable.h"

namespace s2 {

class VertexBuffer : public Resource, Mappable {
public:
	virtual 						~VertexBuffer() {}
	virtual void 				Initialize(	unsigned int element_count,unsigned int element_member_count,
													unsigned int per_ele_size, const void *data, GeneralEnum::MapBehavior map_behavior)= 0;
	template <typename T>
	void Initialize(unsigned int element_count, const T *data, GeneralEnum::MapBehavior map_behavior) {
		Initialize(element_count, TypeInfoManager::GetSingleton()->Get<T>(), (const void *)data, map_behavior );
	}
	virtual unsigned int 	GetElementCount() const = 0;
	virtual unsigned int 	GetElementBytewidth() const = 0;
	virtual unsigned int 	GetElementMemberCount() const = 0;
	//VertexBuffer with raw data, not initialized or initialized through 
	//void Initialize(unsigned int size, const void *data, bool is_dynamic)
	//will return empty string.
	virtual s2string 			GetElementTypeName() const = 0;
	
	virtual GeneralEnum::MapBehavior GetMapBehavior() const = 0;
	template <typename T>
	void Update(unsigned int start_index, T *data, unsigned int array_size) {
		this->Update(start_index, (void *)data, sizeof(T)*array_size);
	}
	virtual void Map(bool is_partial_map) = 0;
	virtual void UnMap() = 0;
	
protected:
	virtual void Update(unsigned int start_index, const void *data, unsigned int size) = 0;
	virtual void Initialize(unsigned int element_count, const TypeInfo &type_info, const void *data, GeneralEnum::MapBehavior map_behavior) = 0;
};

}

#endif 	//VERTEX_BUFFER_H_