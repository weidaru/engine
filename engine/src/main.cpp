#define NOGDI 
#include <Windows.h>
#undef ERROR
#include <glog/logging.h>

#include "engine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	const char dummy = 0;
	google::InitGoogleLogging(&dummy);
	google::SetLogDestination(google::INFO, ".\\log\\info.log");
	google::SetLogDestination(google::WARNING, ".\\log\\warning.log");
	google::SetLogDestination(google::ERROR, ".\\log\\error.log");
	google::SetLogDestination(google::FATAL, ".\\log\\fatal.log");

	Engine *loop = Engine::GetSingleton();
	EngineOption option;
	option.window_option.full_screen = false;
	option.window_option.screen_width = 800;
	option.window_option.screen_height = 600;
	option.window_option.window_name = "engine";
	loop->Init(option);
	loop->Run();
	loop->Release();

	return 0;
}