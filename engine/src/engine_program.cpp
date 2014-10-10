#include "engine_program.h"

namespace s2 {

EngineProgramManager::EngineProgramManager() {}

EngineProgramManager::~EngineProgramManager() {
	for(Map::iterator it=map.begin(); it!=map.end(); it++) {
		delete it->second;
	}
}

EngineProgramManager & EngineProgramManager::Add(EngineProgram *program) {
	map[program->GetName()] = program;
	return *this;
}

EngineProgram * EngineProgramManager::Get(const s2string &name) {
	return map[name];
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

