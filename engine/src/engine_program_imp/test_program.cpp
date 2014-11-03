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

namespace s2 {

class TestProgram : public EngineProgram {
public:
	TestProgram():ds_buffer(0), vb_pos(0), vb_color(0), ib(0), vs(0), ps(0) {}

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
		float red[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		pipeline->SetRenderTargetClearOption(0, true, red);
		
		//Set vertex shader
		vs = manager->CreateVertexShader();
		CHECK(vs->Initialize("D:\\github_repository\\engine\\engine\\test\\color.vs", "ColorVertexShader")) <<
			vs->GetLastError();
		pipeline->SetVertexShader(vs);
		
		//Set pixel shader
		ps = manager->CreatePixelShader();
		CHECK(ps->Initialize("D:\\github_repository\\engine\\engine\\test\\color.ps", "ColorPixelShader")) <<
			ps->GetLastError();
		pipeline->SetPixelShader(ps);
		
		//Set vertex buffer
		float vertices[3][3] = {
			{0.0f, 0.5f, 0.0f},
			{0.45f, -0.5f, 0.0f}, 
			{-0.45f, -0.5f, 0.0f}
		};
		vb_pos = manager->CreateVertexBuffer();
		vb_pos->Initialize(3, 1, 12, vertices, false);
		pipeline->SetVertexBuffer(0, 0, vb_pos);
		float colors[3][4] = {
			{1.0f, 0.0f, 0.0f, 1.0f},
			{0.0f, 1.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f, 1.0f}
		};
		vb_color = manager->CreateVertexBuffer();
		vb_color->Initialize(3, 1, 16, colors, false);
		pipeline->SetVertexBuffer(1,1, vb_color);

		//Set index buffer
		IndexBuffer::InputType indices[3] = {0,1,2};
		ib = manager->CreateIndexBuffer();
		ib->Initialize(3, indices, false);
		pipeline->SetIndexBuffer(ib);

		return true;
	}
	
	virtual const s2string & GetName() {
		static s2string name="test";
		return name;
	}
	
	virtual void OneFrame(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		
		pipeline->Draw();
	}
	
private:
	Texture2D *ds_buffer;
	VertexBuffer *vb_pos;
	VertexBuffer *vb_color;
	IndexBuffer *ib;
	VertexShader *vs;
	PixelShader *ps;
};

AddBeforeMain(TestProgram)

}

