#ifndef VERTEX_BUFFER_H_
#define VERTEX_BUFFER_H_

#include "resource.h"
#include "utils/s2string.h"
#include "utils/type_info.h"
#include "renderer_enum.h"

namespace s2 {

class VertexBuffer : public Resource {
public:
	enum Binding {
		VERTEX_BUFFER = 0x1,
		SHADER_RESOURCE = 0x2,
		STREAM_OUT = 0x4
	};

public:
	virtual 						~VertexBuffer() {}
	virtual void 				Initialize(	unsigned int element_count,unsigned int element_member_count,
													unsigned int per_ele_size, const void *data, 
													RendererEnum::ResourceWrite resource_write=RendererEnum::CPU_WRITE_OCCASIONAL, 
													Binding binding = VERTEX_BUFFER)= 0;
	template <typename T>
	void Initialize(	unsigned int element_count, const T *data, 
							RendererEnum::ResourceWrite resource_write=RendererEnum::CPU_WRITE_OCCASIONAL, 
							Binding binding = VERTEX_BUFFER) {
		Initialize(element_count, TypeInfoManager::GetSingleton()->Get<T>(), (const void *)data, resource_write, binding );
	}
	virtual unsigned int 	GetElementCount() const = 0;
	virtual unsigned int 	GetElementBytewidth() const = 0;
	virtual unsigned int 	GetElementMemberCount() const = 0;
	//VertexBuffer with raw data, not initialized or initialized through 
	//void Initialize(unsigned int size, const void *data, bool is_dynamic)
	//will return empty string.
	virtual s2string 			GetElementTypeName() const { return ""; }
	
	virtual RendererEnum::ResourceWrite GetResourceWrite() const = 0;
	virtual VertexBuffer::Binding GetBinding() const = 0;
	
	virtual void WriteMap(bool is_partial_map) = 0;
	virtual void WriteUnmap() = 0;
	template <typename T>
	void Write(unsigned int index, T *data, unsigned int array_size) {
		this->Write(index, (void *)data, array_size, sizeof(T));
	}
	template <typename T>
	void Update(unsigned int index, T *data, unsigned int array_size) {
		this->Update(index, (const void *)data, array_size, sizeof(T));
	}
	
	virtual void ReadMap() = 0;
	virtual void ReadUnmap() = 0;
	template <typename T>
	T * Read(unsigned int index) {
		return this->Read(index, sizeof(T));
	}
	
protected:
	virtual void Write(unsigned int index, const void *data, unsigned int array_size, unsigned int element_byetwidth) = 0;
	virtual const void * Read(unsigned int index, unsigned int element_byetwidth) const = 0;
	virtual void Initialize(unsigned int element_count, const TypeInfo &type_info, const void *data, 
									RendererEnum::ResourceWrite resource_write=RendererEnum::CPU_WRITE_OCCASIONAL, 
									Binding binding = VERTEX_BUFFER) = 0;
	virtual void Update(unsigned int index, const void *data, unsigned int array_size, unsigned int element_byetwidth) = 0;
};

}

#endif 	//VERTEX_BUFFER_H_