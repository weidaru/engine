#ifndef	SPRITE_SYSTEM_H_
#define SPRITE_SYSTEM_H_

#include <vector>
#include <stdint.h>

//[[TypeInfo]]//
struct SpriteVertex {
	float position[3];
};

namespace s2 {
class Sprite;
class GraphicBuffer;
class VertexShader;
class PixelShader;
class DrawingState;

class SpriteSystem{
public:
	SpriteSystem();
	~SpriteSystem();
	
	void Register(Sprite *s);
	void Deregister(Sprite *s);

	void OneFrame(float delta);

private:
	std::vector<Sprite *> sprites;

	VertexShader *vs;
	PixelShader *ps;
	GraphicBuffer *vertex_buffer, *instance_buffer;
	
	DrawingState *drawing_state;

	static uint32_t kSpriteBatchSize;
};
}

#endif				//SPRITE_SYSTEM_H_