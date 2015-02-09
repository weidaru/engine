#ifndef D3D11_STREAM_OUT_H_
#define D3D11_STREAM_OUT_H_

#include "renderer/stream_out.h"

namespace s2 {

class D3D11StreamOut : public StreamOut {
public:
	virtual ~D3D11StreamOut() {}
	virtual void Initialize(unsigned int element_count, unsigned int element_member_count, unsigned int per_ele_size,
		RendererEnum::ResourceWrite resource_write = RendererEnum::CPU_WRITE_OCCASIONAL,
		Binding binding = STREAM_OUT) = 0;

	virtual unsigned int 	GetElementCount() const = 0;
	virtual unsigned int 	GetElementBytewidth() const = 0;
	virtual unsigned int 	GetElementMemberCount() const = 0;

	virtual s2string 			GetElementTypeName() const { return ""; }

	virtual RendererEnum::ResourceWrite GetResourceWrite() const = 0;
	virtual StreamOut::Binding GetBinding() const = 0;

	virtual void WriteMap(bool is_partial_map) = 0;
	virtual void WriteUnmap() = 0;

	virtual void ReadMap(bool wipe_cache = true) = 0;
	virtual void ReadUnmap() = 0;

protected:
	virtual void Write(unsigned int index, const void *data, unsigned int array_size, unsigned int element_byetwidth) = 0;
	virtual const void * Read(unsigned int index, unsigned int element_byetwidth) const = 0;
	virtual void Initialize(unsigned int element_count, const TypeInfo &type_info,
								RendererEnum::ResourceWrite resource_write = RendererEnum::CPU_WRITE_OCCASIONAL,
								Binding binding = STREAM_OUT) = 0;
	virtual void Update(unsigned int index, const void *data, unsigned int array_size, unsigned int element_byetwidth) = 0;
};

}


#endif