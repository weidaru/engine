#ifndef D3D_RASTERIZER_STATE_FACTORY_H_
#define D3D_RASTERIZER_STATE_FACTORY_H_

#include <string>
#include <map>

struct ID3D11RasterizerState;
struct D3D11_RASTERIZER_DESC ;
struct ID3D11Device;
class D3DRasterizerStateFactory {
private:
	typedef std::map<std::string, ID3D11RasterizerState *> Map;

public:
	D3DRasterizerStateFactory(ID3D11Device *device);
	~D3DRasterizerStateFactory();

	ID3D11RasterizerState * GetDefault();
	ID3D11RasterizerState * Get(const std::string &name);
	void Remove(const std::string &name);
	ID3D11RasterizerState * Create(const std::string &name, const D3D11_RASTERIZER_DESC  * desc);

private:
	Map data_;
	ID3D11Device *device_;
};

#endif		//D3D_RASTERIZER_STATE_FACTORY_H_