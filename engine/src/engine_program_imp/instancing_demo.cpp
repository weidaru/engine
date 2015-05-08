#include "engine_program.h"
#include "engine.h"

#include "graphics/renderer/all.h"
#include "asset/model.h"
#include "asset/asset_path.h"
#include "scene/camera.h"
#include "input/input_system.h"

#include <glog/logging.h>
#include <stdlib.h>

#include "instancing_demo.h"

#define PI 3.141596253f

namespace s2 {


class InstancingDemo : public EngineProgram {
public:
	InstancingDemo()
		: position_buffer(0), color_buffer(0), instance_buffer(0), index_buffer(0), vs(0), ps(0), ds_buffer(0) {
		
	}
	
	virtual ~InstancingDemo() { }
	
	virtual bool Initialize() {
		camera.SetPosition(Vector3(20.0f, 20.0f, 75.0f));

		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		
		const RendererSetting &renderer_setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
		Texture2D::Option ds_option;
		Texture2D::Option::SetAsDepthStencilBuffer(&ds_option, renderer_setting.window_width, renderer_setting.window_height);
		ds_buffer = manager->CreateTexture2D();
		ds_buffer->Initialize(ds_option);
		
		vs = manager->CreateVertexShader();
		CHECK(vs->Initialize(ResolveTestAssetPath("instancing.vs"), "main"))
				<<vs->GetLastError();
		
		{
			vs->SetUniform("world", Matrix4x4());
			vs->SetUniform("view", camera.GetViewMatrix());

			float np=0.5f, fp =1000.0f;
			float aspect=((float)renderer_setting.window_width)/((float)renderer_setting.window_height);
			float fov=PI*35/180;
			Matrix4x4 projection;
			projection.SetProjection(aspect, fov, np, fp);
			vs->SetUniform("projection", projection);
		}
		
		
		ps = manager->CreatePixelShader();
		CHECK(ps->Initialize(ResolveTestAssetPath("instancing.ps"), "main"))
				<<ps->GetLastError();
		
		position_buffer = manager->CreateBuffer();
		color_buffer = manager->CreateBuffer();
		index_buffer = manager->CreateBuffer();
		{
			Model model;
			CHECK(model.Initialize(ResolveTestAssetPath("model/cube.obj"))) << model.GetLastError();
			
			{
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
				Buffer::Option option;
				option.Initialize(size, positions);
				option.resource_write = RendererEnum::IMMUTABLE;

				position_buffer->Initialize(option);
				option.Initialize(size, colors);
				option.resource_write = RendererEnum::IMMUTABLE;
				color_buffer->Initialize(option);
				delete[] positions;
				delete[] colors;
			}


			{
				//Create IndexBuffer
				unsigned int size = model.GetTriangleSize()*3;
				Buffer::IndexBufferElementType *indices = new Buffer::IndexBufferElementType[size];
				for(unsigned int i=0; i<model.GetTriangleSize(); i++) {
					indices[i*3] = model.GetTriangleVertexIndex(i, 0);
					indices[i*3+1] = model.GetTriangleVertexIndex(i, 1);
					indices[i*3+2] = model.GetTriangleVertexIndex(i, 2);
				}
				Buffer::Option option;
				option.InitializeAsIndexBuffer(size, indices);
				option.resource_write = RendererEnum::IMMUTABLE;
				index_buffer->Initialize(option);
				delete[] indices;
			}
		}
		
		instance_buffer = manager->CreateBuffer();
		{
			InstancingTestInstancePosition positions[10];
			for(unsigned int i=0; i<10; i++) {
				positions[i].data[0] = (float)5*i;
				positions[i].data[1] = (float)5*i;
				positions[i].data[2] = (float)5*i;
			}
			Buffer::Option option;
			option.Initialize(10, positions);
			option.resource_write = RendererEnum::IMMUTABLE;
			instance_buffer->Initialize(option);
		}	

		return true;
	}
	
	virtual s2string GetName() const {
		return "InstancingDemo";
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

		pipeline->ClearDepthStencil(ds_buffer->AsDepthStencil(), true, 1.0f, true, 0);
		vs->SetUniform("view", camera.GetViewMatrix());

		pipeline->Start();
			pipeline->SetPrimitiveTopology(GraphicPipeline::TRIANGLE_LIST);
			pipeline->SetDepthStencil(ds_buffer->AsDepthStencil());
			pipeline->SetRenderTarget(0, manager->GetBackBuffer()->AsRenderTarget());
			pipeline->SetVertexShader(vs);
			pipeline->SetPixelShader(ps);
			pipeline->SetVertexBuffer(2, 0, position_buffer->AsVertexBuffer());
			pipeline->SetVertexBuffer(0, 1, color_buffer->AsVertexBuffer());
			pipeline->SetVertexBuffer(1, 2, instance_buffer->AsVertexBuffer());
			pipeline->SetIndexBuffer(index_buffer->AsIndexBuffer());
			pipeline->DrawInstance();
		pipeline->End();
	}
	
private:
	Buffer *position_buffer, *color_buffer;
	Buffer *instance_buffer;
	Buffer *index_buffer;
	VertexShader *vs;
	PixelShader *ps;
	Texture2D *ds_buffer;

	Camera camera;
};

//AddBeforeMain(InstancingDemo)


}
