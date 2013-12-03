#ifndef ENTITY_H_
#define ENTITY_H_

#include <string>
#include <map>

class Component;
class Entity {
public:
	typedef unsigned int ID;

private:
	typedef std::map<std::string, Component *> CompMap;

public:
	Entity();
	~Entity();

	Component* GetComponent(const std::string &comp_name);
	void RemoveComponent(const std::string &comp_name);
	Component* AddComponent(const std::string &comp_name);

	ID getId();

private:
	Entity(Entity &);
	Entity & operator=(Entity&);

private:
	CompMap data_;
	ID id_;

	static ID kCurId;
};

#endif			//ENTITY_H_