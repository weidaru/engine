#ifndef COMPONENT_H_
#define COMPONENT_H_

#include "utils/s2string.h"
#include <stdint.h>
#include <functional>
#include <map>

namespace s2 {

class Entity;

class Component {
public:
	typedef std::function<void (Component *)> Callback;

public:
	Component(Entity *_entity);
	virtual ~Component();

	uint32_t GetId() const { return id; } 
	const s2string & GetName() const { return name;  }
	
	Component & SetName(const s2string &new_name) { name = new_name; return *this; }
	Entity * GetEntity() { return entity; }

	Component & SetEnabled(bool new_value) { enabled = new_value; return *this; }
	bool IsEnabled() { return enabled; }

	//Called on entity AddComponent.
	virtual void OnEntityRegister(Entity *_entity);
	//Called on entity RemoveComponent.
	virtual void OnEntityDeregister(Entity *_entity);

	void AddDestroyCallback(const s2string &cb_name, Callback cb);
	void RemoveDestroyCallback(const s2string &cb_name);

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

	std::map<s2string, Callback> destroy_cbs;
};

}

#endif			//COMPONENT_H_
