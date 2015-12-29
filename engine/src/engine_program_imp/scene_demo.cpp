#include "engine.h"
#include "engine_program.h"

#include "graphics/renderer/all.h"

#include "asset/asset_path.h"

#include "scene/scene.h"
#include "scene/scene_manager.h"
#include "scene/camera.h"

#include "entity/transform.h"

#include <glog/logging.h>


namespace s2 {

class SceneDemo : public EngineProgram {
public:
	SceneDemo() : scene(0) {}

	virtual bool Initialize() {
		scene = new Scene();
		CHECK(scene->Initialize(ResolveTestAssetPath("scene/cornell_box.fbx")))<<scene->GetLastError();
		
		Camera *camera = Engine::GetSingleton()->GetSceneManager()->GetCamera();
		camera->SetEnabled(true);
		camera->GetTransform()->SetTranslate(-2.0f, 2.0f, -10.0f);

		return true;
	}

	virtual ~SceneDemo() {
		delete scene;
	}

	virtual s2string GetName() const override {
		return "SceneDemo";
	}

	virtual void OneFrame(float delta) {
		
	}

private:
	Scene *scene;
};

//AddBeforeMain(SceneDemo)

}