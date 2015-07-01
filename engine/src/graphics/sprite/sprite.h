#ifndef SCREEN_SPACE_TEXTURE_H_
#define SCREEN_SPACE_TEXTURE_H_

#include "entity/component.h"
#include "graphics/renderer/texture2d.h"
#include "utils/matrix4x4.h"
#include "utils/vector4.h"
#include "utils/vector3.h"

#include <stdint.h>

//[[TypeInfo]]//
struct SpriteInstance {
	Matrix4x4 transform;
	float color[4];
};

namespace s2 {

/**
 * Default as [width, height] = [0.2, 0.2], pos = [0.0f, 0.0f, 0.0f] in NDC space.
 */
class Sprite: public Component {
public:
	Sprite(Entity *entity);
	virtual ~Sprite() override;

	Sprite & SetBackgroundColor(const Vector4 &color);
	const Vector4 & GetBackgroundColor() { return bg_color; }

	const Vector3 & GetPosition();
	Sprite & SetPosition(float x, float y, float z);
	Sprite & SetPosition(const Vector3 &vec);

	Vector3 GetAbsolutePosition();
	Sprite & SetAbsolutePosition(float x, float y, float z);
	Sprite & SetAbsolutePosition(const Vector3 &vec);
	
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

private:
	Vector4 bg_color;

	SpriteInstance data;
};

}


#endif			//SCREEN_SPACE_TEXTURE_H_