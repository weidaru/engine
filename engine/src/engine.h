#ifndef ENGINE_H_
#define  ENGINE_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef ERROR

#include "utils/singleton.h"
#include "renderer/renderer_context.h"

namespace s2 {

class InputSystem;

class Engine : public Singleton<Engine>  {
public:
	Engine();
	~Engine();
	
	void Initialize(const s2string &window_name, const RendererSetting &renderer_setting);

	void Run();
	bool OneFrame(float delta);
	
	RendererContext * GetRendererContext() { return renderer_context; }
	InputSystem * GetInputSystem() { return input_system; }
	
	HINSTANCE GetInstanceHandle() { return hinstance; }
	HWND GetWindowHandle() { return hwnd; }

private:
	void InitWindow(const s2string &window_name, unsigned int window_width, unsigned int window_height, bool fullscreen);

private:
	HINSTANCE hinstance;
	HWND hwnd;
	RendererContext *renderer_context;
	InputSystem *input_system;
	s2string window_name;
};

}

#endif		//ENGINE_H_