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

	void Add(Entity *e);
	Entity * Get(unsigned int id);
	Entity * Remove(unsigned int id);

	void OneFrame(float delta);

private:
	std::map<unsigned int, Entity *> entities;
};

}

#endif // !ENTITY_SYSTEM_H_
