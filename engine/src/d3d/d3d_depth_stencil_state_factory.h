#ifndef	D3D_DEPTH_STENCIL_STATE_FACTORY_H_
#define D3D_DEPTH_STENCIL_STATE_FACTORY_H_

#include <map>
#include <string>

#include <singleton.h>

struct ID3D11DepthStencilState;
struct D3D11_DEPTH_STENCIL_DESC;
struct ID3D11Device;

class D3DDepthStencilStateFactory {
private:
	typedef std::map<std::string, ID3D11DepthStencilState *> Map;

public:
	D3DDepthStencilStateFactory(ID3D11Device *device);
	~D3DDepthStencilStateFactory();
	
	ID3D11DepthStencilState * GetDefault();
	ID3D11DepthStencilState * Get(const std::string &name);
	void Remove(const std::string &name);
	ID3D11DepthStencilState * Create(const std::string &name, const D3D11_DEPTH_STENCIL_DESC * desc);
	
private:
	Map data_;
	ID3D11Device *device_;
};



#endif		//D3D_DEPTH_STENCIL_STATE_FACTORY_H_