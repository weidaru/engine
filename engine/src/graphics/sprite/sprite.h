#ifndef SCREEN_SPACE_TEXTURE_H_
#define SCREEN_SPACE_TEXTURE_H_

#include "entity/component.h"
#include "graphics/renderer/texture2d.h"
#include "utils/s2matrix4x4.h"
#include "utils/s2vector4.h"
#include "utils/s2vector3.h"

#include <stdint.h>

//[[TypeInfo]]//
struct SpriteInstance {
	S2Matrix4x4 transform;
	float color[4];
	float tex[4][2];
};

namespace s2 {

/**
 * Default as [width, height] = [0.2, 0.2], pos = [0.0f, 0.0f, 0.0f] in NDC space.
 */
class Sprite: public Component {
public:
	Sprite(Entity *entity);
	virtual ~Sprite() override;

	Sprite & SetBackgroundColor(const S2Vector4 &color);
	const S2Vector4 & GetBackgroundColor() { return bg_color; }

	const S2Vector3 & GetPosition();
	Sprite & SetPosition(float x, float y, float z);
	Sprite & SetPosition(const S2Vector3 &vec);

	S2Vector3 GetAbsolutePosition();
	Sprite & SetAbsolutePosition(float x, float y, float z);
	Sprite & SetAbsolutePosition(const S2Vector3 &vec);
	
	float GetAbsoluteWidth();
	Sprite & SetAbsoluteWidth(float new_value);

	float GetWidth();
	Sprite & SetWidth(float new_value);

	float GetAbsoluteHeight();
	Sprite & SetAbsoluteHeight(float new_value);

	float GetHeight();
	Sprite & SetHeight(float new_value);

	float GetDepth();
	Sprite & SetDepth(float new_value);

	SpriteInstance *GetData();
	uint32_t GetDataSize() { return 4; }

	//[bottom_left, bottom_right, top_left, top_right]
	std::pair<float, float> GetTexCoordinates(int index);
	Sprite & SetTexCoordinates(int index, float u, float v);
	
	Texture2D * GetTexture();
	Sprite & SetTexture(Texture2D * texture);

private:
	S2Vector4 bg_color;
	Texture2D *texture;
	float texCoordinates[4][2];

	SpriteInstance data;
};

}


#endif			//SCREEN_SPACE_TEXTURE_H_