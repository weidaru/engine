#include "engine.h"
#include "engine_program.h"

#include "graphics/renderer/all.h"
#include "asset/asset_path.h"
#include "asset/skybox_image.h"
#include "scene/skybox.h"
#include <glog/logging.h>


namespace s2 {

class SkyboxDemo : public EngineProgram {
public:
	SkyboxDemo() : skybox(0) {}

	virtual bool Initialize() {
		SkyboxImage image;
		CHECK(image.Initialize(ResolveTestAssetPath("image/skybox.png"), ImagePixelFormat::R8G8B8A8))<<image.GetLastError();

		skybox = new Skybox(Engine::GetSingleton()->GetEntitySystem(), &image);
		skybox->SetEnabled(false);

		return true;
	}

	virtual ~SkyboxDemo() {
		delete skybox;
	}

	virtual s2string GetName() const {
		return "SkyboxDemo";
	}

	virtual void OneFrame(float delta) {
		skybox->SetEnabled(true);
	}

private:
	Skybox *skybox;
};

//AddBeforeMain(SkyboxDemo)

}