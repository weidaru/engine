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
	virtual void Initialize(unsigned int element_count, unsigned int per_ele_size, const void *data, bool is_dynamic);
	virtual bool IsDynamic();
	virtual unsigned int GetElementCount();
	virtual unsigned int GetElementBytewidth();
	virtual void * Map();
	virtual void UnMap();
	virtual s2string GetElementTypeName() { return type_name; }

	/**************D3D11 exclusive************/
	ID3D11Buffer * GetInternal() { return vb; }
	
private:
	void Clear();

protected:
	virtual void Initialize(unsigned int element_count, const TypeInfo &type_info, const void *data, bool is_dynamic);

private:
	D3D11GraphicResourceManager *manager;
	ID3D11Buffer *vb;
	unsigned int ele_count;
	unsigned int ele_bytewidth;
	s2string type_name;
	
};

}


#endif		//D3D11_VERTEX_BUFFER_H_