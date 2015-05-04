#include "button.h"

#include "input/input_system.h"
#include "graphics/renderer/renderer_context.h"
#include "engine.h"

namespace s2 {

Button::Button() : Button(0.0f, 0.0f, 0.0f, 0.0f, 0.0f) {

}

Button::Button(float _x, float _y, float _z, float _width, float _height) 
	: inClick(false), x(_x), y(_y), z(_z), width(_width), height(_height) {
}

int Button::AddClickCallback(Callback cb) {
	if(cb) {
		click_callbacks.push_back(cb);
		return click_callbacks.size()-1;
	} else {
		return -1;
	}
}

void Button::RemoveClickCallback(unsigned int index) {
	if(index < click_callbacks.size()) {
		click_callbacks.erase(click_callbacks.begin() + index);
	}
}

void Button::OneFrame(float delta) {
	auto input_system = Engine::GetSingleton()->GetInputSystem();


	if(inClick) {
		if(input_system->IsMouseButtonDown(InputSystem::M_LEFT) == false) {
			inClick = false;
			for(auto it=click_callbacks.begin(); it!=click_callbacks.end(); it++) {
				(*it)(this);
			}
		}
	} else {
		if(input_system->IsMouseButtonDown(InputSystem::M_LEFT)) {
			float mouse_x = (float)input_system->GetMouseX();
			float mouse_y = (float)input_system->GetMouseY();

			if(mouse_x>=x && mouse_x<= (x+width) && mouse_y>=y && mouse_y<=(y+height) ) {
				inClick = true;
			}
		}
	}

}
}