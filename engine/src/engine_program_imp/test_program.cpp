#include <glog/logging.h>

#include "engine_program.h"
#include "engine.h"

#include "renderer/rasterization_option.h"
#include "renderer/graphic_pipeline.h"
#include "renderer/graphic_resource_manager.h"
#include "renderer/texture2d.h"
#include "renderer/vertex_shader.h"
#include "renderer/pixel_shader.h"
#include "renderer/vertex_buffer.h"
#include "renderer/index_buffer.h"
#include "renderer/texture_enum.h"
#include "renderer/sampler.h"

#include "asset/model.h"

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

//[[TypeInfo]]//
struct Matrix {
	//[[CoreData]]//
	float data[4][4];

	Matrix() {
		memset(data, 0, 64);
		data[0][0] = 1.0f;
		data[1][1] = 1.0f;
		data[2][2] = 1.0f;
		data[3][3] = 1.0f;
	}
	
	float * operator[](unsigned int index) {
		return data[index];
	}
	
	const float * operator[](unsigned int index) const {
		return data[index];
	}
};

static void MakeIdentity(Matrix *_m) {
	Matrix &m = *_m;
	memset(_m, 0, 64);
	m[0][0] = 1.0f;
	m[1][1] = 1.0f;
	m[2][2] = 1.0f;
	m[3][3] = 1.0f;
}

static void MakeRotationAxisY(Matrix *_m, float theta) {
	Matrix &m = *_m;
	m[0][0]=cos(theta); 		m[0][1]=0.0f;		m[0][2]=-sin(theta);		m[0][3]=0.0f;
	m[1][0]=0;			 		m[1][1]=1.0f;		m[1][2]=0.0f;				m[1][3]=0.0f;
	m[2][0]=sin(theta); 		m[2][1]=0.0f;		m[2][2]=cos(theta);		m[2][3]=0.0f;
	m[3][0]=0.0f;		 		m[3][1]=0.0f;		m[3][2]=0.0f;				m[3][3]=1.0f;
}

namespace s2 {

class TestProgram : public EngineProgram {
public:
	TestProgram():
		ds_buffer(0), rtt_texture(0),
		vb(0), ib(0), vs(0), ps(0), rotate(0.0f),
		tex_vb(0), tex_ib(0), tex_vs(0), tex_ps(0), sampler(0){}

	virtual ~TestProgram() {}
	virtual bool Initialize(){
		printf("Initialize test program.\n");
		//Create and set depth stencil buffer
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		
		//Set depth buffer
		const RendererSetting &renderer_setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
		Texture2D::Option ds_option;
		Texture2D::Option::SetAsDepthStencilBuffer(&ds_option, renderer_setting.window_width, renderer_setting.window_height);
		ds_buffer = manager->CreateTexture2D();
		ds_buffer->Initialize(ds_option);
		pipeline->SetDepthStencilBuffer(ds_buffer);
		
		CreateColorProgram();
		CreateTextureProgram();
		
		return true;
	}
	
	virtual const s2string & GetName() {
		static s2string name="test";
		return name;
	}
	
	void CreateColorProgram() {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		const RendererSetting &renderer_setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
		
		//Create texture we render to.
		Texture2D::Option tex_option;
		rtt_texture = manager->CreateTexture2D();
		tex_option.width = renderer_setting.window_width;
		tex_option.height = renderer_setting.window_height;
		tex_option.output_bind = TextureEnum::RENDER_TARGET;
		tex_option.format = TextureEnum::R32G32B32A32_FLOAT;
		rtt_texture->Initialize(tex_option);
		
		//Create vertex shader
		vs = manager->CreateVertexShader();
		CHECK(vs->Initialize("C:\\Users\\zhiwshen\\Documents\\GitHub\\engine\\engine\\test\\gouraud.vs", "main")) <<
			vs->GetLastError();
		{
			Matrix rotation_mat;
			MakeRotationAxisY(&rotation_mat, rotate);
			vs->SetUniform("world", rotation_mat);

			Matrix camera;
			camera[3][0] = 0.0f;
			camera[3][1] = -1.5f;
			camera[3][2] = -10.0f;
			vs->SetUniform("view", camera);
			
			float np=-0.5f, fp =-1000.0f;
			float aspect=((float)renderer_setting.window_width)/((float)renderer_setting.window_height);
			float fov=PI*35/180;
			float yscale = 1.0f/tan(fov/2);
			
			Matrix projection;
			
			projection[0][0] = yscale/aspect;
			projection[1][1] = yscale;
			projection[2][2] = fp/(np-fp);
			projection[2][3] = -1.0f;
			projection[3][2] = -np*fp/(np-fp);
			projection[3][3] = 0.0f;

			vs->SetUniform("projection", projection);
		}

		//Create PixelShader;
		ps = manager->CreatePixelShader();
		CHECK(ps->Initialize("C:\\Users\\zhiwshen\\Documents\\GitHub\\engine\\engine\\test\\gouraud.ps", "main")) <<
			ps->GetLastError();
		
		//Create VertexBuffer
		Model model;
		CHECK(model.Initialize("C:\\Users\\zhiwshen\\Documents\\GitHub\\engine\\engine\\test\\model\\bunny.obj")) << model.GetLastError();
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

			vb = manager->CreateVertexBuffer();
			vb->Initialize(size, vertices, GeneralEnum::MAP_WRITE_OCCASIONAL);
			delete[] vertices;
		}
		
