#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#define WIN32_LEAN_AND_MEAN
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>
#include <stdio.h>

#include "d3d_resource_manager.h"
#include "engine.h"
#include "d3d_buffer_factory.h"
#include "d3d_sampler_state_factory.h"
#include "d3d_depth_stencil_state_factory.h"
#include "d3d_rasterizer_state_factory.h"
#include "d3d_texture_factory.h"
#include "d3d_viewport_factory.h"

D3DSwapChain::D3DSwapChain() {
	swap_chain = 0;
	bf_view = 0;
}

void D3DResourceManager::Init() {
	InitVideoCardInfo();
	InitDeviceAndSwapChain();
	InitDepthStencilBuffer();

	buffer_factory_ = new D3DBufferFactory(device_);
	ds_state_factory_ = new D3DDepthStencilStateFactory(device_);
	rasterizer_state_factory_ = new D3DRasterizerStateFactory(device_);
	texture_factory_ = new D3DTextureFactory(device_);
	viewport_factory_ = new D3DViewportFactory;
}

void D3DResourceManager::Release() {
	delete viewport_factory_;
	delete texture_factory_;
	delete rasterizer_state_factory_;
	delete ds_state_factory_;
	delete buffer_factory_;

	depth_stencil_.view->Release();
	depth_stencil_.view=0;
	depth_stencil_.buffer->Release();
	depth_stencil_.buffer=0;

	swap_chain_.bf_view->Release();
	swap_chain_.bf_view = 0;
	swap_chain_.swap_chain->Release();
	swap_chain_.swap_chain = 0;

	context_->Release();
	context_ = 0;
	device_->Release();
	device_ = 0;
}

void D3DResourceManager::InitVideoCardInfo() {
	HRESULT result = 1;
	EngineOption *option = Engine::GetSingleton()->GetOption();

	IDXGIFactory* factory = 0;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	CHECK(!FAILED(result))<<"Cannot create DXGI factory. Error " << GetLastError();

	IDXGIAdapter* adapter = 0;
	result = factory->EnumAdapters(0, &adapter);
	CHECK(!FAILED(result))<<"Cannot create an adapter to video card. Error " << GetLastError();

	IDXGIOutput* adapter_output = 0;
	result = adapter->EnumOutputs(0, &adapter_output);
	CHECK(!FAILED(result))<<"Cannot enumerate the primary adapter output (monitor) Error " << GetLastError();

	DXGI_MODE_DESC* display_mode_list = 0;
	unsigned int mode_count = 0;
	result = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &mode_count, NULL);
	CHECK(!FAILED(result))<<"Cannot get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor). Error " << GetLastError();
	display_mode_list =new DXGI_MODE_DESC[mode_count];
	result = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &mode_count, display_mode_list);
	CHECK(!FAILED(result))<<"Cannot get the modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor). Error " << GetLastError();

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	result = 0;
	for(int i=0; i<mode_count; i++) {
		if(display_mode_list[i].Width == (unsigned int)option->window_option.screen_width &&
		   display_mode_list[i].Height == (unsigned int)option->window_option.screen_height) {
				adaper_info_.refresh_numerator = display_mode_list[i].RefreshRate.Numerator;
				adaper_info_.refresh_denominator= display_mode_list[i].RefreshRate.Denominator;
				result = 1;
				break;
		}
	}
	CHECK(!FAILED(result))<<"Cannot find the mode with width "<<option->window_option.screen_width
									   <<"and height "<<option->window_option.screen_height;

	DXGI_ADAPTER_DESC adapterDesc;
	result = adapter->GetDesc(&adapterDesc);
	CHECK(!FAILED(result))<<"Cannot get adapter description";

	adaper_info_.memory_size =  (unsigned int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024); 
	{
		size_t dummy = 0;
		CHECK(!wcstombs_s(&dummy, adaper_info_.name, sizeof(adaper_info_.name), adapterDesc.Description, sizeof(adaper_info_.name)));
	}
	
	delete[] display_mode_list;
	adapter_output->Release();
	adapter->Release();
	factory->Release();

	LOG(INFO)<<"\n"<<"Using video card " << adaper_info_.name<<"\n"
					 <<"memory = " << adaper_info_.memory_size<<"\n"
					 <<"numerator = "<<adaper_info_.refresh_numerator<<"\n"
					 <<"denominator = "<<adaper_info_.refresh_denominator;
}

void D3DResourceManager::InitDeviceAndSwapChain() {
	HRESULT result = 1;
	EngineOption *option = Engine::GetSingleton()->GetOption();
	DXGI_SWAP_CHAIN_DESC swap_chain_desc;

	ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.BufferDesc.Width = option->window_option.screen_width;
	swap_chain_desc.BufferDesc.Height = option->window_option.screen_height;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if(option->d3d_option.enable_v_sync)
	{
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = adaper_info_.refresh_numerator;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = adaper_info_.refresh_denominator;
	}
	else
	{
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = Engine::GetSingleton()->GetWindowHandle();

	// Turn multisampling off.
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if(option->window_option.full_screen)
		swap_chain_desc.Windowed = false;
	else
		swap_chain_desc.Windowed = true;

	// Set the scan line ordering and scaling to unspecified.
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swap_chain_desc.Flags = 0;

	D3D_FEATURE_LEVEL  feature_level = D3D_FEATURE_LEVEL_11_0;
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &feature_level, 1, 
		D3D11_SDK_VERSION, &swap_chain_desc, &swap_chain_.swap_chain, &device_, NULL, &context_);
	CHECK(!FAILED(result))<<"Cannot create device and swap chain. Error " << GetLastError();

	ID3D11Texture2D* bf_ptr = 0;
	result = swap_chain_.swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&bf_ptr);
	CHECK(!FAILED(result))<<"Cannot get back buffer from swap chain. Error " << GetLastError();
	result = device_->CreateRenderTargetView(bf_ptr, NULL, &swap_chain_.bf_view);
	CHECK(!FAILED(result))<<"Cannot create render target view for back buffer. Error " << GetLastError();
	bf_ptr->Release();
	bf_ptr = 0;
}

void D3DResourceManager::InitDepthStencilBuffer() {
	HRESULT result = 1;
	EngineOption *option = Engine::GetSingleton()->GetOption();
	D3D11_TEXTURE2D_DESC depth_buffer_desc;

	// Initialize the description of the depth buffer.
	ZeroMemory(&depth_buffer_desc, sizeof(depth_buffer_desc));
	// Set up the description of the depth buffer.
	depth_buffer_desc.Width = option->window_option.screen_width;
	depth_buffer_desc.Height = option->window_option.screen_height;
	depth_buffer_desc.MipLevels = 1;
	depth_buffer_desc.ArraySize = 1;
	depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_buffer_desc.SampleDesc.Count = 1;
	depth_buffer_desc.SampleDesc.Quality = 0;
	depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_buffer_desc.CPUAccessFlags = 0;
	depth_buffer_desc.MiscFlags = 0;
	result = device_->CreateTexture2D(&depth_buffer_desc, NULL, &depth_stencil_.buffer);
	CHECK(!FAILED(result))<<"Cannot create depth stencil buffer. Error: "<<GetLastError();

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
	// Initailze the depth stencil view.
	ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));
	// Set up the depth stencil view description.
	depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;
	// Create the depth stencil view.
	result = device_->CreateDepthStencilView(depth_stencil_.buffer, &depth_stencil_view_desc, &depth_stencil_.view);
	CHECK(!FAILED(result))<<"Cannot create depth stencil view. Error " << GetLastError();

}