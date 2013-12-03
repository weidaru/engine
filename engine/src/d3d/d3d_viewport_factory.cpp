#include "d3d_viewport_factory.h"
#include "engine.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

static const char * kDefault = "default";

D3DViewportFactory::D3DViewportFactory() {
	EngineOption *option = Engine::GetSingleton()->GetOption();
	Create(kDefault, 0.0, 0.0, option->window_option.screen_width, option->window_option.screen_height);
}

D3DViewportFactory::~D3DViewportFactory() {
	for(Map::iterator it=data_.begin(); it != data_.end(); it++)
		delete it->second;
}

D3D11_VIEWPORT * D3DViewportFactory::GetDefault() {
	return Get(kDefault);
}

D3D11_VIEWPORT * D3DViewportFactory::Get(const std::string &name) {
	if(data_.find(name) == data_.end())
		return NULL;
	else
		return data_[name];
}

D3D11_VIEWPORT * D3DViewportFactory::Create(const std::string &name, float topleft_x, float topleft_y, float width, float height) {
	if(data_.find(name) != data_.end()) {
		LOG(WARNING)<<"Attempt to create viewport "<<name<<" multiple times. Creation is ignored.";
			return data_[name];
	}
	D3D11_VIEWPORT * viewport = new D3D11_VIEWPORT;
	viewport->TopLeftX = topleft_x;
	viewport->TopLeftY = topleft_y;
	viewport->Width = width;
	viewport->Height = height;
	viewport->MinDepth = 0.0f;
	viewport->MaxDepth = 1.0f;
	data_[name] = viewport;

	return viewport;
}

void D3DViewportFactory::Remove(const std::string &name) {
	if(data_.find(name) != data_.end()) {
		delete data_[name];
		data_.erase(name);
	}
}