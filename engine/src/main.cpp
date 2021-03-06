#define NOGDI 
#include <Windows.h>
#undef ERROR
#include <glog/logging.h>

#include <io.h>
#include <fcntl.h>

#include "engine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	AllocConsole();

    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;

    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;

	const char dummy = 0;
	google::InitGoogleLogging(&dummy);
	google::SetLogDestination(google::INFO, ".\\log\\info.log");

	s2::Engine *engine = s2::Engine::GetSingleton();
	s2::RendererSetting renderer_setting;
	renderer_setting.enable_vsync = true;
	renderer_setting.full_screen = false;
	renderer_setting.window_width = 1024;
	renderer_setting.window_height = 768;
	
	engine->Initialize("Hello World", renderer_setting);
	engine->Run();
	engine->Shutdown();

	return 0;
}