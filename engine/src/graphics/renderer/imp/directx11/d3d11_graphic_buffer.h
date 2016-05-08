#ifndef D3D11_GRAPHIC_BUFFER_H_
#define D3D11_GRAPHIC_BUFFER_H_

#include "graphics/renderer/graphic_buffer.h"

struct ID3D11Buffer;

namespace s2 {

class D3D11GraphicResourceManager;
class D3D11MappedResource;
class D3D11IndexBuffer;
class D3D11VertexBuffer;
class D3D11StreamOut;
class D3D11UnorderedAccess;

class D3D11GraphicBuffer : public GraphicBuffer {
public:
	D3D11GraphicBuffer(D3D11GraphicResourceManager *_manager);
	virtual ~D3D11GraphicBuffer();
	virtual bool Initialize(const Option &option);

	virtual const GraphicBuffer::Option & GetOption() const;

	virtual void WriteMap(GraphicPipeline *pipeline, bool no_overwrite);
	virtual void WriteUnmap();
	virtual void ReadMap(GraphicPipeline *pipeline, bool wipe_cache) const;
	virtual void ReadUnmap() const;

	virtual void Write(uint32_t index, const void *data, uint32_t array_size, uint32_t element_byetwidth);
	virtual const void * Read(uint32_t index, uint32_t element_byetwidth) const;
	virtual void Update(GraphicPipeline *pipeline, uint32_t index, const void *data, uint32_t array_size, uint32_t element_byetwidth);

	virtual IndexBuffer * AsIndexBuffer() const;
	virtual VertexBuffer * AsVertexBuffer() const;
	virtual StreamOut * AsStreamOut() const;
	virtual UnorderedAccess * AsUnorderedAccess() const;

	/**************D3D11 exclusive************/
	ID3D11Buffer * GetInternal() { return buffer; }

private:
	void Check() const;
	void Clean();

private:
	D3D11GraphicResourceManager *manager;
	ID3D11Buffer *buffer;

	mutable D3D11MappedResource *mapped;

	D3D11IndexBuffer *index_buffer;
	D3D11VertexBuffer *vertex_buffer;
	D3D11StreamOut *stream_out;
	D3D11UnorderedAccess *unordered_access;

	Option option;
};

}

#endif			//D3D11_BUFFER_H_