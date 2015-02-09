#ifndef STREAM_OUT_H_
#define STREAM_OUT_H_

#include "resource.h"
#include "renderer_enum.h"
#include "utils/s2string.h"
#include "utils/type_info.h"

namespace s2 {

class StreamOut : public Resource {
public:
	enum Binding {
		STREAM_OUT = 0x1,
		SHADER_RESOURCE = 0x2
	};

public:
	virtual ~StreamOut() {}
	virtual void Initialize(unsigned int element_count, unsigned int element_member_count, unsigned int per_ele_size,
									RendererEnum::ResourceWrite resource_write = RendererEnum::CPU_WRITE_OCCASIONAL,
									Binding binding = STREAM_OUT) = 0;
	template <typename T>
	void Initialize(unsigned int element_count, const T *data,
						RendererEnum::ResourceWrite resource_write = RendererEnum::CPU_WRITE_OCCASIONAL,
						Binding binding = STREAM_OUT) {
		Initialize(element_count, TypeInfoManager::GetSingleton()->Get<T>(), (const void *)data, resource_write, binding);
	}
	virtual unsigned int 	GetElementCount() const = 0;
	virtual unsigned int 	GetElementBytewidth() const = 0;
	virtual unsigned int 	GetElementMemberCount() const = 0;

	virtual s2string 			GetElementTypeName() const { return ""; }

	virtual RendererEnum::ResourceWrite GetResourceWrite() const = 0;
	virtual StreamOut::Binding GetBinding() const = 0;

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

	virtual void ReadMap(bool wipe_cache = true) = 0;
	virtual void ReadUnmap() = 0;
	template <typename T>
	const T * Read(unsigned int index) const {
		return this->Read(index, sizeof(T));
	}

protected:
	virtual void Write(unsigned int index, const void *data, unsigned int array_size, unsigned int element_byetwidth) = 0;
	virtual const void * Read(unsigned int index, unsigned int element_byetwidth) const = 0;
	virtual void Initialize(unsigned int element_count, const TypeInfo &type_info,
		RendererEnum::ResourceWrite resource_write = RendererEnum::CPU_WRITE_OCCASIONAL,
		Binding binding = STREAM_OUT) = 0;
	virtual void Update(unsigned int index, const void *data, unsigned int array_size, unsigned int element_byetwidth) = 0;
};

}




#endif			//STREAM_OUT_H_