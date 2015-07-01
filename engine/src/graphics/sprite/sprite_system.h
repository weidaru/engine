#ifndef	SPRITE_SYSTEM_H_
#define SPRITE_SYSTEM_H_

#include <vector>
#include <stdint.h>
#include <entity/component_system.h>

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

class SpriteSystem : public ComponentSystem {
public:
	SpriteSystem();
	~SpriteSystem();
	
	virtual void Register(Component *s) override;
	virtual void Deregister(Component *s) override;

	void OneFrame(float delta);

private:
	std::vector<Sprite *> sprites;

	VertexShader *vs;
	PixelShader *ps;
	Buffer *vertex_buffer, *instance_buffer;
	
	DrawingState *drawing_state;

	static uint32_t kSpriteBatchSize;
};
}

#endif				//SPRITE_SYSTEM_H_