#include "material_system.h"
#include "material.h"

#include "asset/mesh.h"

#include <glog/logging.h>

#include <algorithm>

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
		vb_data[i].color = S2Vector4(0.8f, 0.8f, 0.8f, 1.0f);
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


MaterialSystem::MaterialSystem() {

}

MaterialSystem::~MaterialSystem() {

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

}


}

