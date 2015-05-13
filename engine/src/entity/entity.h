#ifndef ENTITY_H_
#define ENTITY_H_

#include "transform.h"
#include "utils/s2string.h"

#include <map>

namespace s2 {

class EntitySystem;

class Entity {
public:
	Entity(EntitySystem *system);
	virtual ~Entity();

	Transform & GetTransform() {return transform;}
	Matrix4x4 GetCascadeTransformMatrix();

	void AddChild(Entity *e);
	Entity * RemoveChild(unsigned int id);
	Entity * GetChild(unsigned int id);

	void AddComponent(Component *c);
	Component * RemoveComponent(unsigned int id);
	Component * GetComponent(unsigned int id);

	Entity * GetParent() const { return parent; }
	unsigned int GetId() const { return id; }

	virtual Entity & SetEnabled(bool new_value) { enabled=new_value; return *this; } 
	bool IsEnabled() const { return enabled; }

	const s2string & GetName() const { return name; }

protected:
	virtual void OneFrame(float delta);

private :
	void SetName(const s2string& _name) { name=_name; }

	static unsigned int & GetCurrentId() {
		static unsigned int currentId = 0;
		return currentId;
	}

	friend class EntitySystem;

private:
	Transform transform;

	EntitySystem *system;
	Entity *parent;
	std::map<unsigned int, Component* > components;
	std::map<unsigned int, Entity *> children;
	bool enabled;

	unsigned int id;
	s2string name;
};

}


#endif			//ENTITY_H_