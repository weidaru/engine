#ifndef SKYBOX_H_
#define SKYBOX_H_

#include "entity/entity.h"

namespace s2 {
class Model;
class TextureCube;
class Buffer;
class VertexShader;
class PixelShader;
class Sampler;
class DrawingState;
class SkyboxImage;

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
	//TODO: Good to warp the model as an component, something for later.
	Model *model;
	TextureCube *env_texture;
	Sampler *sampler;

	Buffer *vb;
	Buffer *ib;

	VertexShader *vs;
	PixelShader *ps;

	DrawingState *drawing_state;
};

}


#endif		//SKYBOX_H_