#ifndef D3D11_BUFFER_H_
#define D3D11_BUFFER_H_

#include "renderer/buffer.h"
#include "d3d11_resource_view.h"

struct ID3D11Buffer;

namespace s2 {

class D3D11GraphicResourceManager;
class D3D11MappedResource;

class D3D11Buffer : public Buffer {
public:
	D3D11Buffer(D3D11GraphicResourceManager *_manager);
	virtual ~D3D11Buffer();
	virtual void Initialize(unsigned int element_count, unsigned int element_member_count,
								unsigned int per_ele_size, const void *data,
								RendererEnum::ResourceWrite resource_write,
								Binding binding);
	virtual unsigned int GetElementCount() const;
	virtual unsigned int GetElementBytewidth() const;
	virtual unsigned int GetElementMemberCount() const;
	virtual s2string GetElementTypeName() const { return type_name; }

	virtual RendererEnum::ResourceWrite GetResourceWrite() const;
	virtual Buffer::Binding GetBinding() const;
	virtual void WriteMap(bool is_partial_map);
	virtual void WriteUnmap();
	virtual void ReadMap(bool wipe_cache) const;
	virtual void ReadUnmap() const;

	virtual D3D11IndexBuffer * AsIndexBuffer() const;
	virtual D3D11VertexBuffer * AsVertexBuffer() const;
	virtual D3D11StreamOut * AsStreamOut() const;

	/**************D3D11 exclusive************/
	ID3D11Buffer * GetInternal() { return buffer; }

private:
	void Check() const;

protected:
	virtual void Initialize(unsigned int element_count, const TypeInfo &type_info, const void *data,
									RendererEnum::ResourceWrite resource_write, Binding binding);
	virtual void Write(unsigned int index, const void *data, unsigned int array_size, unsigned int element_byetwidth);
	virtual const void * Read(unsigned int index, unsigned int element_byetwidth) const;
	virtual void Update(unsigned int index, const void *data, unsigned int array_size, unsigned int element_byetwidth);

private:
	D3D11GraphicResourceManager *manager;
	ID3D11Buffer *buffer;
	s2string type_name;

	mutable D3D11MappedResource *mapped;
	Binding binding;

	unsigned int ele_count;
	unsigned int ele_member_count;
	unsigned int ele_bytewidth;

	D3D11IndexBuffer *index_buffer;
	D3D11VertexBuffer *vertex_buffer;
	D3D11StreamOut *stream_out;
};

}

#endif			//D3D11_BUFFER_H_