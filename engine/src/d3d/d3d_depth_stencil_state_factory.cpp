#include "d3d_depth_stencil_state_factory.h"
#include "engine.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

static const char *kDefault = "default";

D3DDepthStencilStateFactory::D3DDepthStencilStateFactory(ID3D11Device *device) : device_(device) {
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	// Initialize the description of the stencil state.
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	// Set up the description of the stencil state.
	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	depth_stencil_desc.StencilEnable = true;
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;
	// Stencil operations if pixel is front-facing.
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Stencil operations if pixel is back-facing.
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	Create(kDefault, &depth_stencil_desc);
}

D3DDepthStencilStateFactory::~D3DDepthStencilStateFactory() {
	for(Map::iterator it=data_.begin(); it != data_.end(); it++) {
		it->second->Release();
	}
}

ID3D11DepthStencilState * D3DDepthStencilStateFactory::GetDefault() {
	return Get(kDefault);
}

ID3D11DepthStencilState * D3DDepthStencilStateFactory::Get(const std::string &name) {
	if(data_.find(name) == data_.end())
		return NULL;
	else
		return data_[name];
}

ID3D11DepthStencilState * D3DDepthStencilStateFactory::Create(const std::string &name, const D3D11_DEPTH_STENCIL_DESC * desc) {
	if(data_.find(name) != data_.end()) {
		LOG(WARNING)<<"Attempt to create state "<<name<<" multiple times. Creation is ignored.";
			return data_[name];
	}
	ID3D11DepthStencilState *state = 0;
	HRESULT result = device_->CreateDepthStencilState(desc, &state);
	CHECK(!FAILED(result))<<"Cannot create depth stencil state " << name << ". Error: "<<GetLastError();
	data_[name] = state;
	return state;
}

void D3DDepthStencilStateFactory::Remove(const std::string &name) {
	if(data_.find(name) != data_.end()) {
		data_[name]->Release();
		data_.erase(name);
	}
}