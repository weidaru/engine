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

RECT GetDrawingRect(HWND hwnd) {
	RECT rect;
	GetWindowRect(hwnd, &rect);
	DWORD style = GetWindowStyle(hwnd);
	if(style & WS_DLGFRAME) {
		int	caption_height = GetSystemMetrics(SM_CYCAPTION);
		rect.top += caption_height;
	}

	if(style & WS_BORDER) {
		int border_width = GetSystemMetrics(SM_CXBORDER);
		int border_height = GetSystemMetrics(SM_CYBORDER);
		rect.left += border_width;
		rect.right -= border_width;
		rect.top += border_height;
		rect.bottom -= border_height;
	}

	return rect;
}

}

void InputSystem::SetMousePosition(int x, int y) {
	if(GetForegroundWindow() == hwnd) {
		const RendererSetting &rs = Engine::GetSingleton()->GetRendererContext()->GetSetting();
		mouse_x = Clamp(x, 0, (int)rs.window_width);
		mouse_y = Clamp(y, 0, (int)rs.window_height);
		RECT rect = GetDrawingRect((HWND)hwnd);
		SetCursorPos( rect.left+mouse_x, rect.top+mouse_y);
	}
}

void InputSystem::SetMousePositionPercent(float x, float y) {
	const RendererSetting &rs = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	SetMousePosition((int)(x*rs.window_width), (int)(y*rs.window_height));
}

InputSystem::InputSystem(void *_hwnd) 
			: hwnd(_hwnd),current(256), mouse_x(-1), mouse_y(-1), mouse_x_delta(0), mouse_y_delta(0){
	SetMousePositionPercent(0.5f,0.5f);
}

int InputSystem::GetMouseXMove() const {
	return mouse_x_delta;
}

int InputSystem::GetMouseYMove() const {
	return  mouse_y_delta;
} 

void InputSystem::OneFrame(float delta) {
	if(GetForegroundWindow() == hwnd) {
		CHECK(GetKeyboardState(&current[0]))<<GetLastError();
		POINT p;
		GetCursorPos(&p);
		RECT rect = GetDrawingRect((HWND)hwnd);
		
		int old_x = mouse_x;
		mouse_x = p.x-rect.left;
		int old_y = mouse_y;
		mouse_y = p.y-rect.top;

		mouse_x_delta = mouse_x-old_x;
		mouse_y_delta = mouse_y-old_y;
	}
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
	
