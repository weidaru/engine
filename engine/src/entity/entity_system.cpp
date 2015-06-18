#include <glog/logging.h>

#include "entity_system.h"
#include "entity.h"

namespace s2 {

EntitySystem::EntitySystem() {

}

EntitySystem::~EntitySystem() {
	for(auto it = entities.begin(); it != entities.end();) {
		Entity *e = it->second;
		it = entities.erase(it);
		delete e;
	}
}

void EntitySystem::Add(Entity * e) {
	CHECK(e) << "entity cannot be null";
	entities[e->GetId()] = e;
}

Entity * EntitySystem::Get(uint32_t id) {
	auto it = entities.find(id);
	if(it == entities.end()) {
		return 0;
	}  else {
		return it->second;
	}
}

Entity * EntitySystem::Remove(uint32_t id) {
	auto it = entities.find(id);
	Entity *e = 0;
	if(it != entities.end()) {
		e = it->second;
		entities.erase(it);
	}
	
	return e;
}

void EntitySystem::OneFrame(float delta) {
	for(auto it=entities.begin(); it!=entities.end(); it++) {
		if(it->second->IsEnabled()) {
			it->second->OneFrame(delta);
		}
	}
}


}

