#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#define WIN32_LEAN_AND_MEAN
#include <dxgi.h>
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_context.h"

#include "video_card_info.h"

namespace s2 {

D3D11Context::D3D11Context() 
		: swap_chain(0), back_buffer(0), resource_manager(0), pipeline(0){}

D3D11Context::~D3D11Context() {
	delete back_buffer;
	swap_chain->SetFullscreenState(FALSE, NULL);
	swap_chain->Release();
	delete pipeline;
	delete resource_manager;
}

void D3D11Context::Initialize(void *hwnd) {
	ID3D11Device *device = 0;
	ID3D11DeviceContext *context = 0;

	InitDeviceAndContextAndSwapchain(
		setting.window_width, setting.window_height, hwnd, setting.enable_vsync, setting.full_screen,
		&device, &context);

	resource_manager = new D3D11GraphicResourceManager(device);
	pipeline = new D3D11GraphicPipeline(resource_manager, context);

	//Get back buffer and create render target view.
	HRESULT result = 1;
	ID3D11Texture2D* bf_ptr = 0;
	ID3D11RenderTargetView *bf_rt_view = 0;
	result = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&bf_ptr);
	CHECK(!FAILED(result))<<"Cannot get back buffer from swap chain. Error " << ::GetLastError();
	result = device->CreateRenderTargetView(bf_ptr, NULL, &bf_rt_view);
	
	CHECK(!FAILED(result))<<"Cannot create render target view for back buffer. Error " << ::GetLastError();
	back_buffer = new D3D11Texture2D(resource_manager);
	Texture2D::Option bf_desc;
	bf_desc.width = setting.window_width;
	bf_desc.height = setting.window_height;
	bf_desc.mip_level= 1;
	bf_desc.array_size = 1;
	bf_desc.format = RendererEnum::R8G8B8A8_UNORM;
	bf_desc.sample_size = 1;
	bf_desc.resource_write = RendererEnum::IMMUTABLE;
	bf_desc.output_bind = TextureEnum::RENDER_TARGET;
	bf_desc.input_bind = TextureEnum::NOT_INPUT;
	back_buffer->InitAsBackBuffer(bf_ptr, bf_rt_view, bf_desc);
}

bool D3D11Context::SetSetting(const RendererSetting &_setting) {
	setting = _setting;
	return true;
}

const RendererSetting & D3D11Context::GetSetting() const {
	return setting;
}

D3D11Texture2D* D3D11Context::GetBackBuffer() {
	return back_buffer;
}

void D3D11Context::PropagateSetting() {
}

void D3D11Context::SwapBuffer() {
	CHECK(swap_chain)<<"Need to initialize first.";
	swap_chain->Present(0, 0);
}

void D3D11Context::InitDeviceAndContextAndSwapchain(
						unsigned int window_width, unsigned int window_height, 
						void *hwnd, bool enable_vsync, bool full_screen,
						ID3D11Device **device, ID3D11DeviceContext **context) {
	VideoCardInfo::AdapterInfo adapter_info;
	VideoCardInfo::GetProperAdapter(window_width, window_height, &adapter_info);
	
	HRESULT result = 1;
	DXGI_SWAP_CHAIN_DESC swap_chain_desc;

	ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.BufferDesc.Width = window_width;
	swap_chain_desc.BufferDesc.Height = window_height;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if(enable_vsync) {
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = adapter_info.refresh_numerator;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = adapter_info.refresh_denominator;
	} else {
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = (HWND)hwnd;

	// Turn multisampling off.
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if(full_screen)
		swap_chain_desc.Windowed = false;
	else
		swap_chain_desc.Windowed = true;

	// Set the scan line ordering and scaling to unspecified.
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D_FEATURE_LEVEL  feature_level = D3D_FEATURE_LEVEL_11_0;
	UINT creation_flags = 0;
#ifndef NDEBUG
	creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creation_flags, &feature_level, 1, 
		D3D11_SDK_VERSION, &swap_chain_desc, &swap_chain, device, NULL, context);
	CHECK(!FAILED(result))<<"Cannot create device and swap chain. Error " << ::GetLastError();
}

}


