#include "d3d_rasterizer_state_factory.h"
#include "engine.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

static const char * kDefault = "default";

D3DRasterizerStateFactory::D3DRasterizerStateFactory(ID3D11Device *device) : device_(device) {
	EngineOption *option = Engine::GetSingleton()->GetOption();
	D3D11_RASTERIZER_DESC raster_desc;

	// Setup the raster description which will determine how and what polygons will be drawn.
	raster_desc.AntialiasedLineEnable = false;
	raster_desc.CullMode = D3D11_CULL_BACK;
	raster_desc.DepthBias = 0;
	raster_desc.DepthBiasClamp = 0.0f;
	raster_desc.DepthClipEnable = true;
	raster_desc.FillMode = D3D11_FILL_SOLID;
	raster_desc.FrontCounterClockwise = false;
	raster_desc.MultisampleEnable = false;
	raster_desc.ScissorEnable = false;
	raster_desc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	Create(kDefault, *raster_desc);
}

D3DRasterizerStateFactory::~D3DRasterizerStateFactory() {
	for(Map::iterator it = data_.begin(); it != data_.end(); it ++) {
		it->second->Release();
	}
}

ID3D11RasterizerState * D3DRasterizerStateFactory::GetDefault() {
	return Get(kDefault);
}

ID3D11RasterizerState * D3DRasterizerStateFactory::Get(const std::string &name) {
	if(data_.find(name) == data_.end())
		return NULL;
	else
		return data_[name];
}

ID3D11RasterizerState * D3DRasterizerStateFactory::Create(const std::string &name, const D3D11_RASTERIZER_DESC  * desc) {
	if(data_.find(name) != data_.end()) {
		LOG(WARNING)<<"Attempt to create state "<<name<<" multiple times. Creation is ignored.";
		return data_[name];
	}
	ID3D11RasterizerState *state = 0;
	HRESULT result = device_->CreateRasterizerState(desc, *state);
	CHECK(!FAILED(result))<<"Cannot create rasterizer state " << name<< ". Error "<<GetLastError();
	data_[name] = state;
	return state;
}

void D3DRasterizerStateFactory::Remove(const std::string &name) {
	if(data_.find(name) != data_.end()) {
		data_[name]->Release();
		data_.erase(name);
	}
}