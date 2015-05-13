#include "entity.h"
#include "entity_system.h"

#include "transform.h"

#include <glog/logging.h>

namespace s2 {

Entity::Entity(EntitySystem *_system) 
	: transform(0), system(_system), parent(0), enabled(true), id(GetCurrentId()++) {
	CHECK_NOTNULL(system);
	
	transform = new Transform(this);
	system->Add(this);
}

Entity::~Entity() {
	system->Remove(GetId());
	if(parent) {
		parent->RemoveChild(GetId());
	}
	
	delete RemoveComponent(transform->GetId());

	for(auto it = components.begin(); it != components.end();) {
		Component *c = it->second;
		it = components.erase(it);
		delete c;
	}
	for(auto it = children.begin(); it != children.end();) {
		Entity *e = it->second;
		it = children.erase(it);
		delete e;
	}
}

void Entity::AddChild(Entity *e) {
	CHECK_NOTNULL(e);
	children[e->GetId()] = e;
	e->parent = this;
}

Entity * Entity::RemoveChild(unsigned int id) {
	Entity *child = GetChild(id);
	children.erase(id);
	child->parent = 0;
	return child;
}

Entity * Entity::GetChild(unsigned int id) {
	auto it = children.find(id);
	if(it != children.end()) {
		return it->second;
	} else {
		return 0;
	}
}

void Entity::AddComponent(Component *c) {
	CHECK_NOTNULL(c);
	components[c->GetId()] = c;
	c->entity = this;
}

Component * Entity::RemoveComponent(unsigned int id) {
	Component *c = GetComponent(id);
	if(c) {
		components.erase(id);
		c->entity = 0;
	}
	return c;
}

Component * Entity::GetComponent(unsigned int id) {
	auto it = components.find(id);
	if(it != components.end()) {
		return it->second;
	} else {
		return 0;
	}
}

void Entity::OneFrame(float delta) {
	for(auto it=children.begin(); it != children.end(); it++) {
		if(it->second->IsEnabled()) {
			it->second->OneFrame(delta);
		}
	}
}

Matrix4x4 Entity::GetCascadeTransformMatrix() {
	if(parent) {
		return parent->GetCascadeTransformMatrix()  * transform->GetMatrix();
	} else {
		return transform->GetMatrix();
	}
}

}


