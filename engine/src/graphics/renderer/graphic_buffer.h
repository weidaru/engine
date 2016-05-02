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

	struct Option {
		uint32_t element_count;
		uint32_t element_member_count;
		uint32_t element_bytewidth;
		void * data;
		s2string element_typename;
		RendererResourceWrite resource_write;
		RendererInputBind input_bind;
		RendererOutputBind output_bind;

		Option() {
			element_count = 0;
			element_member_count = 0;
			element_bytewidth = 0;
			data = 0;
			resource_write = RendererResourceWrite::CPU_WRITE_OCCASIONAL;
			input_bind = RendererInputBind::VERTEX_BUFFER;
			output_bind = RendererOutputBind::NOT_OUTPUT;
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
			input_bind = RendererInputBind::INDEX_BUFFER;
			output_bind = RendererOutputBind::NOT_OUTPUT;
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

	virtual RendererResourceWrite GetResourceWrite() const = 0;
	virtual RendererInputBind GetInputBind() const = 0;
	virtual RendererOutputBind GetOutputBind() const = 0;

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
	virtual UnorderedAccess * AsUnorderedAccess() const = 0;
};

}

#endif		//GRAPHIC_BUFFER_H_