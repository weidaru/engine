#include "d3d_sampler_state_factory.h"
#include "engine.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

D3DSamplerStateFactory::D3DSamplerStateFactory(ID3D11Device *device) : device_(device) {

}

D3DSamplerStateFactory::~D3DSamplerStateFactory() {
	for(Map::iterator it = data_.begin(); it != data_.end(); it++)
		it->second->Release();
}

ID3D11SamplerState * D3DSamplerStateFactory::Create(const std::string &name, const D3D11_SAMPLER_DESC * desc) {
	if(data_.find(name) != data_.end()) {
		LOG(WARNING)<<"Attempt to create sampler state "<<name<<" multiple times. Creation is ignored.";
		return data_[name];
	}
	ID3D11SamplerState *state = 0;
	HRESULT result =  device_->CreateSamplerState(desc, &state);
	CHECK(!FAILED(result))<<"Cannot create sampler state "<<name<<". Error "<<GetLastError();
	data_[name] = state;
	return state;
}

ID3D11SamplerState * D3DSamplerStateFactory::Get(const std::string &name) {
	if(data_.find(name) == data_.end())
		return NULL;
	else
		return data_[name];
}

void D3DSamplerStateFactory::Remove(const std::string &name) {
	if(data_.find(name) != data_.end()) {
		data_[name]->Release();
		data_.erase(name);
	}
}