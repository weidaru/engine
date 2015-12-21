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
			index_buffer(0), vs(0), vs_data(0), ps(0), ds_buffer(0), camera(0), input_layout(0), state(0) {
		
	}
	
	virtual ~InstancingDemo() {
		delete state;
	}
	
	virtual bool Initialize() {
		camera = new Camera(Engine::GetSingleton()->GetEntitySystem());
		camera->GetTransform()->Translate(S2Vector3(20.0f, 20.0f, -75.0f));

		RendererContext *context = Engine::GetSingleton()->GetRendererContext();
		GraphicResourceManager *manager = context->GetResourceManager();
		const RendererSetting &renderer_setting = context->GetSetting();

		Texture2D::Option ds_option;
		Texture2D::Option::SetAsDepthStencilBuffer(&ds_option, renderer_setting.window_width, renderer_setting.window_height);
		ds_buffer = manager->CreateTexture2D();
		ds_buffer->Initialize(ds_option);
		
		vs = manager->CreateVertexShader();
		CHECK(vs->Initialize(ResolveTestAssetPath("instancing.vs"), "main"))
				<<vs->GetLastError();
		vs_data = manager->CreateShaderData();
		vs_data->Initialize(vs->GetShaderBytecode());
		{
			vs_data->SetUniform("world", S2Matrix4x4());
			vs_data->SetUniform("view", camera->GetViewMatrix());

			float np=0.5f, fp =1000.0f;
			float aspect=((float)renderer_setting.window_width)/((float)renderer_setting.window_height);
			float fov=PI*35/180;
			S2Matrix4x4 projection;
			projection.SetProjection(aspect, fov, np, fp);
			vs_data->SetUniform("projection", projection);
		}
		
		
		ps = manager->CreatePixelShader();
		CHECK(ps->Initialize(ResolveTestAssetPath("instancing.ps"), "main"))
				<<ps->GetLastError();
		
		position_buffer = manager->CreateGraphicBuffer();
		color_buffer = manager->CreateGraphicBuffer();
		index_buffer = manager->CreateGraphicBuffer();
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
					S2Vector3 p = mesh.GetPosition(i);
					positions[i].data[0] = p[0];
					positions[i].data[1] = p[1];
					positions[i].data[2] = p[2];
				
					colors[i].data[0] = (float(rand()%100))/100.0f;
					colors[i].data[1] = (float(rand()%100))/100.0f;
					colors[i].data[2] = (float(rand()%100))/100.0f;	
				}
				GraphicBuffer::Option option;
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
				GraphicBuffer::IndexBufferElementType *indices = new GraphicBuffer::IndexBufferElementType[size];
				for(uint32_t i=0; i<mesh.GetTriangleSize(); i++) {
					indices[i*3] = mesh.GetTriangleVertexIndex(i, 0);
					indices[i*3+1] = mesh.GetTriangleVertexIndex(i, 1);
					indices[i*3+2] = mesh.GetTriangleVertexIndex(i, 2);
				}
				GraphicBuffer::Option option;
				option.InitializeAsIndexBuffer(size, indices);
				option.resource_write = RendererEnum::IMMUTABLE;
				index_buffer->Initialize(option);
				delete[] indices;
			}
		}
		
		instance_buffer = manager->CreateGraphicBuffer();
		{
			InstancingTestInstancePosition positions[10];
			for(uint32_t i=0; i<10; i++) {
				positions[i].data[0] = (float)5*i;
				positions[i].data[1] = (float)5*i;
				positions[i].data[2] = (float)5*i;
			}
			GraphicBuffer::Option option;
			option.Initialize(10, positions);
			option.resource_write = RendererEnum::IMMUTABLE;
			instance_buffer->Initialize(option);
		}	

		input_layout = manager->CreateInputLayout();
		input_layout->InitializeWithVertexBuffer(
		{
			{VertexBufferDescriptor::Create<InstancingTestPosition>(2), },
			{VertexBufferDescriptor::Create<InstancingTestColor>(0), },
			{VertexBufferDescriptor::Create<InstancingTestInstancePosition>(1), }
		}, 
			*vs);

		state = context->CreatePipelineState();

		state->SetVertexShader(vs);
		state->SetPixelShader(ps);
		state->SetInputLayout(input_layout);

		return true;
	}
	
	virtual s2string GetName() const {
		return "InstancingDemo";
	}
	
	virtual void OneFrame(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();

		pipeline->ClearDepthStencil(ds_buffer->AsDepthStencil(), true, 1.0f, true, 0);
		vs_data->SetUniform("view", camera->GetViewMatrix());
		vs_data->FlushConstantBuffer(pipeline);

		pipeline->SetState(*state);

		pipeline->SetPrimitiveTopology(GraphicPipeline::TRIANGLE_LIST);
		pipeline->SetDepthStencil(ds_buffer->AsDepthStencil());
		pipeline->SetRenderTarget(0, Engine::GetSingleton()->GetRendererContext()->GetBackBuffer()->AsRenderTarget());
		pipeline->SetVertexShaderData(vs_data);
		pipeline->SetVertexBuffer(2, position_buffer->AsVertexBuffer());
		pipeline->SetVertexBuffer(0, color_buffer->AsVertexBuffer());
		pipeline->SetVertexBuffer(1, instance_buffer->AsVertexBuffer());
		pipeline->SetIndexBuffer(index_buffer->AsIndexBuffer());
		pipeline->DrawInstanceIndex(0, index_buffer->GetElementCount(), 0, instance_buffer->GetElementCount());
	}
	
private:
	GraphicBuffer *position_buffer, *color_buffer;
	GraphicBuffer *instance_buffer;
	GraphicBuffer *index_buffer;
	VertexShader *vs;
	ShaderData *vs_data;
	PixelShader *ps;
	Texture2D *ds_buffer;
	InputLayout *input_layout;

	GraphicPipelineState *state;

	Camera *camera;
};

//AddBeforeMain(InstancingDemo)


}
