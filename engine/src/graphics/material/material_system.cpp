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

void MaterialSystem::Register(Component *c) {
	Material *m = NiceCast(Material *, c);
	CHECK_NOTNULL(m);

	if(m->GetSystem() != 0) {
		LOG(ERROR)<<"Try to register a component twice.";
		return;
	}
	materials.push_back(m);
	m->OnSystemRegister(this);
}

void MaterialSystem::Deregister(Component *c) {
	auto it = std::find_if(materials.begin(), materials.end(), 
		[c](Material* candiate){ return candiate->GetId()==c->GetId();});
	if(it != materials.end()) {
		materials.erase(it);
		c->OnSystemDeregister(this);
	}
}

void MaterialSystem::OneFrame(float delta) {

}


}

