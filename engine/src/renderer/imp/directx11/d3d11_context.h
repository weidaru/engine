#ifndef D3D11_CONTEXT_H_
#define D3D11_CONTEXT_H_

#include "renderer/renderer_context.h"
#include "d3d11_graphic_resource_manager.h"
#include "d3d11_graphic_pipeline.h"

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
	virtual bool SetSetting(const RendererSetting &_setting);
	virtual void GetSetting(RendererSetting *_setting);
	virtual void GetLastError(s2string *str);
	virtual void PropagateSetting();
	virtual void SwapBuffer();
	
private:
	D3D11GraphicResourceManager *resource_manager;
	D3D11GraphicPipeline *pipeline;
	
	s2string error;
	RendererSetting setting;
};

}




#endif			//D3D11_CONTEXT_H_