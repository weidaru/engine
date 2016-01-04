#include "engine_program.h"

#include <glog/logging.h>

#include "entity/entity.h"

namespace s2 {

EngineProgram::~EngineProgram() {}

void EngineProgram::SaveEntityEnabledState() {
	std::vector<Entity *> entities;
	entity_system.GetAll(&entities);

	for(uint32_t i=0; i<entities.size(); i++) {
		enabled_map.insert(std::make_pair(entities[i]->GetId(), entities[i]->IsEnabled()));
	}
}

void EngineProgram::RestoreEntityEnabledState() {
	for(auto it=enabled_map.begin(); it!=enabled_map.end(); it++) {
		Entity *entity = entity_system.Get(it->first); 
		bool new_enabled = it->second;
		if(new_enabled != entity->IsEnabled()) {
			entity->SetEnabled(new_enabled);
		}
	}
}

EngineProgramManager::EngineProgramManager() {}

EngineProgramManager::~EngineProgramManager() {
	for(Map::iterator it=map.begin(); it!=map.end(); it++) {
		delete it->second;
	}
}

EngineProgramManager & EngineProgramManager::Add(EngineProgram *program) {
	CHECK(Get(program->GetName()) == 0)<<"Name conflict.";
	map[program->GetName()] = program;
	return *this;
}

EngineProgram * EngineProgramManager::Get(const s2string &name) {
	auto it = map.find(name);
	if( it == map.end()) {
		return 0;
	} else {
		return it->second;
	}
}

void EngineProgramManager::GetAll(std::vector<EngineProgram *> *result) {
	for(Map::iterator it=map.begin(); it!=map.end(); it++) {
		result->push_back(it->second);
	}
}

void EngineProgramManager::Release(const s2string &name) {
	delete map[name];
	map.erase(name);
}


}
