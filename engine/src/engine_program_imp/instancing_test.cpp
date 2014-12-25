#include "engine_program.h"
#include "engine.h"

#include "renderer/all.h"

#include "asset/model.h"

#include "scene/camera.h"

#include <glog/logging.h>
#inlcude <stdlib.h>

//[[TypeInfo]]//
struct InstancingTestPosition {
	float data[3];
};

//[[TypeInfo]]//
struct InstancingTestColor {
	float data[4];
};

//[[TypeInfo]]//
struct InstancingTestInstancePosition {
	float data[3];
};


namespace s2 {


class InstancingTest : public EngineProgram {
public:
	InstancingTest()
		: vertex_buffer(0), instance_buffer(0), vs(0), ps(0), ds_buffer(0) {
		
	}
	
	virtual ~InstancingTest() { }
	
	virtual bool Initialize() {
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		
		const RendererSetting &renderer_setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
		Texture2D::Option ds_option;
		Texture2D::Option::SetAsDepthStencilBuffer(&ds_option, renderer_setting.window_width, renderer_setting.window_height);
		ds_buffer = manager->CreateTexture2D();
		ds_buffer->Initialize(ds_option);
		
		vs = manager->CreateVertexShader();
		CHECK(vs->Initialize("C:\\Users\\zhiwshen\\Documents\\GitHub\\engine\\engine\\test\\instancing.vs", "main"))
				<<vs->GetLastError();
		
		ps = manager->CreatePixelShader();
		CHECK(ps->Initialize("C:\\Users\\zhiwshen\\Documents\\GitHub\\engine\\engine\\test\\instancing.ps", "main"))
				<<ps->GetLastError();
		
		position_buffer = manager->CreateVertexBuffer();
		color_buffer = manager->CreateVertexBuffer();
		{
			Model model;
			CHECK(model.Initialize("C:\\Users\\zhiwshen\\Documents\\GitHub\\engine\\engine\\test\\model\\cube.obj")) << model.GetLastError();
			InstancingTestPosition *positions = 0;
			InstancingTestColor *colors = 0;
			
	 		unsigned int size = model.GetVertexSize();
			positions = new InstancingTestPosition[size];
			colors = new InstancingTestColor[size];
			for(unsigned int i=0; i<size; i++) {
				Model::Vertex v = model.GetVertex(i);
				positions[i].data[0] = v.x;
				positions[i].data[1] = v.y;
				positions[i].data[2] = v.z;
				
				colors[i].data[0] = (float(rand()%100))/100.0f;
				colors[i].data[1] = (float(rand()%100))/100.0f;
				colors[i].data[2] = (float(rand()%100))/100.0f;	
			}

			position_buffer->Initialize(size, positions, GeneralEnum::MAP_FORBIDDEN);
			color_buffer->Initialize(size, colors, GeneralEnum::MAP_FORBIDDEN);
			delete[] positions;
			delete[] colors;
		}
		
		instance_buffer = manager->CreateVertexBuffer();
		{
			InstancingTestInstancePosition positions[10];
			for(unsigned int i=0; i<10; i++) {
				positions[i].data[0] = 5*i;
				positions[i].data[1] = 5*i;
				positions[i].data[2] = 5*i;
			}
			instance_buffer->Initialize(10, positions, GeneralEnum::MAP_FORBIDDEN);
		}	
		
		camera.SetPosition(25.0f, 25.0f, 75.0f);
		
		return true;
	}
	
	virtual const s2string & GetName() {
		static s2string name="instancing_test";
		return name;
	}
	
	virtual void OneFrame(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		
		pipeline->ClearRenderTarget();
		pipeline->SetDepthStencilBuffer(ds_buffer);
		pipeline->SetRenderTarget(0, manager->GetBackBuffer());
		pipeline->SetVertexShader(vs);
		pipeline->SetPixelShader(ps);
		pipeline->SetVertexBuffer(2, 0, position_buffer);
		pipeline->SetVertexBuffer(0, 1, color_buffer);
		pipeline->SetVertexBuffer(1, 2, instance_buffer);
		
		float black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		pipeline->ClearRenderTarget(manager->GetBackBuffer(), black);
		pipeline->ClearDepthStencilBuffer(ds_buffer, true, 1.0f, true, 0);

		pipeline->Draw();
	}
	
private:
	VertexBuffer *position_buffer, *color_buffer;
	VertexBuffer *instance_buffer;
	VertexShader *vs;
	PixelShader *ps;
	Texture2D *ds_buffer;
};

AddBeforeMain(InstancingTest)


}
