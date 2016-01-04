#include "engine.h"
#include "engine_program.h"

#include "graphics/renderer/all.h"
#include "asset/asset_path.h"
#include "asset/skybox_image.h"
#include "scene/skybox.h"
#include "scene/camera.h"
#include "scene/scene_manager.h"
#include "input/input_system.h"

#include <glog/logging.h>


namespace s2 {

class SkyboxDemo : public EngineProgram {
public:
	SkyboxDemo() : skybox(0) {}

	virtual bool Initialize() override {
		SkyboxImage image;
		CHECK(image.Initialize(ResolveTestAssetPath("image/skybox.png"), ImagePixelFormat::R8G8B8A8))<<image.GetLastError();

		skybox = new Skybox(&entity_system, &image);

		return true;
	}

	virtual ~SkyboxDemo() {
	}

	virtual s2string GetName() const override {
		return "SkyboxDemo";
	}

	virtual void OnEnter() override {
		Camera *camera = Engine::GetSingleton()->GetSceneManager()->GetCamera();
		camera->SetEnabled(true);

		Engine::GetSingleton()->GetInputSystem()->SetMouseVisible(false);
	}

	virtual void OnLeave() override {
		Camera *camera = Engine::GetSingleton()->GetSceneManager()->GetCamera();
		camera->SetEnabled(false);

	}

	virtual void OneFrame(float delta) override {

	}

private:
	Skybox *skybox;
};

AddBeforeMain(SkyboxDemo)

}