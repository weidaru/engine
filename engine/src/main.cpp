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

	s2::Engine *engine = s2::Engine::GetSingleton();
	s2::RendererSetting renderer_setting;
	
	engine->Initialize("Hello World", renderer_setting);
	engine->Run();

	return 0;
}