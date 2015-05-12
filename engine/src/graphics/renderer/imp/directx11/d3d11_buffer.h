#ifndef D3D11_BUFFER_H_
#define D3D11_BUFFER_H_

#include "graphics/renderer/buffer.h"

struct ID3D11Buffer;

namespace s2 {

class D3D11GraphicResourceManager;
class D3D11MappedResource;
class D3D11IndexBuffer;
class D3D11VertexBuffer;
class D3D11StreamOut;

class D3D11Buffer : public Buffer {
public:
	D3D11Buffer(D3D11GraphicResourceManager *_manager);
	virtual ~D3D11Buffer();
	virtual void Initialize(const Option &option);
	virtual unsigned int GetElementCount() const;
	virtual unsigned int GetElementBytewidth() const;
	virtual unsigned int GetElementMemberCount() const;
	virtual s2string GetElementTypeName() const;

	virtual RendererEnum::ResourceWrite GetResourceWrite() const;
	virtual unsigned int GetBinding() const;
	virtual void WriteMap(GraphicPipeline *pipeline, bool no_overwrite);
	virtual void WriteUnmap();
	virtual void ReadMap(GraphicPipeline *pipeline, bool wipe_cache) const;
	virtual void ReadUnmap() const;

	virtual void Write(unsigned int index, const void *data, unsigned int array_size, unsigned int element_byetwidth);
	virtual const void * Read(unsigned int index, unsigned int element_byetwidth) const;
	virtual void Update(GraphicPipeline *pipeline, unsigned int index, const void *data, unsigned int array_size, unsigned int element_byetwidth);

	virtual IndexBuffer * AsIndexBuffer() const;
	virtual VertexBuffer * AsVertexBuffer() const;
	virtual StreamOut * AsStreamOut() const;

	/**************D3D11 exclusive************/
	ID3D11Buffer * GetInternal() { return buffer; }

private:
	void Check() const;

private:
	D3D11GraphicResourceManager *manager;
	ID3D11Buffer *buffer;

	mutable D3D11MappedResource *mapped;

	D3D11IndexBuffer *index_buffer;
	D3D11VertexBuffer *vertex_buffer;
	D3D11StreamOut *stream_out;

	Option option;
};

}

#endif			//D3D11_BUFFER_H_