#include "button.h"

#include "input/input_system.h"
#include "graphics/renderer/renderer_context.h"
#include "engine.h"
#include "graphics/sprite.h"
#include "graphics/sprite_system.h"

namespace s2 {

Button::Button() : inClick(false) {
	sprite = new Sprite();
	AddComponent("sprite", sprite);
	Engine::GetSingleton()->GetSpriteSystem()->RegisterSprite(sprite);
}

Button::~Button() {
	Engine::GetSingleton()->GetSpriteSystem()->DeregisterSprite(sprite);
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
			Vector3 pos = sprite->GetAbsolutePosition();
			float x = pos[0], y = pos[1];
			float width = sprite->GetAbsoluteWidth(), height = sprite->GetAbsoluteHeight();

			if(mouse_x>=(x-width/2.0f) && mouse_x<= (x+width/2.0f) && mouse_y>=(y-height/2.0f) && mouse_y<=(y+height/2.0f) ) {
				inClick = true;
			}
		}
	}
}
}