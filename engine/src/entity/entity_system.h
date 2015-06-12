#ifndef ENTITY_SYSTEM_H_
#define ENTITY_SYSTEM_H_

#include <map>

#include "utils/s2string.h"
#include <stdint.h>

namespace s2 {
class Entity;

class EntitySystem {
public:
	EntitySystem();
	~EntitySystem();

	void Add(Entity *e);
	Entity * Get(uint32_t id);
	Entity * Remove(uint32_t id);

	void OneFrame(float delta);

private:
	std::map<uint32_t, Entity *> entities;
};

}

#endif // !ENTITY_SYSTEM_H_
