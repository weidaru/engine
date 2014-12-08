#ifndef D3D11_SAMPLER_H_
#define D3D11_SAMPLER_H_

#include "renderer/sampler.h"

struct ID3D11SamplerState;

namespace s2 {

class D3D11GraphicResourceManager;

class D3D11Sampler : public Sampler {
public:
	D3D11Sampler(D3D11GraphicResourceManager *_manager);

	virtual ~D3D11Sampler();
	virtual void Initialize(const Option &_option);
	virtual const Option & GetOption() const;

	ID3D11SamplerState * GetInternal() { return sampler; }
	
private:
	D3D11GraphicResourceManager *manager;

	Option option;
	ID3D11SamplerState *sampler;
	
};

}


#endif 