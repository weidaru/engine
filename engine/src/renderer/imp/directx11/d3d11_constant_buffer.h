#ifndef D3D11_CONSTANT_BUFFER_H_
#define D3D11_CONSTANT_BUFFER_H_

#include "renderer/constant_buffer.h"

struct ID3D11Buffer;


namespace s2 {
class D3D11GraphicResourceManager;

class D3D11ConstantBuffer : public ConstantBuffer {
public:
	D3D11ConstantBuffer(D3D11GraphicResourceManager * _manager);
	virtual ~D3D11ConstantBuffer();
	virtual void Initialize(unsigned int size, const void *data);
	virtual unsigned int GetSize();
 	virtual void * Map();
 	virtual void UnMap();
	
	/*******************D3D11 exclusive*******************/
	ID3D11Buffer * GetInternal() { return cb; }

private:
	void Clear();
	
private:
	D3D11GraphicResourceManager *manager;
	ID3D11Buffer  *cb;
};


}



#endif		//D3D11_CONSTANT_BUFFER_H_