#ifndef SKYBOX_H_
#define SKYBOX_H_

#include "entity/entity.h"

namespace s2 {
class Model;
class TextureCube;

class Skybox : public Entity {
public:
	Skybox(EntitySystem *system, SkyboxImage *image);
	virtual ~Skybox() override;

protected:
	virtual void OneFrame(float delta) override;

private:
	//TODO: Good to warp the model as an component, something for later.
	Model *model;
	TextureCube *env_texture;
};

}


#endif		//SKYBOX_H_