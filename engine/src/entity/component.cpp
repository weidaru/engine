#include "component.h"
#include "component_system.h"
#include "entity.h"

#include <glog/logging.h>

namespace s2 {

Component::Component(Entity *_entity) : 
	id(GetCurrentId()++), entity(0), enabled(true), system(0) {
	if(_entity) {
		_entity->AddComponent(this);
	}
}

Component::~Component() {
	if(system) {
		system->Deregister(this);
	}
	if(entity) {
		entity->RemoveComponent(GetId());
	}
}

void Component::OnSystemRegister(ComponentSystem *_system) {
	CHECK_NOTNULL(_system);
	system = _system;
}

void Component::OnSystemDeregister(ComponentSystem *_system) {
	if(system == _system) {
		system = 0;
	}
}

void Component::OnEntityRegister(Entity *_entity) {
	CHECK_NOTNULL(_entity);
	entity = _entity;
}

void Component::OnEntityDeregister(Entity *_entity) {
	if(entity == _entity) {
		entity = 0;
	}
}

}