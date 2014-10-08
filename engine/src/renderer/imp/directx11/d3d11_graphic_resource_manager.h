#ifndef D3D11_GRAPHIC_RESOURCE_MANAGER_H_
#define D3D11_GRAPHIC_RESOURCE_MANAGER_H_

#include "renderer/graphic_resource_manager.h"

#include "d3d11_texture2d.h"
#include "d3d11_vertex_buffer.h"
#include "d3d11_index_buffer.h"
#include "d3d11_vertex_shader.h"
#include "d3d11_pixel_shader.h"

#include <map>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;

namespace s2 {

/**
 * Right now, only single window is  used. So world is simple, single window, single swapchain, single backbuffer(We only use double buffering.)
 */
class D3D11GraphicResourceManager : public GraphicResourceManager {
public:
	D3D11GraphicResourceManager(unsigned int window_width, unsigned int window_height, 
												void *hwnd, bool enable_vsync, bool full_screen);
	virtual ~D3D11GraphicResourceManager();

	/**************Inherited stuffs************/
	//VertexBuffer
	virtual D3D11VertexBuffer * 		CreateVertexBuffer(unsigned int size, const void* data, bool is_dynamic);
	virtual D3D11VertexBuffer * 		GetVertexBuffer(unsigned int id);
	virtual void 					RemoveVertexBuffer(unsigned int id);
	
	//IndexBuffer
	virtual D3D11IndexBuffer *		CreateIndexBuffer(unsigned int size, const uint32_t* data, bool is_dynamic);
	virtual D3D11IndexBuffer * 		GetIndexBuffer(unsigned int id);
	virtual void 					RemoveIndexBuffer(unsigned int id);
	
	//Texture1D
	virtual Texture1D * 			CreateTexture1D(const Texture1D::Option &option);
	virtual Texture1D * 			GetTexture1D(unsigned int id);
	virtual void 					RemoveTexture1D(unsigned int id);
	
	//Texture2D
	virtual D3D11Texture2D * 			CreateTexture2D(const Texture2D::Option &option);
	virtual D3D11Texture2D * 			GetTexture2D(unsigned int id);
	virtual void 					RemoveTexture2D(unsigned int id);
	virtual D3D11Texture2D *			GetBackBuffer();
	
	//Texture3D
	virtual Texture3D * 			CreateTexture3D(const Texture1D::Option &option);
	virtual Texture3D * 			GetTexture3D(unsigned int id);
	virtual void 					RemoveTexture3D(unsigned int id);
	
	//Sampler
	virtual Sampler * 			CreateSampler(const Texture1D::Option &option);
	virtual Sampler * 			GetSampler(unsigned int id);
	virtual void 					RemoveSampler(unsigned int id);
	
	//VertexShader
	virtual D3D11VertexShader * 		CreateVertexShader(const s2string &path, VertexShader::PrimitiveTopology topology);
	virtual D3D11VertexShader *		GetVertexShader(unsigned int id);
	virtual void 					RemoveVertexShader(unsigned int id);
	
	//PixelShader
	virtual D3D11PixelShader * 		CreatePixelShader(const s2string &path);
	virtual D3D11PixelShader *		GetPixelShader(unsigned int id);
	virtual void 					RemovePixelShader(unsigned int id);
	
	virtual void 					GetLastError(s2string *str);
	
	
	/****************D3D11 exclusive*****************/
public:
	ID3D11Device * GetDevice() { return device; }
	ID3D11DeviceContext * GetDeviceContext() { return context; }
	
private:
	/*	The assumption is that we are only using one monitor thus only one swapchain is needed.
		Refactor if support for multiple monitors is needed. */
	void InitDeviceAndContextAndSwapchain(unsigned int screen_width, unsigned int screen_height, void *hwnd, bool enable_vsync, bool full_screen);
	
	D3D11GraphicResourceManager(const D3D11GraphicResourceManager &);
	D3D11GraphicResourceManager & operator=(const D3D11GraphicResourceManager &);
	
	
private:
	ID3D11Device *device;
	ID3D11DeviceContext  *context;
	IDXGISwapChain *swap_chain;
	D3D11Texture2D * back_buffer;				//This go with the swap_chain
	
	s2string error;
	
	std::map<unsigned int, D3D11VertexBuffer *> vb_map;
	std::map<unsigned int, D3D11IndexBuffer *> ib_map;
	std::map<unsigned int, Texture1D *> tex1d_map;
	std::map<unsigned int, D3D11Texture2D *> tex2d_map;
	std::map<unsigned int, Texture3D *> tex3d_map;
	std::map<unsigned int, Sampler *> sampler_map;
	std::map<unsigned int, D3D11VertexShader *> vs_map;
	std::map<unsigned int, D3D11PixelShader *> ps_map;
};


}



#endif 			//D3D11_GRAPHIC_RESOURCE_MANAGER_H_
