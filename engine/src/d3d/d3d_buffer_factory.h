#ifndef D3D_BUFFER_FACTORY_H_
#define D3D_BUFFER_FACTORY_H_

#include <string>
#include <map>

struct ID3D11Buffer;
struct ID3D11Device;

class D3DBufferFactory {
private:
	typedef std::map<std::string, ID3D11Buffer *> Map;

public:
	D3DBufferFactory(ID3D11Device *device);
	~D3DBufferFactory();

	ID3D11Buffer * CreateConstantBuffer(const std::string &name, unsigned int size, const void *data);
	ID3D11Buffer * CreateIndexBuffer(const std::string &name, unsigned int size, const void *data);
	ID3D11Buffer * CreateVertexBuffer(const std::string &name, unsigned int size, const void *data);
	ID3D11Buffer * Get(const std::string &name);
	void Remove(const std::string &name);

private:
	Map data_;
	ID3D11Device *device_;

};

#endif			//D3D_BUFFER_FACTORY_H_