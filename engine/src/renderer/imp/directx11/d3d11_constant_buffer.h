#ifndef D3D11_CONSTANT_BUFFER_H_
#define D3D11_CONSTANT_BUFFER_H_

struct ID3D11Buffer;

namespace s2 {

class D3D11GraphicResourceManager;

class D3D11ConstantBuffer {
public:
	D3D11ConstantBuffer(D3D11GraphicResourceManager * _manager);
	~D3D11ConstantBuffer();
	void Initialize(unsigned int size, const void *data);
 	void FLush();
	
	void *GetDataBuffer() { return (void *)data_buffer; }
	unsigned int GetSize() { return size;}
	
	/*******************D3D11 exclusive*******************/
	ID3D11Buffer * GetInternal() { return cb; }

private:
	void Clear();
	
private:
	D3D11GraphicResourceManager * manager;
	
	ID3D11Buffer  *cb;
	unsigned int size;
	char *data_buffer;
};


}



#endif		//D3D11_CONSTANT_BUFFER_H_