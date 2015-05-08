#ifndef ENTITY_SYSTEM_H_
#define ENTITY_SYSTEM_H_

#include <map>

#include "utils/s2string.h"


namespace s2 {
class Entity;

class EntitySystem {
public:
	EntitySystem();
	~EntitySystem();

	void Add(const s2string &name, Entity *e);
	Entity * Get(const s2string &name);
	void Remove(const s2string &name);

	void OneFrame(float delta);

private:
	std::map<s2string, Entity *> entities;
};

}

#endif // !ENTITY_SYSTEM_H_
