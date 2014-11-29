#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#include "d3d11_graphic_resource_manager.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dxgi.h>
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "video_card_info.h"

#include "renderer/texture1d.h"
#include "renderer/texture2d.h"
#include "renderer/texture3d.h"
#include "renderer/sampler.h"


namespace s2 {

D3D11GraphicResourceManager::D3D11GraphicResourceManager(
						unsigned int window_width, unsigned int window_height, 
						void *hwnd, bool enable_vsync, bool full_screen) {
	InitDeviceAndContextAndSwapchain(window_width, window_height, hwnd, enable_vsync, full_screen);
}

#define MapClean(Type, name) \
for(std::map<unsigned int, Type *>::iterator it=name.begin(); it!=name.end(); it++) { \
	delete it->second; \
}\
name.clear()

D3D11GraphicResourceManager::~D3D11GraphicResourceManager() {
	MapClean(D3D11VertexBuffer, vb_map);
	MapClean(D3D11IndexBuffer, ib_map);
	MapClean(Texture1D, tex1d_map);
	MapClean(D3D11Texture2D, tex2d_map);
	MapClean(Texture3D, tex3d_map);
	MapClean(Sampler, sampler_map);
	MapClean(D3D11VertexShader, vs_map);
	MapClean(D3D11PixelShader, ps_map);
}

void D3D11GraphicResourceManager::InitDeviceAndContextAndSwapchain(
						unsigned int window_width, unsigned int window_height, 
						void *hwnd, bool enable_vsync, bool full_screen) {
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
	swap_chain_desc.Flags = 0;

	D3D_FEATURE_LEVEL  feature_level = D3D_FEATURE_LEVEL_11_0;
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &feature_level, 1, 
		D3D11_SDK_VERSION, &swap_chain_desc, &swap_chain, &device, NULL, &context);
	CHECK(!FAILED(result))<<"Cannot create device and swap chain. Error " << ::GetLastError();

	ID3D11Texture2D* bf_ptr = 0;
	ID3D11RenderTargetView *bf_rt_view = 0;
	result = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&bf_ptr);
	CHECK(!FAILED(result))<<"Cannot get back buffer from swap chain. Error " << ::GetLastError();
	result = device->CreateRenderTargetView(bf_ptr, NULL, &bf_rt_view);
	CHECK(!FAILED(result))<<"Cannot create render target view for back buffer. Error " << ::GetLastError();
	back_buffer = new D3D11Texture2D(this);
	Texture2D::Option bf_desc;
	bf_desc.width = window_width;
	bf_desc.height = window_height;
	bf_desc.mip_level= 1;
	bf_desc.array_size = 1;
	bf_desc.format = TextureEnum::R8G8B8A8_UNORM;
	bf_desc.sample_size = 1;
	bf_desc.map_behavior = GeneralEnum::MAP_FORBIDDEN;
	bf_desc.output_bind = TextureEnum::RENDER_TARGET;
	bf_desc.input_bind = TextureEnum::NOT_INPUT;
	back_buffer->InitAsBackBuffer(bf_ptr, bf_rt_view, bf_desc);
}

D3D11VertexBuffer * D3D11GraphicResourceManager::CreateVertexBuffer() {
	D3D11VertexBuffer *buffer = new D3D11VertexBuffer(this);
	vb_map[buffer->GetID()] = buffer;
	return buffer;
}

D3D11VertexBuffer * D3D11GraphicResourceManager::GetVertexBuffer(unsigned int id) {
	return vb_map[id];
}

void D3D11GraphicResourceManager::RemoveVertexBuffer(unsigned int id) {
	if(vb_map.find(id) != vb_map.end()) {
		delete vb_map[id];
		vb_map.erase(id);
	}
}
	
D3D11IndexBuffer * D3D11GraphicResourceManager::CreateIndexBuffer() {
	D3D11IndexBuffer *buffer = new D3D11IndexBuffer(this);
	ib_map[buffer->GetID()] = buffer;
	return buffer;
}

D3D11IndexBuffer * D3D11GraphicResourceManager::GetIndexBuffer(unsigned int id) {
	return ib_map[id];
}

void D3D11GraphicResourceManager::RemoveIndexBuffer(unsigned int id) {
	if(ib_map.find(id) != ib_map.end()) {
		delete ib_map[id];
		ib_map.erase(id);
	}
}
	
Texture1D * D3D11GraphicResourceManager::CreateTexture1D() {
	CHECK(false)<<"Disabled.";
	return 0;
}

Texture1D * D3D11GraphicResourceManager::GetTexture1D(unsigned int id) {
	CHECK(false)<<"Disabled.";
	return 0;
}

void D3D11GraphicResourceManager::RemoveTexture1D(unsigned int id) {
	CHECK(false)<<"Disabled.";
}
	
D3D11Texture2D * D3D11GraphicResourceManager::CreateTexture2D() {
	D3D11Texture2D *texture = new D3D11Texture2D(this);
	tex2d_map[texture->GetID()] = texture;
	return texture;
}

D3D11Texture2D * D3D11GraphicResourceManager::GetTexture2D(unsigned int id) {
	return tex2d_map[id];
}

void D3D11GraphicResourceManager::RemoveTexture2D(unsigned int id) {
	if(tex2d_map.find(id) != tex2d_map.end()) {
		delete tex2d_map[id];
		tex2d_map.erase(id);
	}
}

D3D11Texture2D * D3D11GraphicResourceManager::GetBackBuffer() {
	return back_buffer;
}	

Texture3D * D3D11GraphicResourceManager::CreateTexture3D() {
	CHECK(false)<<"Disabled.";
	return 0;
}

Texture3D * D3D11GraphicResourceManager::GetTexture3D(unsigned int id) {
	CHECK(false)<<"Disabled.";
	return 0;
}

void D3D11GraphicResourceManager::RemoveTexture3D(unsigned int id) {
	CHECK(false)<<"Disabled.";
}
	
Sampler * D3D11GraphicResourceManager::CreateSampler() {
	CHECK(false)<<"Disabled.";
	return 0;
}

Sampler * D3D11GraphicResourceManager::GetSampler(unsigned int id) {
	CHECK(false)<<"Disabled.";
	return 0;
}

void D3D11GraphicResourceManager::RemoveSampler(unsigned int id) {
	CHECK(false)<<"Disabled.";
}

D3D11VertexShader * D3D11GraphicResourceManager::CreateVertexShader() {
	D3D11VertexShader *shader = new D3D11VertexShader(this);
	vs_map[shader->GetID()] = shader;
	return shader;
}

D3D11VertexShader * D3D11GraphicResourceManager::GetVertexShader(unsigned int id) {
	return vs_map[id];
}

void D3D11GraphicResourceManager::RemoveVertexShader(unsigned int id) {
	if(vs_map.find(id) != vs_map.end()) {
		delete vs_map[id];
		vs_map.erase(id);
	}
}

D3D11PixelShader * D3D11GraphicResourceManager::CreatePixelShader() {
	D3D11PixelShader *shader = new D3D11PixelShader(this);
	ps_map[shader->GetID()] = shader;
	return shader;
}

D3D11PixelShader * D3D11GraphicResourceManager::GetPixelShader(unsigned int id) {
	return ps_map[id];
}

void D3D11GraphicResourceManager::RemovePixelShader(unsigned int id) {
	if(ps_map.find(id) != ps_map.end()) {
		delete ps_map[id];
		ps_map.erase(id);
	}
}

}







