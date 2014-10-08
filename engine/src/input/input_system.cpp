#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#define DIRECTINPUT_VERSION 0x0800
#define WIN32_LEAN_AND_MEAN
#include <dinput.h>
#undef ERROR


#include "input_system.h"
#include "engine.h"

#include <glog/logging.h>

namespace s2 {

InputSystem::InputSystem() : 
		directinput_(0), mouse_device_(0), keyboard_device_(0), 
		mousex_(0), mousey_(0), resolution_x_(0), resolution_y_(0) {
	mouse_state_ = new DIMOUSESTATE;
}

void InputSystem::Initialize() {
	HINSTANCE hinst = Engine::GetSingleton()->GetInstanceHandle();
	HWND hwnd = Engine::GetSingleton()->GetWindowHandle();
	HRESULT result;

	RendererSetting setting;
	Engine::GetSingleton()->GetRendererContext()->GetSetting(&setting);
	this->resolution_x_ = setting.window_width;
	this->resolution_y_ = setting.window_height;

	//Direct Input
	result = DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&directinput_, NULL);
	CHECK(!FAILED(result)) << "Cannot create Direct Input. Version" << DIRECTINPUT_VERSION <<  " Error " << GetLastError();

	//Keyboard
	result = directinput_->CreateDevice(GUID_SysKeyboard, &keyboard_device_, NULL);
	CHECK(!FAILED(result))<< "Cannnot create keyboard device. Error " << GetLastError();
	result = keyboard_device_->SetDataFormat(&c_dfDIKeyboard);
	CHECK(!FAILED(result))<<"Bad data format for keyboard. Error " << GetLastError();
	result = keyboard_device_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	CHECK(!FAILED(result))<<"Cannot set cooperative level for keyboard. Error " << GetLastError();
	result = keyboard_device_->Acquire();
	CHECK(!FAILED(result))<<"Cannot acquire keyboard. Error " << GetLastError();

	//Mouse
	result = directinput_->CreateDevice(GUID_SysMouse, &mouse_device_, NULL);
	CHECK(!FAILED(result))<<"Cannot create mouse device. Error " << GetLastError();
	result = mouse_device_->SetDataFormat(&c_dfDIMouse);
	CHECK(!FAILED(result))<<"Bad data format for mouse. Error " << GetLastError();
	result = mouse_device_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	CHECK(!FAILED(result))<<"Cannot set cooperative level for mouse. Error " << GetLastError();
	result = mouse_device_->Acquire();
	CHECK(!FAILED(result))<<"Cannot acquire mouse. Error " << GetLastError();
}

InputSystem::~InputSystem() {
	if(keyboard_device_) {
		keyboard_device_->Unacquire();
		keyboard_device_->Release();
		keyboard_device_ = 0;
	}
	if(mouse_device_) {
		mouse_device_->Unacquire();
		mouse_device_->Release();
		mouse_device_ = 0;
	}
	if(directinput_) {
		directinput_->Release();
		directinput_ = 0;
	}

	delete mouse_state_;
}

/**
 * Get input state each frame.
 */
bool InputSystem::OneFrame() {
	HRESULT result;

	//Keyboard
	result = keyboard_device_->GetDeviceState(sizeof(keyboard_state_), (void *)&keyboard_state_);
	if(FAILED(result)){
		// If the keyboard lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			keyboard_device_->Acquire();
		else {
			LOG_EVERY_N(ERROR, 300)<<"Error getting keyboard state. result = "<<result;
		}
	}

	//Mouse
	// Read the mouse device.
	result = mouse_device_->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)mouse_state_);
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			mouse_device_->Acquire();
		else
			LOG_EVERY_N(ERROR, 300)<<"Error getting mouse state. result = "<<result;
	}
	//update  mouse x y
	mousex_ += mouse_state_->lX;
	mousey_ += mouse_state_->lY;

	mousex_ = (mousex_ < 0 ? 0:mousex_) > resolution_x_ ? resolution_x_:mousex_;
	mousey_ = (mousey_ < 0 ? 0:mousey_) > resolution_y_ ? resolution_y_:mousey_;

	return true;
}

bool InputSystem::IsKeyDown(int key_enum) {
	return (keyboard_state_[key_enum] & 0x80) != 0;
}

int InputSystem::GetMouseX(){
	return mousex_;
}

int InputSystem::GetMouseY() {
	return mousey_;
}

bool InputSystem::GetIsMouseDown(int button_num) {
	return (mouse_state_->rgbButtons[button_num] & 0x80) != 0;
}

}