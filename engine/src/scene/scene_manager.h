#ifndef SCENE_MANAGER_H_
#define SCENE_MANAGER_H_

#include "utils/matrix4x4.h"

namespace s2 {
class Camera;
class Texture2D;

class SceneManager {
public:
	SceneManager();
	~SceneManager();

	Camera * GetCamera() { return camera; }
	Texture2D * GetDepthStencilBuffer() { return ds; }
	const Matrix4x4 & GetProjectionMatrix() { return projection; }

private:
	Camera *camera;
	Texture2D *ds;
	Matrix4x4 projection;
};

}

#endif