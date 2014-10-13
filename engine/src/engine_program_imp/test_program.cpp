#include "engine_program.h"
#include "engine.h"

#include "renderer/graphic_pipeline.h"
#include "renderer/graphic_resource_manager.h"

#include "renderer/texture2d.h"

#include <stdio.h>

namespace s2 {

class TestProgram : public EngineProgram {
public:
	TestProgram():ds_buffer(0) {}

	virtual ~TestProgram() {}
	virtual bool Initialize(){
		printf("Initialize test program.\n");
		
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		
		//Back buffer as render target 
		pipeline->SetRenderTarget(0, manager->GetBackBuffer());
		
		//Create and set depth stencil buffer
		RendererSetting renderer_setting;
		Engine::GetSingleton()->GetRendererContext()->GetSetting(&renderer_setting);
		Texture2D::Option ds_option;
		Texture2D::Option::SetAsDepthStencilBuffer(&ds_option, renderer_setting.window_width, renderer_setting.window_height);
		Texture2D *ds_buffer = manager->CreateTexture2D(ds_option);
		pipeline->SetDepthStencilBuffer(ds_buffer);
		
		return true;
	}
	
	virtual const s2string & GetName() {
		static s2string name="test";
		return name;
	}
	
	virtual void OneFrame(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
	
		float red[4] = {1.0f, 0.0f, 0.0f, 1.0f};
		pipeline->SetRenderTargetClearOption(0, true, red);
		pipeline->Draw();
	}
	
private:
	Texture2D *ds_buffer;
};

AddBeforeMain(TestProgram)

}

