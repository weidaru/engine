#include "component.h"
#include "entity.h"

#include <glog/logging.h>

namespace s2 {

Component::Component(Entity *_entity) : id(GetCurrentId()++), entity(_entity), enabled(true) {
	CHECK_NOTNULL(entity);

	entity->AddComponent(this);
}

Component::~Component() {
	if(entity) {
		entity->RemoveComponent(GetId());
	}
}
}