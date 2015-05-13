#include "button.h"

#include "input/input_system.h"
#include "graphics/renderer/renderer_context.h"
#include "engine.h"
#include "graphics/sprite.h"
#include "graphics/text.h"

namespace s2 {

Button::Button(EntitySystem *system) : Entity(system), inClick(false), sprite(0), text(0) {
	sprite = new Sprite(this, Engine::GetSingleton()->GetSpriteSystem());
	text = new Text(this, Engine::GetSingleton()->GetTextSystem());
	text->SetClipped(true);
	SetWidth(sprite->GetWidth());
	SetHeight(sprite->GetHeight());

	SetDepth(0.1f);
}

Button::~Button() {
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

Button & Button::SetEnabled(bool new_value) {
	Entity::SetEnabled(new_value);

	sprite->SetEnabled(new_value);
	

	return *this;
}

void Button::OneFrame(float delta) {
	if(!IsEnabled()) {
		return;
	}

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

Button & Button::SetFontSize(unsigned int size) {
	text->SetFontSize(size);

	return *this;
}

unsigned int Button::GetFontSize() {
	return text->GetFontSize();
}

Button & Button::SetText(const s2string &content) {
	text->SetContent(content);

	return *this;
}

const s2string & Button::GetText() {
	return text->GetContent();
}

Button & Button::SetTextColor(const Vector4 &color) {
	text->SetColor(color);
	return *this;
}

const Vector4 & Button::GetTextColor() {
	return text->GetColor();
}

Button & Button::SetBackgroundColor(const Vector4 &color) {
	sprite->SetBackgroundColor(color);
	return *this;
}

const Vector4 & Button::GetBackgroundColor() {
	return sprite->GetBackgroundColor();
}

const Vector3 & Button::GetPosition() {
	return sprite->GetPosition();
}

Button & Button::SetPosition(float x, float y, float z) {
	sprite->SetPosition(x, y, z);
	return *this;
}

Button & Button::SetPosition(const Vector3 &vec) {
	sprite->SetPosition(vec);
	text->SetDepth(vec[2]-1e-4f);
	return *this;
}

Vector3 Button::GetAbsolutePosition() {
	return sprite->GetAbsolutePosition();
}

Button & Button::SetAbsolutePosition(float x, float y, float z) {
	sprite->SetAbsolutePosition(x, y, z);
	return *this;
}

Button & Button::SetAbsolutePosition(const Vector3 &vec) {
	sprite->SetAbsolutePosition(vec);
	return *this;
}
	
float Button::GetAbsoluteWidth() {
	return sprite->GetAbsoluteWidth();
}

Button & Button::SetAbsoluteWidth(float new_value) {
	sprite->SetAbsoluteWidth(new_value);
	text->SetAbsoluteClipperWidth(new_value);

	return *this;
}

float Button::GetWidth() {
	return sprite->GetWidth();
}

Button & Button::SetWidth(float new_value) {
	sprite->SetWidth(new_value);
	text->SetClipperWidth(new_value);

	return *this;
}

float Button::GetAbsoluteHeight() {
	return sprite->GetAbsoluteHeight();
}

Button & Button::SetAbsoluteHeight(float new_value) {
	sprite->SetAbsoluteHeight(new_value);
	text->SetAbsoluteClipperHeight(new_value);

	return *this;
}

float Button::GetHeight() {
	return sprite->GetHeight();
}

Button & Button::SetHeight(float new_value) {
	sprite->SetHeight(new_value);
	text->SetClipperHeight(new_value);

	return *this;
}

float Button::GetDepth() {
	return sprite->GetDepth();
}

Button & Button::SetDepth(float new_value) {
	sprite->SetDepth(new_value);
	text->SetDepth(new_value-1e-4f);
	
	return *this;
}



}