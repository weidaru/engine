#include "input_system.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef ERROR

#include <glog/logging.h>

namespace s2 {


namespace {

int MouseButtonToVirtualKey(InputSystem::MouseButton button) {
	switch(button) {
	case InputSystem::M_LEFT:
		return VK_LBUTTON;
	case InputSystem::M_RIGHT:
		return VK_RBUTTON;
	case InputSystem::M_MID:
		return VK_MBUTTON;
	default:
		CHECK(false)<<"Unknown button "<<button;
		return 0;
	}
}

int KeyToVirtualKey(InputSystem::Key key) {
	if(key>=InputSystem::K_0 && key<=InputSystem::K_9) {
		return key-InputSystem::K_0+0x30;
	} else if(key>=InputSystem::K_A && key<=InputSystem::K_Z) {
		return key-InputSystem::K_A+0x41;
	} else {
		switch(key) {
		case InputSystem::K_SPACE:
			return VK_SPACE;
		default:
			CHECK(false)<<"Unknown key "<<key;
			return 0;
		}
	}
}

}

InputSystem::InputSystem(void *_hwnd) 
			: hwnd(_hwnd),current(256), mouse_x(-1), mouse_y(-1), mouse_x_delta(0), mouse_y_delta(0) {
	
}

void InputSystem::OneFrame(float delta) {
	CHECK(GetKeyboardState(&current[0]))<<GetLastError();
	
	POINT p;
	CHECK(GetCursorPos(&p))<<"Cannot get absolute cursor position. Error "<<GetLastError();
	CHECK(ScreenToClient((HWND)hwnd, &p))<<"Cannot get relative cursor position. Error"<<GetLastError();
	mouse_x_delta = p.x-mouse_x;
	mouse_x = p.x;
	mouse_y_delta = p.y-mouse_y;
	mouse_y = p.y;
}

bool InputSystem::IsMouseButtonDown(MouseButton button) const {
	int index = MouseButtonToVirtualKey(button);
	return (current[index]&0x80)!=0;
}

bool InputSystem::IsKeyDown(Key key) const {
	int index = KeyToVirtualKey(key);
	return (current[index]&0x80)!=0;
}

}
	
