#ifndef D3D11_CONSTANT_BUFFER_H_
#define D3D11_CONSTANT_BUFFER_H_

#include "utils/s2string.h"
#include <stdint.h>

struct ID3D11Buffer;

namespace s2 {

class D3D11GraphicResourceManager;
class D3D11GraphicPipeline;

class D3D11ConstantBuffer {
public:
	D3D11ConstantBuffer(D3D11GraphicResourceManager * _manager);
	~D3D11ConstantBuffer();
	void Initialize(uint32_t size, const void *data);
 	void Flush(D3D11GraphicPipeline *pipeline);
	
	bool SetData(uint32_t offset, const void *data, uint32_t size);
	uint32_t GetSize() { return size;}
	
	/*******************D3D11 exclusive*******************/
	ID3D11Buffer * GetInternal() { return cb; }
	
	const s2string & GetLastError() { return error; }

private:
	void Clear();
	
private:
	D3D11GraphicResourceManager * manager;
	
	ID3D11Buffer  *cb;
	uint32_t size;
	char *data_buffer;
	s2string error;
};


}



#endif		//D3D11_CONSTANT_BUFFER_H_