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
													unsigned int per_ele_size, const void *data, GeneralEnum::CPUAccess cpu_access)= 0;
	template <typename T>
	void Initialize(unsigned int element_count, const T *data, GeneralEnum::CPUAccess cpu_access) {
		Initialize(element_count, TypeInfoManager::GetSingleton()->Get<T>(), (const void *)data, cpu_access );
	}
	virtual GeneralEnum::CPUAccess GetCPUAccessFlag() const = 0;
	virtual unsigned int 	GetElementCount() const = 0;
	virtual unsigned int 	GetElementBytewidth() const = 0;
	virtual unsigned int 	GetElementMemberCount() const = 0;
	//VertexBuffer with raw data, not initialized or initialized through 
	//void Initialize(unsigned int size, const void *data, bool is_dynamic)
	//will return empty string.
	virtual s2string 			GetElementTypeName() const = 0;
	virtual void * 			Map() = 0;
	virtual void 				UnMap() = 0;
	
protected:
	virtual void Initialize(unsigned int element_count, const TypeInfo &type_info, const void *data, GeneralEnum::CPUAccess cpu_access) = 0;
};

}

#endif 	//VERTEX_BUFFER_H_