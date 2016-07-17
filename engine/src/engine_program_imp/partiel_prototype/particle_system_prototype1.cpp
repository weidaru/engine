#include "particle_system_prototype1.h"

#include "engine.h"
#include "engine_program.h"

#include "asset/asset_path.h"
#include "asset/pixel_map.h"

#include "graphics/renderer/all.h"
#include "graphics/renderer/renderer_helper.h"

#include <glog/logging.h>
#include <random>

namespace s2 {

class ParticleSystemPrototype1 : public EngineProgram {
public:
	virtual s2string GetName() const override {
		return "ParticleSystemPrototype1";
	}


	virtual void OnEnter() override {

	}

	virtual void OnLeave() override {

	}

	virtual void OneFrame(float delta) override {
		DeclareContext();
		Texture2D *back_texture = Engine::GetSingleton()->GetRendererContext()->GetBackBuffer();
		
		pipeline->ClearDepthStencil(ds_texture->AsDepthStencil(), true, 1.0, true, 0);

		pipeline->SetPrimitiveTopology(GraphicPipeline::POINT_LIST);
		pipeline->SetVertexBuffer(0, render_vb->AsVertexBuffer());
		pipeline->SetRenderTargetAndDepthStencil(0, back_texture->AsRenderTarget(), 0);
		pipeline->SetVertexShaderData(render_vs_data);
		pipeline->SetPixelShaderData(render_ps_data);
		pipeline->SetState(*render_state);
		

		pipeline->Draw(0, render_vb->GetOption().element_count);
		pipeline->Draw(0, 1);
		
	}

	virtual bool Initialize() override {
		DeclareContext();

		uint32_t width = 1024, height = 1024;
		std::vector<float> data(width * height * 4);
		std::default_random_engine generator;
		std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

		Texture2D::Option texture_option;
		texture_option.width = width;
		texture_option.height = height;
		texture_option.format = RendererFormat::R32G32B32A32_FLOAT;
		position_tex = manager->CreateTexture2D();
		for (uint32_t i = 0; i<width*height; i++) {
			data[4 * i] = distribution(generator) * 2.0f - 1.0f;
			data[4 * i + 1] = 1.0f - distribution(generator) * 2.0f;
			data[4 * i + 2] = 0.0f;
			data[4 * i + 3] = 1.0f;
		}
		texture_option.data.SetData(0, 0, &data[0]);
		CHECK(position_tex->Initialize(texture_option)) << position_tex->GetLastError();

		texture_option.format = RendererFormat::R32G32_FLOAT;
		dimension_tex = manager->CreateTexture2D();
		for (uint32_t i = 0; i<width*height; i++) {
			data[2 * i] = 0.03f;
			data[2 * i + 1] = 0.04f;
		}
		texture_option.data.SetData(0, 0, &data[0]);
		CHECK(dimension_tex->Initialize(texture_option)) << dimension_tex->GetLastError();

		texture_option.format = RendererFormat::R32G32B32A32_FLOAT;
		velocity_tex = manager->CreateTexture2D();
		for (uint32_t i = 0; i<width*height; i++) {
			data[4 * i] = (distribution(generator) - 0.5f) * 0.1f;
			data[4 * i + 1] = distribution(generator) * -0.1f;
			data[4 * i + 2] = 0.0f;
			data[4 * i + 3] = 0.0f;
		}
		texture_option.data.SetData(0, 0, &data[0]);
		CHECK(velocity_tex->Initialize(texture_option)) << velocity_tex->GetLastError();

		texture_option.format = RendererFormat::R32G32B32A32_FLOAT;
		acceleration_tex = manager->CreateTexture2D();
		for (uint32_t i = 0; i<width*height; i++) {
			data[4 * i] = 0.0f;
			data[4 * i + 1] = 0.0f;
			data[4 * i + 2] = 0.0f;
			data[4 * i + 3] = 0.0f;
		}
		texture_option.data.SetData(0, 0, &data[0]);
		CHECK(acceleration_tex->Initialize(texture_option)) << acceleration_tex->GetLastError();

		texture_option.format = RendererFormat::R32G32B32A32_FLOAT;
		color_tex = manager->CreateTexture2D();
		for (uint32_t i = 0; i<width*height; i++) {
			float factor = distribution(generator);
			data[4 * i] = 1.0f;
			data[4 * i + 1] = 1.0f;
			data[4 * i + 2] = 1.0f;
			data[4 * i + 3] = factor * 0.7f;
		}
		texture_option.data.SetData(0, 0, &data[0]);
		CHECK(color_tex->Initialize(texture_option)) << color_tex->GetLastError();

		InitializeRenderProgram();
		InitializeUpdateProgram();

		return true;
	}

