#include "particle_system_prototype.h"

#include "engine_program.h"
#include "engine.h"

#include "graphics/renderer/all.h"

namespace s2 {

class ParticleSystemPrototype : public EngineProgram {
public:
	ParticleSystemPrototype() {
		position_tex = 0;
		position_update_vs = 0;
		position_update_ps = 0;

		render_vb = 0;
		render_vs = 0;
		render_ps = 0;
	}

	virtual ~ParticleSystemPrototype() {

	}

	virtual bool Initialize() override {
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		position_tex = manager->CreateTexture2D();
		{
			Texture2D::Option option;
			option.width = 512;
			option.height = 512;
			option.format = RendererEnum::R32G32B32A32_FLOAT;
			option.input_bind = TextureEnum::SHADER_RESOURCE;
			option.output_bind = TextureEnum::RENDER_TARGET;
			position_tex->Initialize(option);
		}



	}

	virtual s2string GetName() const override {
		return "ParticleSystemPrototype";
	}


	virtual void OnEnter() override {
	
	}

	virtual void OnLeave() override {
	
	}

	virtual void OneFrame(float delta) override {
		//Spawn

		//Update

		//Render
	}

private:
	Texture2D *position_tex;

	VertexShader *position_update_vs;
	PixelShader *position_update_ps;

	GraphicBuffer *render_vb;
	VertexShader *render_vs;
	GeometryShader *render_gs;
	PixelShader *render_ps;
};

AddBeforeMain(ParticleSystemPrototype)

}