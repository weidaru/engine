#ifndef D3D11_VERTEX_BUFFER_H_
#define D3D11_VERTEX_BUFFER_H_

#include "renderer/vertex_buffer.h"

struct ID3D11Buffer;

namespace s2 {

class D3D11GraphicResourceManager;

class D3D11VertexBuffer : public VertexBuffer {
public:
	D3D11VertexBuffer(D3D11GraphicResourceManager *_manager);
	virtual ~D3D11VertexBuffer();
	virtual void Initialize(unsigned int size, const void *data, bool is_dynamic);
	virtual bool IsDynamic();
	virtual unsigned int GetSize();
	virtual void * Map();
	virtual void UnMap();

	/**************D3D11 exclusive************/
	ID3D11Buffer * GetInternal() { return vb; }
	
private:
	void Clear();

private:
	D3D11GraphicResourceManager *manager;
	ID3D11Buffer *vb;
};

}


#endif		//D3D11_VERTEX_BUFFER_H_