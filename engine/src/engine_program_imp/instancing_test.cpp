#include "engine_program.h"
#include "engine.h"

#include "renderer/all.h"
#include "asset/model.h"
#include "scene/camera.h"
#include "input_system.h"

#include <glog/logging.h>
#include <stdlib.h>

#define PI 3.141596253f

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
		: position_buffer(0), color_buffer(0), instance_buffer(0), index_buffer(0), vs(0), ps(0), ds_buffer(0) {
		
	}
	
	virtual ~InstancingTest() { }
	
	virtual bool Initialize() {
		camera.SetPosition(Vector3(20.0f, 20.0f, 75.0f));

		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		
		const RendererSetting &renderer_setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
		Texture2D::Option ds_option;
		Texture2D::Option::SetAsDepthStencilBuffer(&ds_option, renderer_setting.window_width, renderer_setting.window_height);
		ds_buffer = manager->CreateTexture2D();
		ds_buffer->Initialize(ds_option);
		
		vs = manager->CreateVertexShader();
		CHECK(vs->Initialize("C:\\Users\\zhiwshen\\Documents\\GitHub\\engine\\engine\\test\\instancing.vs", "main"))
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
		CHECK(ps->Initialize("C:\\Users\\zhiwshen\\Documents\\GitHub\\engine\\engine\\test\\instancing.ps", "main"))
				<<ps->GetLastError();
		
		position_buffer = manager->CreateVertexBuffer();
		color_buffer = manager->CreateVertexBuffer();
		index_buffer = manager->CreateIndexBuffer();
		{
			Model model;
			CHECK(model.Initialize("C:\\Users\\zhiwshen\\Documents\\GitHub\\engine\\engine\\test\\model\\cube.obj")) << model.GetLastError();
			
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

				position_buffer->Initialize(size, positions, RendererEnum::IMMUTABLE);
				color_buffer->Initialize(size, colors, RendererEnum::IMMUTABLE);
				delete[] positions;
				delete[] colors;
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
				index_buffer->Initialize(size, indices, RendererEnum::IMMUTABLE);
				delete[] indices;
			}
		}
		
		instance_buffer = manager->CreateVertexBuffer();
		{
			InstancingTestInstancePosition positions[10];
			for(unsigned int i=0; i<10; i++) {
				positions[i].data[0] = (float)5*i;
				positions[i].data[1] = (float)5*i;
				positions[i].data[2] = (float)5*i;
			}
			instance_buffer->Initialize(10, positions, RendererEnum::IMMUTABLE);
		}	
		
		
		
		return true;
	}
	
	virtual const s2string & GetName() {
		static s2string name="instancing_test";
		return name;
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

		pipeline->SetDepthStencilBuffer(ds_buffer);
		pipeline->SetRenderTarget(0, manager->GetBackBuffer());
		pipeline->SetVertexShader(vs);
		pipeline->SetPixelShader(ps);
		pipeline->SetVertexBuffer(2, 0, position_buffer);
		pipeline->SetVertexBuffer(0, 1, color_buffer);
		pipeline->SetVertexBuffer(1, 2, instance_buffer);
		pipeline->SetIndexBuffer(index_buffer);
		
		float black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		pipeline->ClearRenderTarget(manager->GetBackBuffer(), black);
		pipeline->ClearDepthStencilBuffer(ds_buffer, true, 1.0f, true, 0);

		vs->SetUniform("view", camera.GetViewMatrix());

		pipeline->Draw();
	}
	
private:
	VertexBuffer *position_buffer, *color_buffer;
	VertexBuffer *instance_buffer;
	IndexBuffer *index_buffer;
	VertexShader *vs;
	PixelShader *ps;
	Texture2D *ds_buffer;

	Camera camera;
};

AddBeforeMain(InstancingTest)


}
