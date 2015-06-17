#include "scene_manager.h"
#include "camera.h"
#include "engine.h"

namespace s2 {

SceneManager::SceneManager() : default_camera(0) {
	default_camera = new Camera(Engine::GetSingleton()->GetEntitySystem());
}

SceneManager::~SceneManager() {
	delete default_camera;
}

}