#ifndef ENTITY_H_
#define ENTITY_H_

#include "transform.h"
#include "utils/s2string.h"

#include <map>

namespace s2 {

class EntitySystem;

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

	const s2string & GetName() { return name; }

private :
	void SetName(const s2string& _name) { name=_name; }

	static unsigned int & GetCurrentId() {
		static unsigned int currentId = 0;
		return currentId;
	}

	friend class EntitySystem;

private:
	Transform transform;

	Entity *parent;
	std::map<s2string, Component* > components;
	std::map<s2string, Entity *> children;
	bool enabled;

	unsigned int id;
	s2string name;
};

}


#endif			//ENTITY_H_