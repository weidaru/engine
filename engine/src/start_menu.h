#ifndef START_MENU_H_
#define START_MENU_H_

#include <stdint.h>
#include <vector>

#include "utils/s2vector4.h"

namespace s2 {
class EngineProgramManager;
class EngineProgram;
class Text;
class Entity;

class StartMenu {
private:
	struct ProgramData {
		EngineProgram *program;
		Text *text; 
	};

	enum State {
		SELECT, SELECT_INTERVAL
	};

public:
	//Assume there is no dynamic adding or removing in EngineProgramManager;
	StartMenu(EngineProgramManager *_manager);
	~StartMenu();

	void OneFrame(float delta);
	void OnEnter();
	void OnLeave();

	EngineProgram * GetProgram(uint32_t index) { return programs[index].program; }
	uint32_t GetSelectedIndex() { return selected_index; }

private:
	void SetSelectedIndex(uint32_t index);

private:
	std::vector<ProgramData> programs;
	Entity *text_container;
	uint32_t selected_index;

	S2Vector4 selected_color;
	S2Vector4 unselected_color;

	float check_interval, check_timer;
	State cur_state;
};

}

#endif // !START_MENU_H_
