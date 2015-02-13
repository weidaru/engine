#ifndef BUFFER_H_
#define BUFFER_H_

#include "resource.h"
#include "resource_view.h"
#include "renderer_enum.h"
#include "utils/s2string.h"
#include "utils/type_info.h"

namespace s2 {

class Buffer : public Resource {
public:
	enum Binding {
		INDEX_BUFFER = 0x1,
		VERTEX_BUFFER = 0x2,
		STREAM_OUT = 0x10,
		//Normally we won't need to bind a buffer as shader resource, render target or depth stencil
		//And I am not entirely sure how hard and restricted it is.
		//It may invovles some hardware memory access ability which should not be a concern for now.
		//SHADER_RESOURCE = 0x100,
		//RENDER_TARGET = 0x200,
		//DEPTH_STENCIL = 0x300
	};

public:
	virtual 						~Buffer() {}
	/**
	 * TODO: Really don't like such long list of parameter, refactor!!!!
	 */
	virtual void 				Initialize(unsigned int element_count, unsigned int element_member_count,
												unsigned int per_ele_size, const void *data,
												RendererEnum::ResourceWrite resource_write = RendererEnum::CPU_WRITE_OCCASIONAL,
												Binding binding = VERTEX_BUFFER) = 0;

	template <typename T>
	void Initialize(unsigned int element_count, const T *data,
						RendererEnum::ResourceWrite resource_write = RendererEnum::CPU_WRITE_OCCASIONAL,
						Binding binding = VERTEX_BUFFER) {
						Initialize(element_count, TypeInfoManager::GetSingleton()->Get<T>(), (const void *)data, resource_write, binding);
					}
	virtual unsigned int 	GetElementCount() const = 0;
	virtual unsigned int 	GetElementBytewidth() const = 0;
	virtual unsigned int 	GetElementMemberCount() const = 0;
	//VertexBuffer with raw data, not initialized or initialized through 
	//will return have "" TypeName.
	virtual s2string 			GetElementTypeName() const { return ""; }

	virtual RendererEnum::ResourceWrite GetResourceWrite() const = 0;
	virtual Buffer::Binding GetBinding() const = 0;

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

	virtual void ReadMap(bool wipe_cache = true) const = 0;
	virtual void ReadUnmap() const = 0;
	template <typename T>
	const T * Read(unsigned int index) const {
		return this->Read(index, sizeof(T));
	}

	virtual IndexBuffer * AsIndexBuffer() const = 0;
	virtual VertexBuffer * AsVertexBuffer() const = 0;
	virtual StreamOut * AsStreamOut() const = 0;

protected:
	virtual void Write(unsigned int index, const void *data, unsigned int array_size, unsigned int element_byetwidth) = 0;
	virtual const void * Read(unsigned int index, unsigned int element_byetwidth) const = 0;
	virtual void Initialize(unsigned int element_count, const TypeInfo &type_info, const void *data,
		RendererEnum::ResourceWrite resource_write = RendererEnum::CPU_WRITE_OCCASIONAL,
		Binding binding = VERTEX_BUFFER) = 0;
	virtual void Update(unsigned int index, const void *data, unsigned int array_size, unsigned int element_byetwidth) = 0;
};

}

#endif		//BUFFER_H_