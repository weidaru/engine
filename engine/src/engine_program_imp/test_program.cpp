#include "engine_program.h"
#include "engine.h"

#include <stdio.h>

namespace s2 {

class TestProgram : public EngineProgram {
public:
	virtual ~TestProgram() {}
	virtual bool Initialize(){
		printf("Initialize test program.\n");
		return true;
	}
	
	virtual const s2string & GetName() {
		static s2string name="test";
		return name;
	}
	
	virtual void OneFrame(float delta) {
		static bool first = true;
		if(first) {
			printf("Hello World from test program.\n");
			first = false;
		}
	}
};

AddBeforeMain(TestProgram)

}

