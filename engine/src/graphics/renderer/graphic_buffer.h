#ifndef GRAPHIC_BUFFER_H_
#define GRAPHIC_BUFFER_H_

#include "resource.h"
#include "resource_view.h"
#include "renderer_enum.h"
#include "utils/s2string.h"
#include "utils/type_info.h"

namespace s2 {

class GraphicPipeline;

class GraphicBuffer : public Resource {
public:
	typedef uint32_t IndexBufferElementType;

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

	struct Option {
		uint32_t element_count;
		uint32_t element_member_count;
		uint32_t element_bytewidth;
		void * data;
		s2string element_typename;
		RendererEnum::Format format;
		RendererEnum::ResourceWrite resource_write;
		uint32_t binding;

		Option() {
			element_count = 0;
			element_member_count = 0;
			element_bytewidth = 0;
			data = 0;
			format = RendererEnum::R8G8B8A8_UNORM;
			resource_write = RendererEnum::CPU_WRITE_OCCASIONAL;
			binding = VERTEX_BUFFER;
		}

		template<typename T>
		void Initialize(uint32_t size, T *_data=0) {
			const TypeInfo &type_info =  TypeInfoManager::GetSingleton()->Get<T>();
			element_count = size;
			element_member_count = type_info.GetMemberSize();
			element_bytewidth = type_info.GetSize();
			element_typename = type_info.GetName();
			data = _data;
		}
		
		void InitializeAsIndexBuffer(uint32_t size, IndexBufferElementType *_data=0) {
			element_count = size;
			element_member_count = 1;
			element_bytewidth = sizeof(IndexBufferElementType);
			element_typename = "uint32_t";
			binding = INDEX_BUFFER;
			data = _data;
		}

	private:
		//Avoid initialize with void *;
		template<>
		void Initialize(uint32_t size, void *_data);
	};

public:
	virtual 						~GraphicBuffer() {}
	/**
	 * TODO: Really don't like such long list of parameter, refactor!!!!
	 */
	virtual void 				Initialize(const Option &option) = 0;

	virtual uint32_t 	GetElementCount() const = 0;
	virtual uint32_t 	GetElementBytewidth() const = 0;
	virtual uint32_t 	GetElementMemberCount() const = 0;
	//VertexBuffer with raw data, not initialized or initialized through 
	//will return have "" TypeName.
	virtual s2string 			GetElementTypeName() const { return ""; }

	virtual RendererEnum::ResourceWrite GetResourceWrite() const = 0;
	virtual uint32_t GetBinding() const = 0;

	virtual void WriteMap(GraphicPipeline *pipeline, bool no_overwrite = false) = 0;
	virtual void WriteUnmap() = 0;
	template <typename T>
	void Write(uint32_t index, T *data, uint32_t array_size) {
		this->Write(index, (void *)data, array_size, sizeof(T));
	}
	template <typename T>
	void Update(uint32_t index, T *data, uint32_t array_size) {
		this->Update(index, (const void *)data, array_size, sizeof(T));
	}

	virtual void ReadMap(GraphicPipeline *pipeline, bool wipe_cache = true) const = 0;
	virtual void ReadUnmap() const = 0;
	template <typename T>
	const T * Read(uint32_t index) const {
		return (const T *)this->Read(index, sizeof(T));
	}

	virtual void Write(uint32_t index, const void *data, uint32_t array_size, uint32_t element_byetwidth) = 0;
	virtual const void * Read(uint32_t index, uint32_t element_byetwidth) const = 0;
	virtual void Update(GraphicPipeline *pipeline, uint32_t index, const void *data, uint32_t array_size, uint32_t element_byetwidth) = 0;

	virtual IndexBuffer * AsIndexBuffer() const = 0;
	virtual VertexBuffer * AsVertexBuffer() const = 0;
	virtual StreamOut * AsStreamOut() const = 0;
};

}

#endif		//GRAPHIC_BUFFER_H_