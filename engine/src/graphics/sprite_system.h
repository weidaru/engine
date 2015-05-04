#ifndef	SPRITE_SYSTEM_H_
#define SPRITE_SYSTEM_H_

#include "utils/singleton.h"

#include <vector>


//[[TypeInfo]]//
struct SpriteVertex {
	float position[3];
};

namespace s2 {
class Sprite;
class Buffer;
class VertexShader;
class PixelShader;
class DrawingState;

class SpriteSystem {
public:
	SpriteSystem();
	~SpriteSystem();
	
	void RegisterSprite(Sprite *s);
	void DeregisterSprite(Sprite *s);

	void OneFrame(float delta);

private:
	std::vector<Sprite *> sprites;

	VertexShader *vs;
	PixelShader *ps;
	Buffer *vertex_buffer, *instance_buffer;
	
	DrawingState *drawing_state;

	static unsigned int kSpriteBatchSize;
};
}

#endif				//SPRITE_SYSTEM_H_