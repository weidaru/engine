#include <glog/logging.h>

#include "engine_program.h"
#include "engine.h"

#include "renderer/all.h"

#include "asset/model.h"
#include "asset/pixel_map.h"
#include "asset/asset_path.h"

#include "scene/camera.h"
#include "input_system.h"

#include <stdio.h>
#include <math.h>

#define PI 3.141596253f

//[[TypeInfo]]//
struct Vertex {
	float position[3];
	float normal[3];
};

//[[TypeInfo]]//
struct TextureVertex {
	float position[3];
	float tex[2];
};

namespace s2 {

class RenderToTargetDemo : public EngineProgram {
public:
	RenderToTargetDemo() :
		ds_buffer(0), texture(0),
		vb(0), ib(0), vs(0), ps(0), rotate(0.0f),
		tex_vb(0), tex_ib(0), tex_vs(0), tex_ps(0), sampler(0){}

	virtual ~RenderToTargetDemo() {}
	virtual bool Initialize(){
		printf("Initialize test program.\n");
		//Create and set depth stencil buffer
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		
		//Set depth buffer
		const RendererSetting &renderer_setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
		Texture2D::Option ds_option;
		Texture2D::Option::SetAsDepthStencilBuffer(&ds_option, renderer_setting.window_width, renderer_setting.window_height);
		ds_buffer = manager->CreateTexture2D();
		ds_buffer->Initialize(ds_option);
		
		camera.SetPosition(Vector3(0.0, 1.0, 40.0f));
		camera.SetForwardVector(Vector3(0.0f, 0.0f, -1.0f));
		
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
		CHECK(vs->Initialize(ResolveAssetPath("gouraud.vs"), "main")) <<
			vs->GetLastError();
		{
			Matrix4x4 identity;
			vs->SetUniform("world", identity);
			vs->SetUniform("view", camera.GetViewMatrix());
			
			float np=0.5f, fp =1000.0f;
			float aspect=((float)renderer_setting.window_width)/((float)renderer_setting.window_height);
			float fov=PI*35/180;
			Matrix4x4 projection;
			projection.SetProjection(aspect, fov, np, fp);

			vs->SetUniform("projection", projection);
		}

		//Create PixelShader;
		ps = manager->CreatePixelShader();
		CHECK(ps->Initialize(ResolveAssetPath("gouraud.ps"), "main")) <<
			ps->GetLastError();
		
		//Create VertexBuffer
		Model model;
		CHECK(model.Initialize(ResolveAssetPath("model/bunny.obj"))) << model.GetLastError();
		{
			Vertex *vertices = 0;
			
	 		unsigned int size = model.GetVertexSize();
			vertices = new Vertex[size];
			for(unsigned int i=0; i<size; i++) {
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
			unsigned int size = model.GetTriangleSize()*3;
			Buffer::IndexBufferElementType*indices = new Buffer::IndexBufferElementType[size];
			for(unsigned int i=0; i<model.GetTriangleSize(); i++) {
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
		CHECK(tex_vs->Initialize(ResolveAssetPath("texture.vs"), "main")) <<
			tex_vs->GetLastError();

		//Set pixel shader
		tex_ps = manager->CreatePixelShader();
		CHECK(tex_ps->Initialize(ResolveAssetPath("texture.ps"), "main")) <<
			tex_ps->GetLastError();
		tex_ps->SetSampler("shader_sampler", sampler);
		tex_ps->SetShaderResource("shader_texture", texture->AsShaderResource());
		
		//Set vertex buffer
		TextureVertex vertices[4] = {
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
		Buffer::IndexBufferElementType indices[6] = {0,2,1, 0,3,2};
		tex_ib = manager->CreateBuffer();
		buffer_option.InitializeAsIndexBuffer(6, indices);
		buffer_option.resource_write = RendererEnum::IMMUTABLE;
		tex_ib->Initialize(buffer_option);
	}
	
	void DrawNormal(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();

		rotate += delta*PI / 2.0f;
		rotate = rotate>2 * PI ? rotate - 2 * PI : rotate;
		Matrix4x4 rotation_mat;
		rotation_mat.SetRotationY(rotate);
		vs->SetUniform("world", rotation_mat);
		vs->SetUniform("view", camera.GetViewMatrix());

		pipeline->Start();
			pipeline->SetDepthStencil(ds_buffer->AsDepthStencil());
			pipeline->SetRenderTarget(0, manager->GetBackBuffer()->AsRenderTarget());
			pipeline->SetRenderTarget(1, texture->AsRenderTarget());
			pipeline->SetVertexShader(vs);
			pipeline->SetPixelShader(ps);
			pipeline->SetVertexBuffer(0, 0, vb->AsVertexBuffer());
			pipeline->SetIndexBuffer(ib->AsIndexBuffer());
			pipeline->ResolveConflict();
			pipeline->Draw();
		pipeline->End();
	}
	
	void DrawTexture(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();

		pipeline->Start();
			pipeline->SetDepthStencil(ds_buffer->AsDepthStencil());
			pipeline->SetRenderTarget(0, manager->GetBackBuffer()->AsRenderTarget());
			pipeline->SetVertexShader(tex_vs);
			pipeline->SetPixelShader(tex_ps);
			pipeline->SetVertexBuffer(0, 0, tex_vb->AsVertexBuffer());
			pipeline->SetIndexBuffer(tex_ib->AsIndexBuffer());
			pipeline->ResolveConflict();
			pipeline->Draw();
		pipeline->End();
	}
	
	void UpdateCamera(float delta) {
		InputSystem &is = *Engine::GetSingleton()->GetInputSystem();
		int delta_x = is.GetMouseXMove();
		int delta_y = is.GetMouseYMove();

		if(delta_x != 0) {
			camera.TurnRight(delta_x*PI/1000.0f);
		}
		if (delta_y != 0) {
			camera.TurnUp(-delta_y*PI/1000.0f);
		}
		is.SetMousePositionPercent(0.5f, 0.5f);

		if(is.IsKeyDown(InputSystem::K_W)) {
			camera.MoveForward(20.0f*delta);
		} else if(is.IsKeyDown(InputSystem::K_S)) {
			camera.MoveForward(-20.0f*delta);
		}
		if(is.IsKeyDown(InputSystem::K_D)) {
			camera.MoveRight(20.0f*delta);
		} else if(is.IsKeyDown(InputSystem::K_A)) {
			camera.MoveRight(-20.0f*delta);
		}
	}
	
	virtual void OneFrame(float delta) {
		UpdateCamera(delta);
	
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		float black[4] = {0.0f, 0.0f, .0f, 1.0f};
		pipeline->ClearRenderTarget(manager->GetBackBuffer()->AsRenderTarget(), black);
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
	
	Camera camera;
};

AddBeforeMain(RenderToTargetDemo)

}
