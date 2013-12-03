#ifndef D3D_SAMPLER_STATE_FACTORY_H_
#define D3D_SAMPLER_STATE_FACTORY_H_ 

#include <string>
#include <map>

struct ID3D11SamplerState;
struct D3D11_SAMPLER_DESC;
struct ID3D11Device;

class D3DSamplerStateFactory {
private:
	typedef std::map<std::string, ID3D11SamplerState *> Map;

public:
	D3DSamplerStateFactory(ID3D11Device *device);
	~D3DSamplerStateFactory();

	ID3D11SamplerState * Create(const std::string &name, const D3D11_SAMPLER_DESC * desc);
	ID3D11SamplerState * Get(const std::string &name);
	void Remove(const std::string &name);

private:
	Map data_;
	ID3D11Device *device_;
};

#endif			//D3D_SAMPLER_STATE_FACTORY_H_