#include "engine.h"
#include "engine_program.h"

#include "compute_shader_demo.h"

#include "entity/entity.h"

#include "graphics/renderer/all.h"
#include "graphics/sprite/sprite.h"
#include "asset/asset_path.h"

#include <glog/logging.h>

namespace s2 {

class ComputeShaderDemo : public EngineProgram {
public:
	ComputeShaderDemo()  : 
		cs(0), cs_data(0), tex(0), state(0),
		tex_vb(0), tex_input_layout(0), tex_ib(0), tex_vs(0), tex_ps(0), tex_ps_data(0), sampler(0), tex_state(0){
	
	}

	virtual ~ComputeShaderDemo(){
		delete state;
		delete tex_state;
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
		tex_opt.input_bind = RendererInputBind::SHADER_RESOURCE;
		tex_opt.output_bind = RendererOutputBind::UNORDERED_ACCESS;
		tex->Initialize(tex_opt);

		cs_data = manager->CreateShaderData();
		CHECK(cs_data->Initialize(cs->GetShaderBytecode()))<<cs_data->GetLastError();
		cs_data->SetUnorderedAccess("output", tex->AsUnorderedAccess(), 0);

		state = context->CreatePipelineState();
		state->SetComputeShader(cs);

		CreateTextureProgram();

		return true;
	}

	void CreateTextureProgram() {
		RendererContext *context = Engine::GetSingleton()->GetRendererContext();
		GraphicPipeline *pipeline = context->GetPipeline();
		GraphicResourceManager *manager = context->GetResourceManager();
		
		//Create sampler
		sampler = manager->CreateSampler();
		Sampler::Option option;
		sampler->Initialize(option);

		//Set vertex shader
		tex_vs = manager->CreateVertexShader();
		CHECK(tex_vs->Initialize(ResolveTestAssetPath("texture.vs"), "main")) <<
			tex_vs->GetLastError();

		//Set pixel shader
		tex_ps = manager->CreatePixelShader();
		CHECK(tex_ps->Initialize(ResolveTestAssetPath("texture.ps"), "main")) <<
			tex_ps->GetLastError();
		tex_ps_data = manager->CreateShaderData();
		tex_ps_data->Initialize(tex_ps->GetShaderBytecode());
		tex_ps_data->SetSampler("shader_sampler", sampler);
		tex_ps_data->SetShaderResource("shader_texture", tex->AsShaderResource());
		
		//Set vertex buffer
		CSDemoTextureVertex vertices[4] = {
			{{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
			{{1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, 
			{{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}
		};
		tex_vb = manager->CreateGraphicBuffer();
		GraphicBuffer::Option buffer_option;
		buffer_option.Initialize(4, vertices);
		tex_vb->Initialize(buffer_option);
		
		//Set index buffer
		GraphicBuffer::IndexBufferElementType indices[6] = {0,1,2, 0,2,3};
		tex_ib = manager->CreateGraphicBuffer();
		buffer_option.InitializeAsIndexBuffer(6, indices);
		buffer_option.resource_write = RendererResourceWrite::IMMUTABLE;
		tex_ib->Initialize(buffer_option);

		//Create input layout
		tex_input_layout = manager->CreateInputLayout();
		tex_input_layout->InitializeWithElement(
		{{0, 0}, {0, 12}}, 
			*tex_vs);

		//Create pipeline state
		tex_state = context->CreatePipelineState();
		tex_state->SetVertexShader(tex_vs);
		tex_state->SetPixelShader(tex_ps);
		tex_state->SetInputLayout(tex_input_layout);
	}

	void DrawTexture(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();

		pipeline->SetState(*tex_state);

		pipeline->SetPrimitiveTopology(GraphicPipeline::TRIANGLE_LIST);
		pipeline->SetRenderTargetAndDepthStencil(1, 0, 0);
		pipeline->SetPixelShaderData(tex_ps_data);
		pipeline->SetRenderTargetAndDepthStencil(
			0, Engine::GetSingleton()->GetRendererContext()->GetBackBuffer()->AsRenderTarget(), 0);
		pipeline->SetVertexBuffer(0, tex_vb->AsVertexBuffer());
		pipeline->SetIndexBuffer(tex_ib->AsIndexBuffer());
		pipeline->DrawIndex(0, tex_ib->GetElementCount());
	}

	virtual s2string GetName() const override {
		return "ComputeShaderDemo";
	}

	virtual void OnEnter() override {
	
	}

	virtual void OnLeave() override {
	
	}

	virtual void OneFrame(float delta) override {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();

		pipeline->SetState(*state);
		pipeline->SetComputeShaderData(cs_data);
		pipeline->Dispatch(32, 32, 1);
		pipeline->SetComputeShaderData(0);
		DrawTexture(delta);
	}

private:
	

	ComputeShader *cs;
	ShaderData *cs_data;
	Texture2D *tex;
	GraphicPipelineState *state;

	GraphicBuffer *tex_vb;
	GraphicBuffer *tex_ib;
	InputLayout *tex_input_layout;
	VertexShader *tex_vs;
	PixelShader *tex_ps;
	ShaderData *tex_ps_data;
	Sampler *sampler;

	GraphicPipelineState *tex_state;
};


AddBeforeMain(ComputeShaderDemo)
}
