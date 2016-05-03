#ifndef	SPRITE_SYSTEM_H_
#define SPRITE_SYSTEM_H_

#include <vector>
#include <stdint.h>


struct SpriteInstance;

namespace s2 {
class Sprite;
class GraphicBuffer;
class VertexShader;
class PixelShader;
class GraphicPipelineState;
class InputLayout;
class Sampler;
class ShaderData;
class Texture2D;

class SpriteSystem{
public:
	SpriteSystem();
	~SpriteSystem();
	
	void Register(Sprite *s);
	void Deregister(Sprite *s);

	void OneFrame(float delta);

private:
	void RenderOneBatch(SpriteInstance *instances, int count, Texture2D *texture);

private:
	std::vector<Sprite *> sprites;

	VertexShader *vs;
	PixelShader *ps;
	Sampler *sampler;
	ShaderData *ps_data;
	
	GraphicBuffer *instance_buffer;
	InputLayout *input_layout;
	
	GraphicPipelineState *pipeline_state;

	SpriteInstance *instances;
	static uint32_t kSpriteBatchSize;
};
}

#endif				//SPRITE_SYSTEM_H_