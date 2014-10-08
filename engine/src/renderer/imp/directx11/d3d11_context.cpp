#pragma comment(lib, "dxgi.lib")

#define WIN32_LEAN_AND_MEAN
#include <dxgi.h>
#undef ERROR

#include "d3d11_context.h"

#include <glog/logging.h>

namespace s2 {

D3D11Context::D3D11Context() 
		: resource_manager(0), pipeline(0){}

D3D11Context::~D3D11Context() {
	delete pipeline;
	delete resource_manager;
}

void D3D11Context::Initialize(void *hwnd) {
	resource_manager = new D3D11GraphicResourceManager(
										setting.window_width, setting.window_height, 
										hwnd, setting.enable_vsync, setting.full_screen);
	pipeline = new D3D11GraphicPipeline(resource_manager);
}

bool D3D11Context::SetSetting(const RendererSetting &_setting) {
	setting = _setting;
	return true;
}

void D3D11Context::GetSetting(RendererSetting *_setting) {
	*_setting = setting;
}

void D3D11Context::GetLastError(s2string *str) {
	*str = error;
}

void D3D11Context::PropagateSetting() {
}

void D3D11Context::SwapBuffer() {
	CHECK(resource_manager)<<"Need to initialize first.";
	resource_manager->GetSwapChain()->Present(0, 0);
}

}


