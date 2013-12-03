#ifndef ENGINE_H_
#define  ENGINE_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef ERROR

#include <string>

#include "utils/singleton.h"
#include "d3d/d3d_option.h"

struct WindowOption {
	int screen_width;
	int screen_height;
	bool full_screen;
	std::string window_name;
};

struct EngineOption {
	WindowOption window_option;
	D3DOption d3d_option;
};

class Engine : public Singleton<Engine>  {
public:
	Engine();
	
	void Init(const EngineOption &option);
	void Release();

	void Run();

	bool OneFrame(float delta);

	HINSTANCE GetInstanceHandle() { return hinstance_; }
	HWND GetWindowHandle() { return hwnd_; }
	EngineOption * GetOption() { return &option_; }

private:
	void InitWindow(const WindowOption &option);
	void ReleaseWindow();

private:
	HINSTANCE hinstance_;
	HWND hwnd_;
	EngineOption option_;
};

#endif		//ENGINE_H_