#ifndef D3D11_INDEX_BUFFER_H_
#define D3D11_INDEX_BUFFER_H_

#include "renderer/index_buffer.h"

#include <stdint.h>

struct ID3D11Buffer;

namespace s2 {

class D3D11GraphicResourceManager;

class D3D11IndexBuffer : public IndexBuffer {
public:
	D3D11IndexBuffer(D3D11GraphicResourceManager *_manager);
	virtual ~D3D11IndexBuffer();
	virtual void Initialize(unsigned int size, const InputType *data, bool is_dynamic);
	virtual bool IsDynamic();
	virtual unsigned int GetSize();
	virtual void * Map();
	virtual void UnMap();
	
	/*********************D3D11 exclusive***********************/
	ID3D11Buffer * GetInternal() { return ib; }
	
private:
	void Clear();

private:
	D3D11GraphicResourceManager *manager;
	ID3D11Buffer *ib;
	s2string error;
};

}


#endif		//D3D11_INDEX_BUFFER_H_