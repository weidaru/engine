#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")


#define WIN32_LEAN_AND_MEAN
#include <dxgi.h>
#include <d3d11.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#undef ERROR

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

#include <stdio.h>
#include <math.h>

#define PI 3.141596253f

//[[TypeInfo]]//
struct Vertex {
	float position[3];
	float color[4];
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
	TestProgram():ds_buffer(0), vb(0), ib(0), vs(0), ps(0), rotate(0.0f) {}

	virtual ~TestProgram() {}
	virtual bool Initialize(){
		printf("Initialize test program.\n");
		
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		
		//Back buffer as render target 
		pipeline->SetRenderTarget(0, manager->GetBackBuffer());
		
		//Create and set depth stencil buffer
		const RendererSetting &renderer_setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
		Texture2D::Option ds_option;
		Texture2D::Option::SetAsDepthStencilBuffer(&ds_option, renderer_setting.window_width, renderer_setting.window_height);
		Texture2D *ds_buffer = manager->CreateTexture2D();
		ds_buffer->Initialize(ds_option);
		pipeline->SetDepthStencilBuffer(ds_buffer);
		
		//Set clean  up
		float background[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		pipeline->SetRenderTargetClearOption(0, true, background);
		
		//Set vertex shader
		vs = manager->CreateVertexShader();
		pipeline->SetVertexShader(vs);
		CHECK(vs->Initialize("D:\\github_repository\\engine\\engine\\test\\color.vs", "ColorVertexShader")) <<
			vs->GetLastError();
			//Set world view projection
		{
			Matrix rotation_mat;
			MakeRotationAxisY(&rotation_mat, rotate);
			vs->SetUniform("world", rotation_mat);

			Matrix camera;
			camera[3][2] = 5.0;
			vs->SetUniform("view", camera);
			
			float np=0.0f, fp =1000.0f;
			float aspect=((float)renderer_setting.window_width)/((float)renderer_setting.window_height);
			float fov=PI*35/180;
			float yscale = 1.0f/tan(fov/2);
			
			//Column major matrix
			Matrix projection;
			projection[0][0] = yscale/aspect;
			projection[1][1] = yscale;
			projection[2][2] = fp/(fp-np);
			projection[2][3] = 1.0f;
			projection[3][2] = -np*fp/(fp-np);
			projection[3][3] = 0.0f;

			vs->SetUniform("projection", projection);
		}

		//Set pixel shader
		ps = manager->CreatePixelShader();
		pipeline->SetPixelShader(ps);
		CHECK(ps->Initialize("D:\\github_repository\\engine\\engine\\test\\color.ps", "ColorPixelShader")) <<
			ps->GetLastError();
		
		//Set vertex buffer
		Vertex vertices[3] = {
			{{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
			{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}}, 
			{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}
		};
		vb = manager->CreateVertexBuffer();
		vb->Initialize(3, (Vertex *)0, GeneralEnum::MAP_WRITE_OCCASIONAL);
		vb->Update(0, vertices, 3);
		pipeline->SetVertexBuffer(0, 0, vb);
		

		//Set index buffer
		IndexBuffer::InputType indices[3] = {0,1,2};
		ib = manager->CreateIndexBuffer();
		ib->Initialize(3, indices, GeneralEnum::MAP_FORBIDDEN);
		pipeline->SetIndexBuffer(ib);

		return true;
	}
	
	virtual const s2string & GetName() {
		static s2string name="test";
		return name;
	}
	
	virtual void OneFrame(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		
		rotate += delta*PI/2.0f;
		rotate = rotate>2*PI ? rotate-2*PI : rotate;
		Matrix rotation_mat;
		MakeRotationAxisY(&rotation_mat, rotate);
		vs->SetUniform("world", rotation_mat);

		pipeline->Draw();
	}
	
private:
	Texture2D *ds_buffer;
	VertexBuffer *vb;
	IndexBuffer *ib;
	VertexShader *vs;
	PixelShader *ps;
	
	float rotate;
};

AddBeforeMain(TestProgram)

}

