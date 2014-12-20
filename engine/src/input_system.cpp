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

template <typename T>
T Clamp(T value, const T &floor, const T &ceiling) {
	T result = value<floor ? floor:value;
	return result>ceiling ? ceiling:result;
}

}

void InputSystem::SetMousePosition(int x, int y) {
	const RendererSetting &rs = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	mouse_x = Clamp(x, 0, (int)rs.window_width);
	mouse_y = Clamp(y, 0, (int)rs.window_height);
	RECT rect;
	GetWindowRect((HWND)hwnd, &rect);
	SetCursorPos( rect.left+mouse_x, rect.top+mouse_y);
}

void InputSystem::SetMousePositionPercent(float x, float y) {
	const RendererSetting &rs = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	SetMousePosition((int)(x*rs.window_width), (int)(y*rs.window_height));
}

InputSystem::InputSystem(void *_hwnd) 
			: hwnd(_hwnd),current(256), mouse_x(-1), mouse_y(-1), mouse_x_delta(0), mouse_y_delta(0), timer(0.0f) {
	SetMousePositionPercent(0.5f,0.5f);
}

float InputSystem::GetMouseXMove() const {
	return mouse_x_delta;
}

float InputSystem::GetMouseYMove() const {
	return  mouse_y_delta;
} 

void InputSystem::OneFrame(float delta) {
	CHECK(GetKeyboardState(&current[0]))<<GetLastError();
	POINT p;
	GetCursorPos(&p);
	ScreenToClient((HWND)hwnd, &p);
	int old_x = mouse_x;
	mouse_x = p.x;
	int old_y = mouse_y;
	mouse_y = p.y;

	if(timer <= 0.0f) {
		mouse_x_delta = (float)(mouse_x-old_x);
		mouse_y_delta = (float)(mouse_y-old_y);
		
		if(mouse_x_delta !=0.0f || mouse_y_delta!=0.0f) {
			float larger = abs(mouse_x_delta);
			larger = larger>abs(mouse_y_delta) ? larger:abs(mouse_y_delta);
			mouse_x_delta /= larger;
			mouse_y_delta /= larger;
			timer = 1.0f/100.0f;
		} else {
			timer = 0.0f;
		}
		
	}
	timer -= delta;
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
	
