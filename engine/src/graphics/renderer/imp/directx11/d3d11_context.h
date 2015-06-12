#ifndef D3D11_CONTEXT_H_
#define D3D11_CONTEXT_H_

#include "graphics/renderer/renderer_context.h"
#include "d3d11_graphic_resource_manager.h"
#include "d3d11_graphic_pipeline.h"
#include "d3d11_texture2d.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;

namespace s2 {

/* Class used for holding everything under Windows and D3D11.
 * Things like HWD, window settings, d3d11 pipeline, d3d11 graphic resource manager and so on.
*/
class D3D11Context: public RendererContext {
public:
	D3D11Context();
	void Initialize(void *hwnd);
	
	virtual ~D3D11Context();
	virtual D3D11GraphicPipeline * GetPipeline() { return pipeline; }
	virtual D3D11GraphicResourceManager * GetResourceManager() { return resource_manager; }
	virtual bool SetSetting(const RendererSetting &_setting) ;
	virtual const RendererSetting & GetSetting() const;
	virtual const s2string & GetLastError() { return error; }
	virtual void PropagateSetting();
	virtual void SwapBuffer();
	virtual D3D11Texture2D * GetBackBuffer();

private:
	void InitDeviceAndContextAndSwapchain(
		uint32_t screen_width, uint32_t screen_height, void *hwnd, bool enable_vsync, bool full_screen,
		ID3D11Device **device, ID3D11DeviceContext **context);
	
private:
	IDXGISwapChain *swap_chain;
	D3D11Texture2D * back_buffer;				//This go with the swap_chain

	D3D11GraphicResourceManager *resource_manager;
	D3D11GraphicPipeline *pipeline;
	
	s2string error;
	RendererSetting setting;
};

}




#endif			//D3D11_CONTEXT_H_