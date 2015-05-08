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

class Engine : public Singleton<Engine>  {
public:
	Engine();
	
	void Shutdown();
	void Initialize(const s2string &window_name, const RendererSetting &renderer_setting);

	void Run();
	void Stop();
	void OneFrame(float delta);
	
	RendererContext * GetRendererContext() { return renderer_context; }
	
	HINSTANCE GetInstanceHandle() { return hinstance; }
	HWND GetWindowHandle() { return hwnd; }
	
	EngineProgramManager * GetEngineProgramManager() { return program_manager; }
	InputSystem * GetInputSystem() { return input_system; }
	EntitySystem * GetEntitySystem() { return entity_system; }
	SpriteSystem * GetSpriteSystem() { return sprite_system; }

private:
	void InitWindow(const s2string &window_name, unsigned int window_width, unsigned int window_height, bool fullscreen);

private:
	HINSTANCE hinstance;
	HWND hwnd;
	RendererContext *renderer_context;
	s2string window_name;
	
	EngineProgramManager *program_manager;
	InputSystem *input_system;
	EntitySystem *entity_system;
	SpriteSystem *sprite_system;

	//Need to be thread safe?
	bool stop;
};

}

#endif		//ENGINE_H_