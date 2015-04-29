#ifndef D3D11_CONSTANT_BUFFER_H_
#define D3D11_CONSTANT_BUFFER_H_

#include "utils/s2string.h"

struct ID3D11Buffer;

namespace s2 {

class D3D11GraphicResourceManager;

class D3D11ConstantBuffer {
public:
	D3D11ConstantBuffer(D3D11GraphicResourceManager * _manager);
	~D3D11ConstantBuffer();
	void Initialize(unsigned int size, const void *data);
 	void Flush();
	
	bool SetData(unsigned int offset, const void *data, unsigned int size);
	unsigned int GetSize() { return size;}
	
	/*******************D3D11 exclusive*******************/
	ID3D11Buffer * GetInternal() { return cb; }
	
	const s2string & GetLastError() { return error; }

private:
	void Clear();
	
private:
	D3D11GraphicResourceManager * manager;
	
	ID3D11Buffer  *cb;
	unsigned int size;
	char *data_buffer;
	s2string error;
};


}



#endif		//D3D11_CONSTANT_BUFFER_H_