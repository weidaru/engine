#include "entity.h"
#include "component.h"
#include "factory.h"

#include <glog/logging.h>

Entity::ID Entity::kCurId = 0;

Entity::Entity() : id_(++kCurId) {

}

Entity::ID Entity::getId() {
	return id_; 
}

Entity::~Entity() {
	for(CompMap::iterator it = data_.begin(); it != data_.end(); it++) {
		delete it->second;
	}
}

Component* Entity::GetComponent(const std::string &comp_name) {
	if(data_.find(comp_name) == data_.end())
		return NULL;
	else
		return data_[comp_name];
}

void Entity::RemoveComponent(const std::string &comp_name) {
	delete data_[comp_name];
	data_.erase(comp_name);
}

Component* Entity::AddComponent(const std::string &comp_name) {
	if(data_.find(comp_name) != data_.end()) {
		LOG(WARNING)<< "Entity " << getId()<< "already has component "<<comp_name;
		return data_[comp_name];
	}
	Component *new_comp = Factory<Component>::Create(comp_name);
	if(new_comp)
		data_[comp_name] = new_comp;
	return new_comp;
}