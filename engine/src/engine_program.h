#ifndef ENGINE_PROGRAM_H_
#define ENGINE_PROGRAM_H_

#include "utils/s2string.h"

#include <map>
#include <vector>

namespace s2 {

class EngineProgram {
public:
	virtual ~EngineProgram() {}
	virtual bool Initialize() = 0;
	virtual const s2string & GetName() = 0;
	virtual void OneFrame(float delta) = 0;
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
struct AddBeforeMain_P { \
	AddBeforeMain_P() { \
		P *program = new P; \
		Engine::GetSingleton()->GetEngineProgramManager()->Add(program); \
	} \
	static AddBeforeMain_P bind; \
}; \
AddBeforeMain_P AddBeforeMain_P::bind; \
}

#endif		//ENGINE_PROGRAM_H_