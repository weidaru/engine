#include <glog/logging.h>

#include "engine_program.h"
#include "engine.h"

#include "graphics/renderer/all.h"

#include "asset/model.h"
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
		vb(0), ib(0), vs(0), ps(0), rotate(0.0f),
		tex_vb(0), tex_ib(0), tex_vs(0), tex_ps(0), sampler(0),
		normal_draw_state(0), rtt_draw_state(0),
		camera(0){
	}

	virtual ~RenderToTargetDemo() {
		delete normal_draw_state;
		delete rtt_draw_state;
		delete camera;
	}

	virtual bool Initialize(){
		printf("Initialize test program.\n");
		camera = new Camera(Engine::GetSingleton()->GetEntitySystem());

		//Create and set depth stencil buffer
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		
		//Set depth buffer
		const RendererSetting &renderer_setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
		Texture2D::Option ds_option;
		Texture2D::Option::SetAsDepthStencilBuffer(&ds_option, renderer_setting.window_width, renderer_setting.window_height);
		ds_buffer = manager->CreateTexture2D();
		ds_buffer->Initialize(ds_option);
		
		camera->GetTransform()->SetTranslate(Vector3(0.0, 1.0, -40.0f));
		camera->SetDirectionVector(Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0, 1.0f, 0.0f));
		
		CreateColorProgram();
		CreateTextureProgram();
		
		
		return true;
	}
	
	virtual s2string GetName() const {
		return "RenderToTargetDemo";
	}
	
	void CreateColorProgram() {
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		const RendererSetting &renderer_setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
		
		//Create vertex shader
		vs = manager->CreateVertexShader();
		CHECK(vs->Initialize(ResolveTestAssetPath("gouraud.vs"), "main")) <<
			vs->GetLastError();
		{
			Matrix4x4 identity;
			vs->SetUniform("world", identity);
			vs->SetUniform("view", camera->GetViewMatrix());
			
			float np=0.5f, fp =1000.0f;
			float aspect=((float)renderer_setting.window_width)/((float)renderer_setting.window_height);
			float fov=PI*35/180;
			Matrix4x4 projection;
			projection.SetProjection(aspect, fov, np, fp);

			vs->SetUniform("projection", projection);
		}

		//Create PixelShader;
		ps = manager->CreatePixelShader();
		CHECK(ps->Initialize(ResolveTestAssetPath("gouraud.ps"), "main")) <<
			ps->GetLastError();
		
		//Create VertexBuffer
		Model model;
		CHECK(model.Initialize(ResolveTestAssetPath("model/bunny.obj"))) << model.GetLastError();
		{
			RTTTestVertex *vertices = 0;
			
	 		uint32_t size = model.GetVertexSize();
			vertices = new RTTTestVertex[size];
			for(uint32_t i=0; i<size; i++) {
				Model::Vertex v = model.GetVertex(i);
				vertices[i].position[0] = v.x;
				vertices[i].position[1] = v.y;
				vertices[i].position[2] = v.z;

				vertices[i].normal[0] = v.nx;
				vertices[i].normal[1] = v.ny;
				vertices[i].normal[2] = v.nz;
			}

			vb = manager->CreateBuffer();
			Buffer::Option option;
			option.Initialize(size, vertices);
			vb->Initialize(option);
			delete[] vertices;
		}
		
		{
			//Create IndexBuffer
			uint32_t size = model.GetTriangleSize()*3;
			Buffer::IndexBufferElementType*indices = new Buffer::IndexBufferElementType[size];
			for(uint32_t i=0; i<model.GetTriangleSize(); i++) {
				indices[i*3] = model.GetTriangleVertexIndex(i, 0);
				indices[i*3+1] = model.GetTriangleVertexIndex(i, 1);
				indices[i*3+2] = model.GetTriangleVertexIndex(i, 2);
			}
			ib = manager->CreateBuffer();
			Buffer::Option option;
			option.InitializeAsIndexBuffer(size, indices);
			option.resource_write = RendererEnum::IMMUTABLE;
			ib->Initialize(option);
			delete[] indices;
		}
	}
	
	void CreateTextureProgram() {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		const RendererSetting &renderer_setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();

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
		tex_ps->SetSampler("shader_sampler", sampler);
		tex_ps->SetShaderResource("shader_texture", texture->AsShaderResource());
		
		//Set vertex buffer
		RTTTestTextureVertex vertices[4] = {
			{{0.5f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}, 
			{{1.0f, 0.5f, 0.0f}, {1.0f, 1.0f}}, 
			{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}}
		};
		tex_vb = manager->CreateBuffer();
		Buffer::Option buffer_option;
		buffer_option.Initialize(4, vertices);
		tex_vb->Initialize(buffer_option);
		
		//Set index buffer
		Buffer::IndexBufferElementType indices[6] = {0,1,2, 0,2,3};
		tex_ib = manager->CreateBuffer();
		buffer_option.InitializeAsIndexBuffer(6, indices);
		buffer_option.resource_write = RendererEnum::IMMUTABLE;
		tex_ib->Initialize(buffer_option);
	}
	
	void DrawNormal(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();

		rotate += delta*PI / 2.0f;
		rotate = rotate>2 * PI ? rotate - 2 * PI : rotate;
		Matrix4x4 rotation_mat;
		rotation_mat.SetRotationY(rotate);
		vs->SetUniform("world", rotation_mat);
		vs->SetUniform("view", camera->GetViewMatrix());

		pipeline->Start();
			pipeline->SetPrimitiveTopology(GraphicPipeline::TRIANGLE_LIST);
			pipeline->SetVertexShader(vs);
			pipeline->SetPixelShader(ps);
			pipeline->SetDepthStencil(ds_buffer->AsDepthStencil());
			pipeline->SetRenderTarget(0, Engine::GetSingleton()->GetRendererContext()->GetBackBuffer()->AsRenderTarget());
			pipeline->SetRenderTarget(1, texture->AsRenderTarget());
			pipeline->SetVertexBuffer(0, 0, vb->AsVertexBuffer());
			pipeline->SetIndexBuffer(ib->AsIndexBuffer());
			pipeline->Draw(&normal_draw_state);
		pipeline->End();
	}
	
	void DrawTexture(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();

		pipeline->Start();
			pipeline->SetPrimitiveTopology(GraphicPipeline::TRIANGLE_LIST);
			pipeline->SetRenderTarget(1, 0);
			pipeline->SetDepthStencil(ds_buffer->AsDepthStencil());
			pipeline->SetVertexShader(tex_vs);
			pipeline->SetPixelShader(tex_ps);
			pipeline->SetRenderTarget(0, Engine::GetSingleton()->GetRendererContext()->GetBackBuffer()->AsRenderTarget());
			pipeline->SetVertexBuffer(0, 0, tex_vb->AsVertexBuffer());
			pipeline->SetIndexBuffer(tex_ib->AsIndexBuffer());
			pipeline->Draw(&rtt_draw_state);
		pipeline->End();
	}
	
	virtual void OneFrame(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		pipeline->ClearDepthStencil(ds_buffer->AsDepthStencil(), true, 1.0f, true, 0);
		float red[4] = {1.0f, 0.0f, 0.0f, 1.0f};
		pipeline->ClearRenderTarget(texture->AsRenderTarget(), red);
		DrawNormal(delta);
		DrawTexture(delta);
	}
	
private:
	Texture2D *ds_buffer;
	Texture2D *texture;
	
	Buffer *vb;
	Buffer *ib;
	VertexShader *vs;
	PixelShader *ps;
	float rotate;
	
	Buffer *tex_vb;
	Buffer *tex_ib;
	VertexShader *tex_vs;
	PixelShader *tex_ps;
	Sampler *sampler;

	DrawingState *normal_draw_state, *rtt_draw_state;
	
	Camera *camera;
};

AddBeforeMain(RenderToTargetDemo)

}

