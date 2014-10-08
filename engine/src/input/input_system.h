#ifndef INPUT_H_
#define INPUT_H_

#include "utils/singleton.h"

struct IDirectInput8;
struct IDirectInputDevice8;
struct _DIMOUSESTATE;

namespace s2 {

class InputSystem {
public:
	InputSystem();
	~InputSystem();

	void Initialize();

	bool OneFrame();
	
	bool IsKeyDown(int key_enum);
	int GetMouseX();
	int GetMouseY(); 
	bool GetIsMouseDown(int button_num);

private:
	IDirectInput8* directinput_;
	IDirectInputDevice8* keyboard_device_;
	IDirectInputDevice8* mouse_device_;

	unsigned char keyboard_state_[256];
	_DIMOUSESTATE *mouse_state_;

	int mousex_, mousey_;
	int resolution_x_, resolution_y_;
};

}

#endif		//INPUT_H_