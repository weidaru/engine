#include "engine_program.h"
#include "engine.h"

#include "graphics/renderer/all.h"
#include "asset/mesh.h"
#include "asset/asset_path.h"
#include "scene/camera.h"
#include "input/input_system.h"
#include "entity/transform.h"

#include <glog/logging.h>
#include <stdlib.h>

#include "instancing_demo.h"

#define PI 3.141596253f

namespace s2 {


class InstancingDemo : public EngineProgram {
public:
	InstancingDemo()
		:	position_buffer(0), color_buffer(0), instance_buffer(0), 
			index_buffer(0), vs(0), ps(0), ds_buffer(0), camera(0) {
		
	}
	
	virtual ~InstancingDemo() { }
	
	virtual bool Initialize() {
		camera = new Camera(Engine::GetSingleton()->GetEntitySystem());
		camera->GetTransform()->Translate(Vector3(20.0f, 20.0f, -75.0f));

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
			vs->SetUniform("view", camera->GetViewMatrix());

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
			Mesh mesh;
			CHECK(mesh.Initialize(ResolveTestAssetPath("model/cube.obj"))) << mesh.GetLastError();
			
			{
				InstancingTestPosition *positions = 0;
				InstancingTestColor *colors = 0;
			
	 			uint32_t size = mesh.GetVertexSize();
				positions = new InstancingTestPosition[size];
				colors = new InstancingTestColor[size];
				for(uint32_t i=0; i<size; i++) {
					Vector3 p = mesh.GetPosition(i);
					positions[i].data[0] = p[0];
					positions[i].data[1] = p[1];
					positions[i].data[2] = p[2];
				
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
				uint32_t size = mesh.GetTriangleSize()*3;
				Buffer::IndexBufferElementType *indices = new Buffer::IndexBufferElementType[size];
				for(uint32_t i=0; i<mesh.GetTriangleSize(); i++) {
					indices[i*3] = mesh.GetTriangleVertexIndex(i, 0);
					indices[i*3+1] = mesh.GetTriangleVertexIndex(i, 1);
					indices[i*3+2] = mesh.GetTriangleVertexIndex(i, 2);
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
			for(uint32_t i=0; i<10; i++) {
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
	
	virtual void OneFrame(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();

		pipeline->ClearDepthStencil(ds_buffer->AsDepthStencil(), true, 1.0f, true, 0);
		vs->SetUniform("view", camera->GetViewMatrix());

		pipeline->Start();
			pipeline->SetPrimitiveTopology(GraphicPipeline::TRIANGLE_LIST);
			pipeline->SetDepthStencil(ds_buffer->AsDepthStencil());
			pipeline->SetRenderTarget(0, Engine::GetSingleton()->GetRendererContext()->GetBackBuffer()->AsRenderTarget());
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

	Camera *camera;
};

AddBeforeMain(InstancingDemo)


}
