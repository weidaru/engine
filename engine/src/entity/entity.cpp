#include "entity.h"

#include <glog/logging.h>

namespace s2 {

Entity::Entity() : parent(0), enabled(true), id(GetCurrentId()++) {

}

Entity::~Entity() {

}

void Entity::AddChild(const s2string &name, Entity *e) {
	CHECK_NOTNULL(e);
	CHECK(GetChild(name) == 0)<<"Child with name already exists. Use replace instead.";
	children[name] = e;
	e->parent = this;
}

Entity * Entity::RemoveChild(const s2string &name) {
	Entity *child = GetChild(name);
	children.erase(name);
	child->parent = 0;
	return child;
}

Entity * Entity::GetChild(const s2string &name) const {
	auto it = children.find(name);
	if(it != children.end()) {
		return it->second;
	} else {
		return 0;
	}
}

void Entity::AddComponent(const s2string &name, Component *c) {
	CHECK_NOTNULL(c);
	CHECK(GetComponent(name) == 0)<<"Component with name already exists. Use replace instead.";
	components[name] = c;
	c->entity = this;
}

Component * Entity::RemoveComponent(const s2string &name) {
	Component *c = GetComponent(name);
	components.erase(name);
	c->entity = 0;
	return c;
}

Component * Entity::GetComponent(const s2string &name) const {
	auto it = components.find(name);
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
		return parent->GetCascadeTransformMatrix()  * transform.GetMatrix();
	} else {
		return transform.GetMatrix();
	}
}

}


