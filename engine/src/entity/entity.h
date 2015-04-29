#ifndef ENTITY_H_
#define ENTITY_H_

#include "transform.h"

#include <map>

namespace s2 {

class Entity {
public:
	Entity();
	virtual ~Entity();

	Transform & GetTransform() {return transform;}
	Matrix4x4 GetCascadeTransformMatrix();

	void AddChild(const s2string &name, Entity *e);
	Entity * RemoveChild(const s2string &name);
	Entity * GetChild(const s2string &name) const;

	void AddComponent(const s2string &name, Component *c);
	Component * RemoveComponent(const s2string &name);
	Component * GetComponent(const s2string &name) const;

	Entity * GetParent() const { return parent; }

	Entity & SetEnabled(bool new_value) { enabled=new_value; return *this; } 
	bool IsEnabled() { return enabled; }

	virtual void OneFrame(float delta);

private:
	Transform transform;

	Entity *parent;
	std::map<s2string, Component* > components;
	std::map<s2string, Entity *> children;
	bool enabled;
};

}


#endif			//ENTITY_H_