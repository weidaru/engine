#include <glog/logging.h>

#include "entity_system.h"
#include "entity.h"

namespace s2 {

EntitySystem::EntitySystem() {

}

EntitySystem::~EntitySystem() {
	for(auto it = entities.begin(); it != entities.end(); it++) {
		delete it->second;
	}
}

void EntitySystem::Add(const s2string &name, Entity * e) {
	CHECK(Get(name) == 0)<<name <<" already exists.";
	CHECK(e) << "entity cannot be null";
	e->SetName(name);
	entities[name] = e;
}

Entity * EntitySystem::Get(const s2string &name) {
	auto it = entities.find(name);
	if(it == entities.end()) {
		return 0;
	}  else {
		return it->second;
	}
}

void EntitySystem::Remove(const s2string &name) {
	entities.erase(name);
}

void EntitySystem::OneFrame(float delta) {
	for(auto it=entities.begin(); it!=entities.end(); it++) {
		it->second->OneFrame(delta);
	}
}


}

