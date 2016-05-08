#include "engine.h"
#include "engine_program.h"

#include "entity/entity.h"

#include "graphics/renderer/all.h"
#include "graphics/sprite/sprite.h"
#include "graphics/sprite/sprite_system.h"
#include "asset/asset_path.h"

#include <glog/logging.h>

namespace s2 {

class ComputeShaderDemo : public EngineProgram {
public:
	ComputeShaderDemo()  : 
		cs(0), cs_data(0), tex(0), state(0), picture(0) {
	
	}

	virtual ~ComputeShaderDemo(){
		delete state;
	}

	virtual bool Initialize() override {
		RendererContext *context = Engine::GetSingleton()->GetRendererContext();
		GraphicResourceManager *manager = context->GetResourceManager();
		
		cs = manager->CreateComputeShader();
		CHECK(cs->Initialize(ResolveTestAssetPath("csdemo_cs.hlsl"), "main"))<<cs->GetLastError();
		
		tex = manager->CreateTexture2D();
		Texture2D::Option tex_opt;
		tex_opt.width = 1024;
		tex_opt.height = 1024;
		tex_opt.binding = RendererBinding::SHADER_RESOURCE | RendererBinding::UNORDERED_ACCESS;
		tex->Initialize(tex_opt);

		cs_data = manager->CreateShaderData();
		CHECK(cs_data->Initialize(cs->GetShaderBytecode()))<<cs_data->GetLastError();
		cs_data->SetUnorderedAccess("output", tex->AsUnorderedAccess(), 0);

		state = context->CreatePipelineState();
		state->SetComputeShader(cs);

		picture = new Entity(&entity_system);
		picture->SetEnabled(false);
		Sprite * sprite = new Sprite(picture);
		Engine::GetSingleton()->GetSpriteSystem()->Register(sprite);
		sprite->SetTexture(tex);
		sprite->SetWidth(2.0);
		sprite->SetHeight(2.0);

		return true;
	}


	virtual s2string GetName() const override {
		return "ComputeShaderDemo";
	}

	virtual void OnEnter() override {
		picture->SetEnabled(true);
	}

	virtual void OnLeave() override {
		picture->SetEnabled(false);
	}

	virtual void OneFrame(float delta) override {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();

		pipeline->SetState(*state);
		pipeline->SetComputeShaderData(cs_data);
		pipeline->Dispatch(32, 32, 1);
		pipeline->SetComputeShaderData(0);
	}

private:
	Entity *picture;

	ComputeShader *cs;
	ShaderData *cs_data;
	Texture2D *tex;
	GraphicPipelineState *state;

};


AddBeforeMain(ComputeShaderDemo)
}
