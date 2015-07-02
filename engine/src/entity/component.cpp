#include "component.h"
#include "entity.h"

#include <glog/logging.h>

namespace s2 {

Component::Component(Entity *_entity) : 
	id(GetCurrentId()++), entity(0), enabled(true) {
	if(_entity) {
		_entity->AddComponent(this);
	}
}

Component::~Component() {
	if(entity) {
		entity->RemoveComponent(GetId());
	}
	for(auto it=destroy_cbs.begin(); it != destroy_cbs.end(); it++) {
		it->second(this);
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

void Component::AddDestroyCallback(const s2string &cb_name, Callback cb) {
	CHECK(destroy_cbs.find(cb_name)==destroy_cbs.end())<<"Name "<<cb_name<<" already exists.";
	destroy_cbs[cb_name] = cb;
}

void Component::RemoveDestroyCallback(const s2string &cb_name) {
	destroy_cbs.erase(cb_name);
}

}