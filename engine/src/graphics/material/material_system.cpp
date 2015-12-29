#include "material_system.h"
#include "material.h"

#include "asset/mesh.h"
#include "asset/asset_path.h"

#include <glog/logging.h>
#include <algorithm>

#include "entity/entity.h"
#include "entity/transform.h"

#include "scene/camera.h"
#include "scene/scene_manager.h"

#include "graphics/renderer/all.h"

#include "engine.h"

#ifdef NDEBUG
	#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
	#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif

namespace s2 {

MeshData::MeshData(const Mesh &mesh) {
	GraphicResourceManager *manager= Engine::GetSingleton()->GetRendererContext()->GetResourceManager();

	uint32_t vertex_size = mesh.GetVertexSize();
	MaterialVertexData *vb_data = new MaterialVertexData[vertex_size];
	for(uint32_t i=0; i<vertex_size; i++) {
		vb_data[i].position = mesh.GetPosition(i);
		vb_data[i].normal = mesh.GetNormal(i);
		if(mesh.GetColorSetSize()!=0) {
			vb_data[i].color = mesh.GetColor(0, i);
		} else {
			vb_data[i].color = S2Vector4(0.8f, 0.8f, 0.8f, 1.0f);
		}
	}
	vb = manager->CreateGraphicBuffer();
	GraphicBuffer::Option vb_opt;
	vb_opt.Initialize(vertex_size, vb_data);
	vb->Initialize(vb_opt);
	delete[] vb_data;

	uint32_t triangle_size = mesh.GetTriangleSize();
	uint32_t *ib_data = new uint32_t[triangle_size*3];
	for(uint32_t i=0; i<triangle_size; i++) {
		ib_data[i*3] = mesh.GetTriangleVertexIndex(i, 0);
		ib_data[i*3+1] = mesh.GetTriangleVertexIndex(i, 1);
		ib_data[i*3+2] = mesh.GetTriangleVertexIndex(i, 2);
	}
	ib = manager->CreateGraphicBuffer();
	GraphicBuffer::Option ib_opt;
	ib_opt.InitializeAsIndexBuffer(triangle_size*3, ib_data);
	ib->Initialize(ib_opt);
	delete[] ib_data;
}

MeshData::~MeshData() {
	GraphicResourceManager *manager= Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
	manager->RemoveGraphicBuffer(vb->GetID());
	manager->RemoveGraphicBuffer(ib->GetID());
}


MaterialSystem::MaterialSystem(GraphicResourceManager *_manager) 
		: manager(_manager), vs(0), ps(0), vs_data(0), ps_data(0), input_layout(0), pipeline_state(0) {
	CHECK_NOTNULL(manager);
	
	vs = manager->CreateVertexShader();
	CHECK(vs->Initialize(ResolveAssetPath("shader/simple_material_vs.hlsl"), "main"))<<vs->GetLastError();

	ps = manager->CreatePixelShader();
	CHECK(ps->Initialize(ResolveAssetPath("shader/simple_material_ps.hlsl"), "main"))<<ps->GetLastError();

	input_layout = manager->CreateInputLayout();
	input_layout->InitializeWithVertexBuffer(
		{
			VertexBufferDescriptor::Create<MaterialVertexData>(0)
		}, 
		*vs);

	vs_data = manager->CreateShaderData();
	vs_data->Initialize(vs->GetShaderBytecode());
	
	ps_data = manager->CreateShaderData();
	ps_data->Initialize(ps->GetShaderBytecode());

	
	RendererContext *context = Engine::GetSingleton()->GetRendererContext();
	SceneManager *scene_manager = Engine::GetSingleton()->GetSceneManager();
	
	pipeline_state = context->CreatePipelineState();
	RasterizationOption rast_opt = pipeline_state->GetRasterizationOption();
	rast_opt.cull_mode = rast_opt.NONE;

	pipeline_state->SetRasterizationOption(rast_opt);
	pipeline_state->SetPrimitiveTopology(GraphicPipeline::TRIANGLE_LIST);
	pipeline_state->SetRenderTarget(0, context->GetBackBuffer()->AsRenderTarget());
	pipeline_state->SetDepthStencil(scene_manager->GetDepthStencilBuffer()->AsDepthStencil());
	pipeline_state->SetVertexShader(vs);
	pipeline_state->SetPixelShader(ps);
	pipeline_state->SetVertexShaderData(vs_data);
	pipeline_state->SetPixelShaderData(ps_data);
	pipeline_state->SetInputLayout(input_layout);
}

MaterialSystem::~MaterialSystem() {
	GraphicResourceManager *manager= Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
	manager->RemoveVertexShader(vs->GetID());
	manager->RemovePixelShader(ps->GetID());
	manager->RemoveShaderData(vs_data->GetID());
	manager->RemoveShaderData(ps_data->GetID());
	manager->RemoveInputLayout(input_layout->GetID());
	
	delete pipeline_state;
}

static const char * kComponentDestroyCallbackName = "material_system_component_destroy_cb";

void MaterialSystem::Register(Material *m) {
	CHECK_NOTNULL(m);
	materials.push_back(m);
	m->AddDestroyCallback(kComponentDestroyCallbackName, [this](Component *c){
		for(auto it=this->materials.begin(); it!=this->materials.end(); it++) {
			if((*it)->GetId() == c->GetId()) {
				this->materials.erase(it);
				break;
			}
		}
	});
}

void MaterialSystem::Deregister(Material *m) {
	auto it = std::find(materials.begin(), materials.end(), m);
	if(it != materials.end()) {
		materials.erase(it);
		(*it)->RemoveDestroyCallback(kComponentDestroyCallbackName);
	}
}

void MaterialSystem::OneFrame(float delta) {
	if(materials.empty()) {
		return;
	}

	RendererContext *renderer_context = Engine::GetSingleton()->GetRendererContext();
	GraphicPipeline *pipeline = renderer_context->GetPipeline();
	SceneManager *scene_manager = Engine::GetSingleton()->GetSceneManager();
	Camera *camera = scene_manager->GetCamera();

	pipeline_state->Flush(pipeline);

	for(uint32_t i=0; i<materials.size(); i++) {
		Material *m = materials[i];
		MeshData *mesh_data = m->GetMeshData();
		Transform *transform = m->GetEntity()->GetTransform();

		ps_data->SetUniform("world", transform->GetMatrix());
		ps_data->SetUniform("view", camera->GetViewMatrix());
		ps_data->SetUniform("projection", scene_manager->GetProjectionMatrix());
		ps_data->FlushConstantBuffer(pipeline);

		vs_data->SetUniform("world", transform->GetMatrix());
		vs_data->SetUniform("view", camera->GetViewMatrix());
		vs_data->SetUniform("projection", scene_manager->GetProjectionMatrix());
		vs_data->FlushConstantBuffer(pipeline);

		pipeline->SetVertexBuffer(0, mesh_data->GetVertexBuffer()->AsVertexBuffer());
		pipeline->SetIndexBuffer(mesh_data->GetIndexBuffer()->AsIndexBuffer());
		pipeline->Draw(0, mesh_data->GetIndexBuffer()->GetElementCount());
	}
}


}

