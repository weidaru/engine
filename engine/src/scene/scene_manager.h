#ifndef SCENE_MANAGER_H_
#define SCENE_MANAGER_H_

namespace s2 {
class Camera;

class SceneManager {
public:
	SceneManager();
	~SceneManager();

	Camera * GetDefaultCamera() { return default_camera; }

private:
	Camera *default_camera;
};

}

#endif