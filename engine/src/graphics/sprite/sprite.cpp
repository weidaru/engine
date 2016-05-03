#include "sprite.h"
#include "engine.h"

#include "entity/entity.h"
#include "entity/transform.h"
#include "graphics/renderer/renderer_context.h"

#include <glog/logging.h>

namespace s2 {

Sprite::Sprite(Entity *entity) : Component(entity), texture(0) {
	SetBackgroundColor(S2Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	texCoordinates[0][0] = 0.0;
	texCoordinates[0][1] = 0.0;
	texCoordinates[1][0] = 1.0;
	texCoordinates[1][1] = 0.0;
	texCoordinates[2][0] = 0.0;
	texCoordinates[2][1] = 1.0;
	texCoordinates[3][0] = 1.0;
	texCoordinates[3][1] = 1.0;
}

Sprite::~Sprite() {

}


Sprite & Sprite::SetBackgroundColor(const S2Vector4 &color) {
	bg_color = color; 
	return *this; 
}

const S2Vector3 & Sprite::GetPosition() {
	return GetEntity()->GetTransform()->GetTranslate();
}

Sprite & Sprite::SetPosition(float x, float y, float z) {
	GetEntity()->GetTransform()->SetTranslate(x, y, z);

	return *this;
}

Sprite & Sprite::SetPosition(const S2Vector3 &vec) {
	return SetPosition(vec[0], vec[1], vec[2]);
}

S2Vector3 Sprite::GetAbsolutePosition() {
	RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_width = (float)setting.window_width, w_height = (float)setting.window_height;
	const S2Vector3 &pos =  GetPosition();
	
	float x = (pos[0]+1.0f)*w_width/2.0f;
	float y = (1.0f-pos[1])*w_height/2.0f;
	float z = pos[2];

	return S2Vector3(x, y, z);
}

Sprite & Sprite::SetAbsolutePosition(float x, float y, float z) {
	auto setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_width = (float)setting.window_width, w_height = (float)setting.window_height;

	float s_x = x/w_width*2-1;
	float s_y = 1-y/w_height*2;
	float s_z = z;
	return SetPosition(s_x, s_y, s_z);
}

Sprite & Sprite::SetAbsolutePosition(const S2Vector3 &vector) {
	return SetAbsolutePosition(vector[0], vector[1], vector[2]);
}

float Sprite::GetAbsoluteWidth() {
	RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_width = (float)setting.window_width;
	
	return GetWidth()*w_width/2.0f;
}

Sprite & Sprite::SetAbsoluteWidth(float new_value) {
	RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_width = (float)setting.window_width;

	return SetWidth(new_value/w_width*2.0f);
}

float Sprite::GetWidth() {
	const S2Vector3& scale = GetEntity()->GetTransform()->GetScale();
	return scale[0]*0.2f;
}

Sprite & Sprite::SetWidth(float new_value) {
	CHECK_GE(new_value, 0.0f);

	S2Vector3 scale = GetEntity()->GetTransform()->GetScale();
	scale[0] = new_value/0.2f; 
	GetEntity()->GetTransform()->SetScale(scale);

	return *this; 
}

float Sprite::GetAbsoluteHeight() {
	RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_height = (float)setting.window_height;

	return GetHeight()*w_height/2.0f;
}

Sprite & Sprite::SetAbsoluteHeight(float new_value) {
	RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_height = (float)setting.window_height;

	return SetHeight(new_value/w_height*2.0f);
}

float Sprite::GetHeight() {
	const S2Vector3& scale = GetEntity()->GetTransform()->GetScale();

	return scale[1]*0.2f;
}

Sprite & Sprite::SetHeight(float new_value) {
	CHECK_GE(new_value, 0.0f);
	S2Vector3 scale = GetEntity()->GetTransform()->GetScale();
	scale[1] = new_value/0.2f; 
	GetEntity()->GetTransform()->SetScale(scale);
	return *this;
}

float Sprite::GetDepth() {
	return GetEntity()->GetTransform()->GetTranslate()[2];
}

Sprite & Sprite::SetDepth(float new_value) {
	S2Vector3 pos = GetPosition();
	pos[2] = new_value;

	return SetPosition(pos);
}

std::pair<float, float> Sprite::GetTexCoordinates(int index) {
	return std::make_pair(texCoordinates[4][0], texCoordinates[4][1]);
}

Sprite & Sprite::SetTexCoordinates(int index, float u, float v) {
	texCoordinates[index][0] = u;
	texCoordinates[index][1] = v;
	return *this;
}
	
Texture2D * Sprite::GetTexture() {
	return texture;
}

Sprite & Sprite::SetTexture(Texture2D * texture_) {
	this->texture = texture_;
	if(texture != 0) {
		CHECK(texture->AsShaderResource())<<"Input texture must be bind as ShaderResourceView";
	}
	return *this;
}

namespace {

inline void FillArrayWithVector4(float *dest, const S2Vector4 &source) {
	for(uint32_t i=0; i<4; i++) {
		dest[i] = source[i];
	}
}
}

SpriteInstance * Sprite::GetData() {
	FillArrayWithVector4(data.color, bg_color);
	data.transform = GetEntity()->GetCascadeTransformMatrix();
	
	memcpy(data.tex, texCoordinates, sizeof(float)*8);
	
	return &data;
}



}


