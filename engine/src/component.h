#ifndef COMPONENT_H_
#define COMPONENT_H_

class Entity;

class Component {
public:
	virtual ~Component() {}
	Component & setEntity(Entity *e) { e_ = e; return *this; }
	Entity * getEntity() { return e_; }

protected:
	Entity *e_;
};

#endif COMPONENT_H_