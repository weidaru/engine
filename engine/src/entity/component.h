#ifndef COMPONENT_H_
#define COMPONENT_H_

#include "utils/s2string.h"
#include <stdint.h>

namespace s2 {

class Entity;
class ComponentSystem;

class Component {
public:
	Component(Entity *_entity);
	virtual ~Component();

	uint32_t GetId() const { return id; } 
	const s2string & GetName() const { return name;  }
	
	Component & SetName(const s2string &new_name) { name = new_name; return *this; }
	Entity * GetEntity() { return entity; }

	Component & SetEnabled(bool new_value) { enabled = new_value; return *this; }
	bool IsEnabled() { return enabled; }

	ComponentSystem *GetSystem() { return system; }

	//Called on system Register.
	virtual void OnSystemRegister(ComponentSystem *_system);
	//Called on system Deregister.
	virtual void OnSystemDeregister(ComponentSystem *_system);

	//Called on entity AddComponent.
	virtual void OnEntityRegister(Entity *_entity);
	//Called on entity RemoveComponent.
	virtual void OnEntityDeregister(Entity *_entity);

private:
	static uint32_t & GetCurrentId() {
		static uint32_t currentId = 0;
		return currentId;
	}

private:
	uint32_t id;
	s2string name;
	Entity *entity;
	bool enabled;

	ComponentSystem *system;
};

}

#endif			//COMPONENT_H_
