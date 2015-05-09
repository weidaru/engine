#ifndef RENDERER_CONTEXT_H_
#define RENDERER_CONTEXT_H_

#include "utils/s2string.h"

namespace  s2 {
class GraphicPipeline;
class GraphicResourceManager;
class Texture2D;

struct RendererSetting {
	unsigned int window_width;
	unsigned int window_height;
	bool enable_vsync;
	bool full_screen;
		
	RendererSetting() {
		window_width = 800;
		window_height = 600;
		enable_vsync = false;
		full_screen = false;
	}
};

class RendererContext {
public:
	virtual ~RendererContext() {}
	virtual GraphicPipeline * GetPipeline() = 0;
	virtual GraphicResourceManager * GetResourceManager() = 0;
	virtual bool SetSetting(const RendererSetting &_setting) = 0;
	virtual const RendererSetting & GetSetting() const = 0;
	virtual const s2string & GetLastError() = 0;
	virtual void PropagateSetting() = 0;
	virtual void SwapBuffer() = 0;
	virtual Texture2D * GetBackBuffer() = 0;
};

}

#endif 			//RENDERER_CONTEXT_H_