#ifndef D3D_RESOURCE_MANAGER_H_
#define D3D_RESOURCE_MANAGER_H_

#include "utils/singleton.h"
#include "d3d_option.h"

struct VideoCardInfo {
	char name[128];
	unsigned int refresh_numerator;
	unsigned int refresh_denominator;
	unsigned int memory_size;
};

struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct D3DSwapChain {
	IDXGISwapChain *swap_chain;
	ID3D11RenderTargetView *bf_view;

	D3DSwapChain();
};

struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
struct D3DDepthStencil{
	ID3D11Texture2D *buffer;
	ID3D11DepthStencilView* view;
};

struct ID3D11Device;
struct ID3D11DeviceContext;

class D3DBufferFactory;
class D3DDepthStencilStateFactory;
class D3DRasterizerStateFactory;
class D3DTextureFactory;
class D3DViewportFactory;

//All the D3D related creation and destruction goes here.
class D3DResourceManager : public Singleton<D3DResourceManager> {
public:
	void Init();
	void Release();

	ID3D11Device* GetDevice() { return device_; }
	ID3D11DeviceContext* GetDeviceContext() { return context_; }
	VideoCardInfo * GetVideoCardInfo() { return &adaper_info_; }
	D3DSwapChain  * GetSwapChain()  { return &swap_chain_; }
	D3DDepthStencil * GetDepthStencil() { return &depth_stencil_; }

	D3DBufferFactory * GetBufferFactory() {  return buffer_factory_; }
	D3DDepthStencilStateFactory * GetDepthStencilStateFactory() { return ds_state_factory_; }
	D3DRasterizerStateFactory * GetRasterizerStateFactory() { return rasterizer_state_factory_; }
	D3DTextureFactory * GetTextureFactory() { return texture_factory_; }
	D3DViewportFactory * GetViewportFactory() { return viewport_factory_; }

private:
	void InitVideoCardInfo();
	void InitDeviceAndSwapChain();
	//MARK: Maybe allow multiple depth stencil buffer in the future?
	void InitDepthStencilBuffer();
	
private:
	ID3D11Device *device_;
	ID3D11DeviceContext *context_;
	VideoCardInfo adaper_info_;
	D3DSwapChain swap_chain_;
	D3DDepthStencil depth_stencil_;

	D3DBufferFactory *buffer_factory_;
	D3DDepthStencilStateFactory *ds_state_factory_;
	D3DRasterizerStateFactory *rasterizer_state_factory_;
	D3DTextureFactory *texture_factory_;
	D3DViewportFactory *viewport_factory_;
};

#endif		//D3D_RESOURCE_MANAGER_H_