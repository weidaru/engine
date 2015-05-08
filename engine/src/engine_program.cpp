#include "engine_program.h"

#include <glog/logging.h>

namespace s2 {

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

