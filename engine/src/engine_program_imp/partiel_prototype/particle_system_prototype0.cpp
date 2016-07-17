#include "particle_system_prototype0.h"

#include "engine_program.h"
#include "engine.h"
#include "input/input_system.h"

#include "asset/asset_path.h"

#include "graphics/renderer/all.h"

#include "utils/s2matrix4x4.h"

#include "texture_paint.h"

#include <glog/logging.h>
#include <random>

namespace s2 {

class ParticleSystemPrototype0 : public EngineProgram {
public:
	ParticleSystemPrototype0() {
		position_tex = 0;
		update_vs = 0;
		update_ps = 0;
		update_ps_shader_data = 0;
		quad_buffer = 0;
		update_state = 0;
		update_input_layout = 0;

		ds_texture = 0;
		render_vs = 0;
		render_vs_data = 0;
		render_ps = 0;
		render_state = 0;

		texture_paint = 0;
	}

	virtual ~ParticleSystemPrototype0() {
		delete texture_paint;
		delete render_state;
		delete update_state;
	}

	virtual bool Initialize() override {
		InitializeUpdateProgram();
		InitializeRenderProgram();

		texture_paint = new TexturePaint();

		return true;
	}

	void InitializeUpdateProgram() {
		RendererContext *context = Engine::GetSingleton()->GetRendererContext();
		GraphicResourceManager *manager = context->GetResourceManager();

		uint32_t width = 128, height = 128;
		position_tex = manager->CreateTexture2D();
		old_position_tex = manager->CreateTexture2D();
		{
			Texture2D::Option option;
			option.width = 128;
			option.height = 128;
			option.format = RendererFormat::R32G32B32A32_FLOAT;
			option.binding = RendererBinding::SHADER_RESOURCE | RendererBinding::RENDER_TARGET;
			position_tex->Initialize(option);

			std::vector<float> data(width * height*4);
			std::default_random_engine generator;
			std::uniform_real_distribution<float> distribution(0.0f,1.0f);
			for(uint32_t i=0; i<width*height; i++) {
				data[4*i] = distribution(generator) * 2.0f - 1.0f;
				data[4*i+1] = 1.0f - distribution(generator) * 0.1f;
				data[4*i+2] = 0.0f;
				data[4*i+3] = 1.0f;
			}
			option.data.SetData(0, 0, &data[0]);
			old_position_tex->Initialize(option);
		}

		update_vs = manager->CreateVertexShader();
		CHECK(update_vs->Initialize(ResolveTestAssetPath("particle/0/update_vs.hlsl"), "main"))<<update_vs->GetLastError();

		update_ps = manager->CreatePixelShader();
		CHECK(update_ps->Initialize(ResolveTestAssetPath("particle/0/update_ps.hlsl"), "main"))<<update_ps->GetLastError();

		simple_sampler = manager->CreateSampler();
		{
			Sampler::Option option;
			simple_sampler->Initialize(option);
		}

		update_ps_shader_data = manager->CreateShaderData();
		update_ps_shader_data->Initialize(update_ps->GetShaderBytecode());
		update_ps_shader_data->SetSampler("simple_sampler", simple_sampler);

		quad_buffer = manager->CreateGraphicBuffer();
		{
			float vb_data[][3] = {
				{ -1.0f, -1.0f, 0.0f },
				{ 1.0f, -1.0f, 0.0f },
				{-1.0f, 1.0f, 0.0f},
				{ 1.0f, 1.0f, 0.0f },
			};
			quad_buffer = manager->CreateGraphicBuffer();
			GraphicBuffer::Option vb_option;
			vb_option.element_count = 4;
			vb_option.element_member_count = 1;
			vb_option.element_bytewidth = 12;
			vb_option.data = vb_data;
			vb_option.binding = RendererBinding::VERTEX_BUFFER;
			quad_buffer->Initialize(vb_option);
		}

		update_input_layout = manager->CreateInputLayout();
		update_input_layout->InitializeWithElement({{0, 0}}, *update_vs);

		update_state =  context->CreatePipelineState();
		update_state->SetVertexShader(update_vs);
		update_state->SetPixelShader(update_ps);
		update_state->SetInputLayout(update_input_layout);
	}

