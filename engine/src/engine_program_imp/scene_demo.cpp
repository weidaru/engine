#include "engine.h"
#include "engine_program.h"

#include "graphics/renderer/all.h"

#include "asset/asset_path.h"

#include "scene/scene.h"
#include "scene/scene_manager.h"
#include "scene/camera.h"

#include "entity/transform.h"

#include "input/input_system.h"

#include <glog/logging.h>


namespace s2 {

class SceneDemo : public EngineProgram {
public:
	SceneDemo() : scene(0), camera(0) {}

	virtual bool Initialize() override {
		scene = new Scene(&entity_system);
		CHECK(scene->Initialize(ResolveTestAssetPath("scene/cornell_box.fbx")))<<scene->GetLastError();

		return true;
	}

	virtual ~SceneDemo() {
		delete scene;
	}

	virtual s2string GetName() const override {
		return "SceneDemo";
	}

	virtual void OnEnter() override {
		Camera *camera = Engine::GetSingleton()->GetSceneManager()->GetCamera();
		camera->SetEnabled(true);
		camera->GetTransform()->SetTranslate(-2.0f, 2.0f, -10.0f);

		Engine::GetSingleton()->GetInputSystem()->SetMouseVisible(false);
	}

	virtual void OnLeave() override {
		Camera *camera = Engine::GetSingleton()->GetSceneManager()->GetCamera();
		camera->SetEnabled(false);
	}

	virtual void OneFrame(float delta) override {
	}

private:
	Camera *camera;
	Scene *scene;
};

AddBeforeMain(SceneDemo)

}