#include <glog/logging.h>

#include "engine_program.h"
#include "engine.h"

#include "graphics/renderer/all.h"

#include "asset/mesh.h"
#include "asset/pixel_map.h"
#include "asset/asset_path.h"

#include "scene/camera.h"
#include "input/input_system.h"
#include "entity/transform.h"

#include <stdio.h>
#include <math.h>

#include "render_to_target_demo.h"

#define PI 3.141596253f

namespace s2 {

class RenderToTargetDemo : public EngineProgram {
public:
	RenderToTargetDemo() :
		ds_buffer(0), texture(0),
		vb(0), ib(0), input_layout(0), vs(0), vs_data(0), ps(0), rotate(0.0f),
		tex_vb(0), tex_input_layout(0), tex_ib(0), tex_vs(0), tex_ps(0), tex_ps_data(0), sampler(0),
		camera(0), normal_state(0), tex_state(0) {
	}

	virtual ~RenderToTargetDemo() {
		delete tex_state;
		delete normal_state;
	}

	virtual bool Initialize() override {
		printf("Initialize test program.\n");
		camera = new Camera(&entity_system);

		//Create and set depth stencil buffer
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		
		//Set depth buffer
		const RendererSetting &renderer_setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
		Texture2D::Option ds_option;
		Texture2D::Option::SetAsDepthStencilBuffer(&ds_option, renderer_setting.window_width, renderer_setting.window_height);
		ds_buffer = manager->CreateTexture2D();
		ds_buffer->Initialize(ds_option);
		
		camera->GetTransform()->SetTranslate(S2Vector3(0.0, 1.0, -40.0f));

		CreateTextureProgram();
		CreateColorProgram();
		
		return true;
	}
	
	virtual s2string GetName() const override {
		return "RenderToTargetDemo";
	}
	
	void CreateColorProgram() {
		RendererContext *context = Engine::GetSingleton()->GetRendererContext();
		GraphicResourceManager *manager = context->GetResourceManager();
		const RendererSetting &renderer_setting = context->GetSetting();
		
		//Create vertex shader
		vs = manager->CreateVertexShader();
		CHECK(vs->Initialize(ResolveTestAssetPath("gouraud.vs"), "main")) <<
			vs->GetLastError();
		vs_data = manager->CreateShaderData();
		CHECK(vs_data->Initialize(vs->GetShaderBytecode()))<<vs_data->GetLastError();

		{
			S2Matrix4x4 identity;
			vs_data->SetUniform("world", identity);
			vs_data->SetUniform("view", camera->GetViewMatrix());
			
			float np=0.5f, fp =1000.0f;
			float aspect=((float)renderer_setting.window_width)/((float)renderer_setting.window_height);
			float fov=PI*35/180;
			S2Matrix4x4 projection;
			projection.SetProjection(aspect, fov, np, fp);

			vs_data->SetUniform("projection", projection);
		}

		//Create PixelShader;
		ps = manager->CreatePixelShader();
		CHECK(ps->Initialize(ResolveTestAssetPath("gouraud.ps"), "main")) <<
			ps->GetLastError();
		
		//Create VertexBuffer
		Mesh model;
		CHECK(model.Initialize(ResolveTestAssetPath("model/bunny.obj"))) << model.GetLastError();
		{
			RTTTestVertex *vertices = 0;
			
	 		uint32_t size = model.GetVertexSize();
			vertices = new RTTTestVertex[size];
			for(uint32_t i=0; i<size; i++) {
				S2Vector3 p = model.GetPosition(i);
				vertices[i].position[0] = p[0];
				vertices[i].position[1] = p[1];
				vertices[i].position[2] = p[2];

				S2Vector3 n = model.GetNormal(i);
				vertices[i].normal[0] = n[0];
				vertices[i].normal[1] = n[1];
				vertices[i].normal[2] = n[2];
			}

			vb = manager->CreateGraphicBuffer();
			GraphicBuffer::Option option;
			option.Initialize(size, vertices);
			vb->Initialize(option);
			delete[] vertices;
		}
		
		{
			//Create IndexBuffer
			uint32_t size = model.GetTriangleSize()*3;
			GraphicBuffer::IndexBufferElementType*indices = new GraphicBuffer::IndexBufferElementType[size];
			for(uint32_t i=0; i<model.GetTriangleSize(); i++) {
				indices[i*3] = model.GetTriangleVertexIndex(i, 0);
				indices[i*3+1] = model.GetTriangleVertexIndex(i, 1);
				indices[i*3+2] = model.GetTriangleVertexIndex(i, 2);
			}
			ib = manager->CreateGraphicBuffer();
			GraphicBuffer::Option option;
			option.InitializeAsIndexBuffer(size, indices);
			option.resource_write = RendererEnum::IMMUTABLE;
			ib->Initialize(option);
			delete[] indices;
		}

		//Create InputLayout
		input_layout = manager->CreateInputLayout();
		input_layout->InitializeWithVertexBuffer({VertexBufferDescriptor::Create<RTTTestVertex>(0)}, *vs);

		//Cache pipeline state.
		normal_state = context->CreatePipelineState();
		normal_state->SetVertexShader(vs);
		normal_state->SetPixelShader(ps);
		normal_state->SetInputLayout(input_layout);

	}
	
