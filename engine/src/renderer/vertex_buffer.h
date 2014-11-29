#ifndef VERTEX_BUFFER_H_
#define VERTEX_BUFFER_H_

#include "resource.h"
#include "utils/s2string.h"
#include "utils/type_info.h"
#include "general_enum.h"

namespace s2 {

class VertexBuffer : public Resource {
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
	virtual s2string 			GetElementTypeName() const { return ""; }
	
	virtual GeneralEnum::MapBehavior GetMapBehavior() const = 0;
	virtual void Map(bool is_partial_map) = 0;
	virtual void UnMap() = 0;
	
	template <typename T>
	void Write(unsigned int index, T *data, unsigned int array_size) {
		this->Write(index, (void *)data, array_size, sizeof(T));
	}
	
	template <typename T>
	const T * Read(unsigned int index) const {
#		ifdef NDEBUG
			return dynamic_cast<T *>(this->Read(index, sizeof(T)));
#		else
			return static_cast<T *>(this->Read(index, sizeof(T)));
#		endif
	}
	
	template <typename T>
	void Update(unsigned int index, T *data, unsigned int array_size) {
		this->Update(index, (const void *)data, array_size, sizeof(T));
	}
	
	
protected:
	virtual void Write(unsigned int index, const void *data, unsigned int array_size, unsigned int element_byetwidth) = 0;
	virtual const void * Read(unsigned int index, unsigned int element_byetwidth) const = 0;
	virtual void Initialize(unsigned int element_count, const TypeInfo &type_info, const void *data, GeneralEnum::MapBehavior map_behavior) = 0;
	virtual void Update(unsigned int index, const void *data, unsigned int array_size, unsigned int element_byetwidth) = 0;
};

}

#endif 	//VERTEX_BUFFER_H_