	void InitializeRenderProgram() {
		RendererContext *context = Engine::GetSingleton()->GetRendererContext();
		RendererSetting setting = context->GetSetting();
		GraphicResourceManager *manager = context->GetResourceManager();

		render_vs = manager->CreateVertexShader();
		CHECK(render_vs->Initialize(ResolveTestAssetPath("particle/0/render_vs.hlsl"), "main"))<<render_vs->GetLastError();

		render_ps = manager->CreatePixelShader();
		CHECK(render_ps->Initialize(ResolveTestAssetPath("particle/0/render_ps.hlsl"), "main"))<<render_ps->GetLastError();

		ds_texture = manager->CreateTexture2D();
		{
			Texture2D::Option option;
			Texture2D::Option::SetAsDepthStencilBuffer(&option, setting.window_width, setting.window_height);
			ds_texture->Initialize(option);
		}

		render_vs_data = manager->CreateShaderData();
		render_vs_data->Initialize(render_vs->GetShaderBytecode());

		render_state = context->CreatePipelineState();
		render_state->SetVertexShader(render_vs);
		render_state->SetPixelShader(render_ps);
	}

	virtual s2string GetName() const override {
		return "ParticleSystemPrototype0";
	}


	virtual void OnEnter() override {
		InputSystem *input_system = Engine::GetSingleton()->GetInputSystem();
		input_system->SetMouseVisible(true);
	}

	virtual void OnLeave() override {
	
	}

	virtual void OneFrame(float delta) override {
		RendererContext *context = Engine::GetSingleton()->GetRendererContext();
		RendererSetting setting = context->GetSetting();
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		uint32_t width = position_tex->GetOption().width, height = position_tex->GetOption().height;

		pipeline->ClearDepthStencil(ds_texture->AsDepthStencil(), true, 1.0, true, 0);

		//Spawn

		//Update
		update_ps_shader_data->SetUniform("width", width);
		update_ps_shader_data->SetUniform("height", height);
		update_ps_shader_data->SetShaderResource("old_position", old_position_tex->AsShaderResource());
		update_ps_shader_data->SetUniform("time_delta", delta);
		update_ps_shader_data->FlushConstantBuffer(pipeline);

		pipeline->SetVertexBuffer(0, quad_buffer->AsVertexBuffer());
		pipeline->SetRenderTargetAndDepthStencil(0, position_tex->AsRenderTarget(), 0);
		pipeline->SetState(*update_state);
		pipeline->SetPrimitiveTopology(GraphicPipeline::TRIANGLE_STRIP);
		pipeline->SetPixelShaderData(update_ps_shader_data);
		pipeline->Draw(0, quad_buffer->GetOption().element_count);
		

		//Render
		Texture2D *back_texture = Engine::GetSingleton()->GetRendererContext()->GetBackBuffer();
		/*For debug.*/
		render_vs_data->SetShaderResource("position_tex", position_tex->AsShaderResource());
		render_vs_data->SetUniform("width", width);
		render_vs_data->SetUniform("height", height);
		render_vs_data->FlushConstantBuffer(pipeline);

		pipeline->SetVertexBuffer(0, 0);
		pipeline->SetRenderTargetAndDepthStencil(0, back_texture->AsRenderTarget(), 0);
		pipeline->SetPrimitiveTopology(GraphicPipeline::POINT_LIST);
		pipeline->SetVertexShaderData(render_vs_data);
		pipeline->SetState(*render_state);
		pipeline->Draw(0, width * height);

		//Swap old data and new data.
		std::swap(old_position_tex, position_tex);
	}

private:
	Texture2D *old_position_tex, *position_tex;

	//Shared
	Sampler *simple_sampler;

	//Update related
	VertexShader *update_vs;
	PixelShader *update_ps;
	ShaderData *update_ps_shader_data;
	GraphicPipelineState *update_state;
	GraphicBuffer *quad_buffer;
	InputLayout *update_input_layout;

	//Render related
	Texture2D *ds_texture;
	VertexShader *render_vs;
	ShaderData *render_vs_data;
	PixelShader *render_ps;
	GraphicPipelineState *render_state;
	

	//For debugging.
	TexturePaint *texture_paint;
};

AddBeforeMain(ParticleSystemPrototype0)

}