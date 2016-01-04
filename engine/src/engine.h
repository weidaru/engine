#ifndef ENGINE_H_
#define  ENGINE_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef ERROR

#include "utils/singleton.h"
#include "graphics/renderer/renderer_context.h"

namespace s2 {

class EngineProgramManager;

class InputSystem;
class EntitySystem;
class SpriteSystem;
class TextSystem;
class MaterialSystem;

class StartMenu;

class SceneManager;

class Engine : public Singleton<Engine>  {
private:
	enum EngineState {
		START_MENU,
		PROGRAM
	};

public:
	Engine();
	
	void Shutdown();
	void Initialize(const s2string &window_name, const RendererSetting &renderer_setting);

	void Run();
	void Stop();
	
	RendererContext * GetRendererContext() { return renderer_context; }
	
	HINSTANCE GetInstanceHandle() { return hinstance; }
	HWND GetWindowHandle() { return hwnd; }
	
	EngineProgramManager * GetEngineProgramManager() { return program_manager; }

	InputSystem * GetInputSystem() { return input_system; }
	EntitySystem * GetEntitySystem() { return entity_system; }
	SpriteSystem * GetSpriteSystem() { return sprite_system; }
	TextSystem * GetTextSystem() { return text_system; }
	MaterialSystem * GetMaterialSystem() { return material_system; }

	SceneManager * GetSceneManager() { return scene_manager; }

private:
	void InitWindow(const s2string &window_name, uint32_t window_width, uint32_t window_height, bool fullscreen);
	void OneFrame(float delta);

private:
	HINSTANCE hinstance;
	HWND hwnd;
	RendererContext *renderer_context;
	s2string window_name;
	
	EngineProgramManager *program_manager;
	
	InputSystem *input_system;
	EntitySystem *entity_system;
	SpriteSystem *sprite_system;
	TextSystem *text_system;
	MaterialSystem *material_system;

	SceneManager *scene_manager;
	
	StartMenu *start_menu;

	EngineState cur_state;

	//Need to be thread safe?
	bool stop;
};

}

#endif		//ENGINE_H_