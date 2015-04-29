#include "sprite.h"
#include "engine.h"

#include "entity/entity.h"
#include "entity/transform.h"
#include "graphics/renderer/renderer_context.h"

namespace s2 {

Sprite::Sprite() {
	SetWidth(0.2f);
	SetHeight(0.2f);
	SetDepth(-1.0f);

}


Sprite & Sprite::SetBackgroundColor(const Vector4 &color) {
	bg_color = color; 
	return *this; 
}

const Vector3 & Sprite::GetPosition() {
	return GetEntity()->GetTransform().GetTranslate();
}

Sprite & Sprite::SetPosition(float x, float y, float z) {
	GetEntity()->GetTransform().SetTranslate(x, y, z);

	return *this;
}

Sprite & Sprite::SetPosition(const Vector3 &vec) {
	return SetPosition(vec[0], vec[1], vec[2]);
}

const Vector3 & Sprite::GetAbsolutePosition() {
	RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_width = setting.window_width, w_height = setting.window_height;
	const Vector3 &pos =  GetPosition();
	
	float x = (pos[0]+1.0f)*w_width/2.0f;
	float y = (1.0f-pos[1])*w_height/2.0f;
	float z = pos[2];

	return Vector3(x, y, z);
}

Sprite & Sprite::SetAbsolutePosition(float x, float y, float z) {
	auto setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_width = setting.window_width, w_height = setting.window_height;

	float s_x = x/w_width*2+1;
	float s_y = 1-y/w_height*2;
	float s_z = z;
	return SetPosition(s_z, s_y, s_z);
}

Sprite & Sprite::SetAbsolutePosition(const Vector3 &vector) {
	return SetAbsolutePosition(vector[0], vector[1], vector[2]);
}

float Sprite::GetAbsoluteWidth() {
	RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_width = setting.window_width;
	
	return GetWidth()*w_width/2.0f;
}

Sprite & Sprite::SetAbsoluteWidth(float new_value) {
	RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_width = setting.window_width;

	return SetWidth(new_value/w_width*2.0f);
}

float Sprite::GetWidth() {
	const Vector3& scale = GetEntity()->GetTransform().GetScale();
	return scale[0]*0.2f;
}

Sprite & Sprite::SetWidth(float new_value) {
	GetEntity()->GetTransform().Scale(new_value/0.2f, 0.0f, 0.0f);

	return *this; 
}

float Sprite::GetAbsoluteHeight() {
	RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_height = setting.window_height;

	return GetHeight()*w_height/2.0f;
}

Sprite & Sprite::SetAbsoluteHeight(float new_value) {
	RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_height = setting.window_height;

	return SetHeight(new_value/w_height*2.0f);
}

float Sprite::GetHeight() {
	const Vector3& scale = GetEntity()->GetTransform().GetScale();

	return scale[1]*0.2f;
}

Sprite & Sprite::SetHeight(float new_value) {
	GetEntity()->GetTransform().Scale(0.0f, new_value/0.1f, 0.0f);
	return *this;
}

float Sprite::GetDepth() {
	return GetEntity()->GetTransform().GetTranslate()[2];
}

Sprite & Sprite::SetDepth(float new_value) {
	Vector3 pos = GetPosition();
	pos[2] = new_value;

	SetPosition(pos);
}

namespace {

inline void FillArrayWithVector4(float *dest, const Vector4 &source) {
	for(unsigned int i=0; i<4; i++) {
		dest[i] = source[i];
	}
}

}

SpriteInstance * Sprite::GetData() {
	FillArrayWithVector4(data.color, bg_color);
	data.transform = GetEntity()->GetCascadeTransformMatrix();
	
	return &data;
}



}

