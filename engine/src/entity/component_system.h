#ifndef COMPONENT_SYSTEM_H
#define COMPONENT_SYSTEM_H

namespace s2 {

class Component;

class ComponentSystem {
public:
	virtual ~ComponentSystem() {}

	virtual void Register(Component *c) = 0;
	virtual void Deregister(Component *c) = 0;
};

}


#endif // !COMPONENT_SYSTEM_H
