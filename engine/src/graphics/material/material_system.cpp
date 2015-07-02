#include "material_system.h"
#include "material.h"

#include <glog/logging.h>

#include <algorithm>

#ifdef NDEBUG
	#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
	#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif

namespace s2 {

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

