#include "input_system.h"
#include "engine.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
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
	RECT rect;
	GetWindowRect((HWND)hwnd, &rect);
	mouse_x = (rect.right - rect.left)/2;
	mouse_y = (rect.bottom - rect.top)/2;
	SetCursorPos((rect.right + rect.left)/2, (rect.bottom + rect.top)/2);
}

void InputSystem::Update(int delta_x, int delta_y) {
	CHECK(GetKeyboardState(&current[0]))<<GetLastError();
	
	if(mouse_x == -1) {
		POINT p;
		GetCursorPos(&p);
		ScreenToClient((HWND)hwnd, &p);
		mouse_x = p.x;
		mouse_y = p.y;
		mouse_x_delta += delta_x;
		mouse_y_delta += delta_y;
	} else {
		mouse_x_delta += delta_x;
		mouse_y_delta += delta_y;
		mouse_x += delta_x;
		mouse_y += delta_y;
	}
}

void InputSystem::PostFrame(float delta) {
	mouse_x_delta = 0;
	mouse_y_delta = 0;
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
	