	void InitializeRenderProgram() {
		DeclareContext();
		RendererSetting setting = context->GetSetting();
		
		ds_texture = manager->CreateTexture2D();
		{
			Texture2D::Option option;
			Texture2D::Option::SetAsDepthStencilBuffer(&option, setting.window_width, setting.window_height);
			ds_texture->Initialize(option);
		}

		render_vs = manager->CreateVertexShader();
		CHECK(render_vs->Initialize(ResolveTestAssetPath("particle/1/render_vs.hlsl"), "main"))
			<< render_vs->GetLastError();

		render_gs = manager->CreateGeometryShader();
		CHECK(render_gs->Initialize(ResolveTestAssetPath("particle/1/render_gs.hlsl"), "main"))
			<< render_gs->GetLastError();

		render_ps = manager->CreatePixelShader();
		CHECK(render_ps->Initialize(ResolveTestAssetPath("particle/1/render_ps.hlsl"), "main"))
			<< render_ps->GetLastError();

		render_vs_data = manager->CreateShaderData();
		CHECK(render_vs_data->Initialize(render_vs->GetShaderBytecode()))
			<<render_vs_data->GetLastError();
		render_vs_data->SetShaderResource("position_tex", position_tex->AsShaderResource());
		render_vs_data->SetShaderResource("dimension_tex", dimension_tex->AsShaderResource());
		render_vs_data->SetShaderResource("velocity_tex", velocity_tex->AsShaderResource());
		
		render_vs_data->SetShaderResource("acceleration_tex", acceleration_tex->AsShaderResource());
		render_vs_data->SetShaderResource("color_tex", color_tex->AsShaderResource());

		render_ps_data = manager->CreateShaderData();
		render_ps_data->Initialize(render_ps->GetShaderBytecode());
		PixelMap particle_image;
		particle_image.Initialize(ResolveTestAssetPath("image/particle.png"), ImagePixelFormat::R8G8B8A8);
		Texture2D::Option tex_option;
		particle_image.PopulateTexture2DOption(&tex_option);
		
		particle_texture = manager->CreateTexture2D();
		particle_texture->Initialize(tex_option);

		particle_sampler = manager->CreateSampler();
		particle_sampler->Initialize(Sampler::Option());

		render_ps_data->SetShaderResource("particle_texture", particle_texture->AsShaderResource());
		render_ps_data->SetSampler("particle_sampler", particle_sampler);

		render_vb = manager->CreateGraphicBuffer();
		GraphicBuffer::Option gbOption;
		std::vector<ParticleVertex> vb_data(2048);
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 1024; j++) {
				vb_data[i * 1024 + j].x = i;
				vb_data[i * 1024 + j].y = j;
			}
		}
		gbOption.Initialize(2048, &(vb_data[0]));
		render_vb->Initialize(gbOption);

		render_layout = manager->CreateInputLayout();
		render_layout->InitializeWithVertexBuffer(
				{ VertexBufferDescriptor::Create<ParticleVertex>(0) },
				*render_vs);

		render_state = context->CreatePipelineState();
		render_state->SetVertexShader(render_vs);
		render_state->SetGeometryShader(render_gs);
		render_state->SetPixelShader(render_ps);
		render_state->SetInputLayout(render_layout);

		BlendOption blend_option;
		blend_option.rt_options[0].src_factor = BlendOption::ONE;
		blend_option.rt_options[0].dest_factor = BlendOption::INV_SRC_ALPHA;
		blend_option.rt_options[0].op = BlendOption::MAX;
		blend_option.rt_options[0].src_alpha_factor = BlendOption::SRC_ALPHA;
		blend_option.rt_options[0].dest_alpha_factor = BlendOption::DEST_ALPHA;
		render_state->SetBlendOption(blend_option);
	}

	void InitializeUpdateProgram() {

	}

	ParticleSystemPrototype1() {
		position_tex = 0;
		dimension_tex = 0;
		velocity_tex = 0;
		acceleration_tex = 0;
		color_tex = 0;
		
		ds_texture = 0;
		
		render_vb = 0;
		render_vs = 0;
		render_vs_data = 0;
		
		render_gs = 0;
		
		render_ps = 0;
		render_ps_data = 0;
		particle_texture = 0;
		particle_sampler = 0;

		render_state = 0;

	}

	virtual ~ParticleSystemPrototype1() {
		delete render_state;
	}

private:
	//Common
	Texture2D *position_tex;
	Texture2D *dimension_tex;
	Texture2D *velocity_tex;
	Texture2D *acceleration_tex;
	Texture2D *color_tex;

	//Render related
	Texture2D *ds_texture;

	GraphicBuffer *render_vb;

	VertexShader *render_vs;
	ShaderData *render_vs_data;

	GeometryShader *render_gs;

	PixelShader *render_ps;
	ShaderData *render_ps_data;
	Texture2D *particle_texture;
	Sampler *particle_sampler;


	InputLayout *render_layout;
	GraphicPipelineState *render_state;
};

AddBeforeMain(ParticleSystemPrototype1)

}