#ifndef ENTITY_H_
#define ENTITY_H_

#include "utils/s2string.h"
#include "utils/s2matrix4x4.h"

#include <map>
#include <stdint.h>
#include <stack>

namespace s2 {

class EntitySystem;
class Transform;
class Component;

class Entity {
public:
	Entity(EntitySystem *system);
	Entity();
	virtual ~Entity();

	Transform * GetTransform() {return transform;}
	const Transform * GetTransform() const {return transform;}
	S2Matrix4x4 GetCascadeTransformMatrix();

	void AddChild(Entity *e);
	Entity * RemoveChild(uint32_t id);
	Entity * GetChild(uint32_t id);

	void AddComponent(Component *c);
	Component * RemoveComponent(uint32_t id);
	Component * GetComponent(uint32_t id);

	Entity * GetParent() const { return parent; }
	uint32_t GetId() const { return id; }

	virtual Entity & SetEnabled(bool new_value);
	bool IsEnabled() const { return enabled; }

	const s2string & GetName() const { return name; }

	virtual void OneFrame(float delta);

	void SetEntitySystem(EntitySystem* system);

private :
	void SetName(const s2string& _name) { name=_name; }

	static uint32_t & GetCurrentId() {
		static uint32_t currentId = 0;
		return currentId;
	}


private:
	Transform *transform;

	EntitySystem *system;
	Entity *parent;
	std::map<uint32_t, Component* > components;
	std::map<uint32_t, Entity *> children;
	bool enabled;
	std::stack<bool> enable_stack;

	uint32_t id;
	s2string name;
};

}


#endif			//ENTITY_H_