	void CreateTextureProgram() {
		RendererContext *context = Engine::GetSingleton()->GetRendererContext();
		GraphicPipeline *pipeline = context->GetPipeline();
		GraphicResourceManager *manager = context->GetResourceManager();
		const RendererSetting &renderer_setting = context->GetSetting();

		//Create texture.
		Texture2D::Option tex_option;
		tex_option.format = RendererEnum::R32G32B32A32_FLOAT;
		texture = manager->CreateTexture2D();
		tex_option.output_bind = TextureEnum::RENDER_TARGET;
		tex_option.width = renderer_setting.window_width;
		tex_option.height = renderer_setting.window_height;
		texture->Initialize(tex_option);
		
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
		tex_ps_data->SetShaderResource("shader_texture", texture->AsShaderResource());
		
		//Set vertex buffer
		RTTTestTextureVertex vertices[4] = {
			{{0.5f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}},
			{{1.0f, 0.5f, 0.0f}, {1.0f, 1.0f}}, 
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
		buffer_option.resource_write = RendererEnum::IMMUTABLE;
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
	
	void DrawNormal(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();

		rotate += delta*PI / 2.0f;
		rotate = rotate>2 * PI ? rotate - 2 * PI : rotate;
		S2Matrix4x4 rotation_mat;
		rotation_mat.SetRotationY(rotate);
		vs_data->SetUniform("world", rotation_mat);
		vs_data->SetUniform("view", camera->GetViewMatrix());
		vs_data->FlushConstantBuffer(pipeline);

		pipeline->SetState(*normal_state);

		pipeline->SetPrimitiveTopology(GraphicPipeline::TRIANGLE_LIST);
		pipeline->SetVertexShaderData(vs_data);
		pipeline->SetDepthStencil(ds_buffer->AsDepthStencil());
		pipeline->SetRenderTarget(0, Engine::GetSingleton()->GetRendererContext()->GetBackBuffer()->AsRenderTarget());
		pipeline->SetRenderTarget(1, texture->AsRenderTarget());
		pipeline->SetVertexBuffer(0, vb->AsVertexBuffer());
		pipeline->SetIndexBuffer(ib->AsIndexBuffer());
		pipeline->DrawIndex(0, ib->GetElementCount());
	}
	
	void DrawTexture(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();

		pipeline->SetState(*tex_state);

		pipeline->SetPrimitiveTopology(GraphicPipeline::TRIANGLE_LIST);
		pipeline->SetRenderTarget(1, 0);
		pipeline->SetDepthStencil(ds_buffer->AsDepthStencil());
		pipeline->SetPixelShaderData(tex_ps_data);
		pipeline->SetRenderTarget(0, Engine::GetSingleton()->GetRendererContext()->GetBackBuffer()->AsRenderTarget());
		pipeline->SetVertexBuffer(0, tex_vb->AsVertexBuffer());
		pipeline->SetIndexBuffer(tex_ib->AsIndexBuffer());
		pipeline->DrawIndex(0, tex_ib->GetElementCount());
	}
	
	virtual void OneFrame(float delta) override {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		pipeline->ClearDepthStencil(ds_buffer->AsDepthStencil(), true, 1.0f, true, 0);
		float red[4] = {1.0f, 0.0f, 0.0f, 1.0f};
		pipeline->ClearRenderTarget(texture->AsRenderTarget(), red);
		DrawNormal(delta);
		DrawTexture(delta);
	}

	virtual void OnEnter() override {
		Engine::GetSingleton()->GetInputSystem()->SetMouseVisible(false);
	}
	
private:
	Texture2D *ds_buffer;
	Texture2D *texture;
	
	GraphicBuffer *vb;
	GraphicBuffer *ib;
	InputLayout *input_layout;
	VertexShader *vs;
	ShaderData *vs_data;
	PixelShader *ps;
	float rotate;
	
	GraphicBuffer *tex_vb;
	GraphicBuffer *tex_ib;
	InputLayout *tex_input_layout;
	VertexShader *tex_vs;
	PixelShader *tex_ps;
	ShaderData *tex_ps_data;
	Sampler *sampler;

	GraphicPipelineState *normal_state, *tex_state;
	Camera *camera;
};

AddBeforeMain(RenderToTargetDemo)

}