		{
			//Create IndexBuffer
			unsigned int size = model.GetTriangleSize()*3;
			IndexBuffer::InputType *indices = new IndexBuffer::InputType[size];
			for(unsigned int i=0; i<model.GetTriangleSize(); i++) {
				indices[i*3] = model.GetTriangleVertexIndex(i, 0);
				indices[i*3+1] = model.GetTriangleVertexIndex(i, 1);
				indices[i*3+2] = model.GetTriangleVertexIndex(i, 2);
			}
			ib = manager->CreateIndexBuffer();
			ib->Initialize(size, indices, GeneralEnum::MAP_FORBIDDEN);
			delete[] indices;
		}
	}
	
	void CreateTextureProgram() {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		
		
		//Create sampler
		sampler = manager->CreateSampler();
		Sampler::Option option;
		sampler->Initialize(option);

		//Set vertex shader
		tex_vs = manager->CreateVertexShader();
		CHECK(tex_vs->Initialize("C:\\Users\\zhiwshen\\Documents\\GitHub\\engine\\engine\\test\\texture.vs", "main")) <<
			tex_vs->GetLastError();

		//Set pixel shader
		tex_ps = manager->CreatePixelShader();
		CHECK(tex_ps->Initialize("C:\\Users\\zhiwshen\\Documents\\GitHub\\engine\\engine\\test\\texture.ps", "main")) <<
			tex_ps->GetLastError();
		tex_ps->SetSampler("shader_sampler", sampler);
		tex_ps->SetTexture2D("shader_texture", rtt_texture);
		
		//Set vertex buffer
		TextureVertex vertices[4] = {
			{{0.5f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}, 
			{{1.0f, 0.5f, 0.0f}, {1.0f, 1.0f}}, 
			{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}}
		};
		tex_vb = manager->CreateVertexBuffer();
		tex_vb->Initialize(4, vertices, GeneralEnum::MAP_WRITE_OCCASIONAL);
		
		//Set index buffer
		IndexBuffer::InputType indices[6] = {0,1,2, 0,2,3};
		tex_ib = manager->CreateIndexBuffer();
		tex_ib->Initialize(6, indices, GeneralEnum::MAP_FORBIDDEN);
	}
	
	void DrawNormal(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();

		pipeline->ResetRenderTargets();
		pipeline->SetRenderTarget(0, manager->GetBackBuffer());
		pipeline->SetRenderTarget(1, rtt_texture);
		pipeline->SetVertexShader(vs);
		pipeline->SetPixelShader(ps);
		pipeline->SetVertexBuffer(0, 0, vb);
		pipeline->SetIndexBuffer(ib);
		
		rotate += delta*PI/2.0f;
		rotate = rotate>2*PI ? rotate-2*PI : rotate;
		Matrix rotation_mat;
		MakeRotationAxisY(&rotation_mat, rotate);
		vs->SetUniform("world", rotation_mat);

		pipeline->Draw();
	}
	
	void DrawTexture(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		pipeline->ResetRenderTargets();
		pipeline->SetRenderTarget(0, manager->GetBackBuffer());
		pipeline->SetVertexShader(tex_vs);
		pipeline->SetPixelShader(tex_ps);
		pipeline->SetVertexBuffer(0, 0, tex_vb);
		pipeline->SetIndexBuffer(tex_ib);

		pipeline->Draw();
	}
	
	virtual void OneFrame(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		float black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		pipeline->ClearRenderTarget(manager->GetBackBuffer(), black);
		pipeline->ClearDepthStencilBuffer(ds_buffer, true, 1.0f, true, 0);
		float red[4] = {1.0f, 0.0f, 0.0f, 1.0f};
		pipeline->ClearRenderTarget(rtt_texture, red);
		DrawNormal(delta);
		
		DrawTexture(delta);
	}
	
private:
	Texture2D *ds_buffer;
	Texture2D *rtt_texture;
	
	VertexBuffer *vb;
	IndexBuffer *ib;
	VertexShader *vs;
	PixelShader *ps;
	float rotate;
	
	VertexBuffer *tex_vb;
	IndexBuffer *tex_ib;
	VertexShader *tex_vs;
	PixelShader *tex_ps;
	Sampler *sampler;
};

AddBeforeMain(TestProgram)

}

