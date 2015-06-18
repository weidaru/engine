#include "scene_manager.h"
#include "camera.h"
#include "engine.h"

#include "graphics/renderer/all.h"

namespace s2 {

SceneManager::SceneManager() : camera(0), ds(0) {
	camera = new Camera(Engine::GetSingleton()->GetEntitySystem());

	GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
	const RendererSetting &renderer_setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	Texture2D::Option ds_option;
	Texture2D::Option::SetAsDepthStencilBuffer(&ds_option, renderer_setting.window_width, renderer_setting.window_height);
	ds = manager->CreateTexture2D();
	ds->Initialize(ds_option);

	float np=0.5f, fp =1000.0f;
	float aspect=((float)renderer_setting.window_width)/((float)renderer_setting.window_height);
	float fov=3.1415926f*35/180;
	projection.SetProjection(aspect, fov, np, fp);
}

SceneManager::~SceneManager() {
	GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();

	manager->RemoveTexture2D(ds->GetID());
	delete camera;
}

}