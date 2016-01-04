#ifndef ENGINE_PROGRAM_H_
#define ENGINE_PROGRAM_H_

#include "utils/s2string.h"

#include <map>
#include <vector>
#include <stdint.h>

#include "entity/entity_system.h"

namespace s2 {

class Entity;

class EngineProgram {
public:
	virtual ~EngineProgram();
	virtual void OnEnter() {}
	virtual void OnLeave() {}
	virtual void OneFrame(float delta) {}

	virtual bool Initialize() = 0;
	virtual s2string GetName() const = 0;

	EntitySystem *GetEntitySystem() { return &entity_system; }

	void SaveEntityEnabledState();
	void RestoreEntityEnabledState();
	
protected:
	EntitySystem entity_system;
	std::map<uint32_t, bool> enabled_map;
};

class EngineProgramManager {
private:
	typedef std::map<s2string, EngineProgram *> Map;

public:
	EngineProgramManager();
	~EngineProgramManager();

	EngineProgramManager & Add(EngineProgram *program);
	EngineProgram * Get(const s2string &name);
	void GetAll(std::vector<EngineProgram *> *result);
	void Release(const s2string &name);

private:
	Map map;
};
}

#define AddBeforeMain(P) \
namespace add_before_main{ \
struct AddBeforeMain_##P { \
	AddBeforeMain_##P() { \
		P *program = new P; \
		Engine::GetSingleton()->GetEngineProgramManager()->Add(program); \
	} \
	static AddBeforeMain_##P bind; \
}; \
AddBeforeMain_##P AddBeforeMain_##P::bind; \
}

#endif		//ENGINE_PROGRAM_H_
