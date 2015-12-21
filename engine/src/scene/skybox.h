#ifndef SKYBOX_H_
#define SKYBOX_H_

#include "entity/entity.h"

namespace s2 {
class Mesh;
class TextureCube;
class GraphicBuffer;
class VertexShader;
class PixelShader;
class Sampler;
class DrawingState;
class SkyboxImage;
class InputLayout;

class Skybox : public Entity {
public:
	Skybox(EntitySystem *system, SkyboxImage *image);
	virtual ~Skybox() override;

protected:
	virtual void OneFrame(float delta) override;

private:
	Skybox(const Skybox &);
	Skybox & operator=(const Skybox &);

private:
	TextureCube *env_texture;
	Sampler *sampler;

	GraphicBuffer *vb;
	GraphicBuffer *ib;

	VertexShader *vs;
	PixelShader *ps;

	InputLayout *input_layout;

	DrawingState *drawing_state;
};

}


#endif		//SKYBOX_H_