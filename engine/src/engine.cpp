#include "engine.h"

#include "start_menu.h"
#include "graphics/renderer/imp/directx11/d3d11_context.h"
#include "engine_program.h"
#include "input/input_system.h"
#include "entity/entity_system.h"
#include "entity/entity.h"
#include "graphics/text/text_system.h"
#include "graphics/sprite/sprite_system.h"
#include "scene/scene_manager.h"
#include "graphics/material/material_system.h"

#include <glog/logging.h>

LRESULT CALLBACK WndProc(HWND _hwnd, UINT umessage, WPARAM wparam, LPARAM lparam){
	switch(umessage) {
	case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		}

	case WM_CLOSE: {
			PostQuitMessage(0);
			return 0;
		}
	default: {
			return DefWindowProc(_hwnd, umessage, wparam, lparam);
		}
	}
}

static const char * PISSED_STR = "If someone name it like this, I will be pissed.";

namespace s2 {

Engine::Engine()
	: 	hinstance(0), hwnd(0), renderer_context(0), window_name(PISSED_STR),
		program_manager(new EngineProgramManager),
		input_system(0), entity_system(0), sprite_system(0), text_system(0),
		material_system(0), scene_manager(0),
		start_menu(0),
		cur_state(START_MENU),
		stop(false){

}

void Engine::Run() {
	MSG msg;
	bool done;

	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	float interval = 0.0f;
	LARGE_INTEGER frequency;			// ticks per second
	LARGE_INTEGER t1, t2;				// ticks
	QueryPerformanceFrequency(&frequency);

	while(!done) {
		// start timer
		QueryPerformanceCounter(&t1);

		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if(msg.message == WM_QUIT || stop == true) {
				done = true;
			}
		}

		if(!done) {
			OneFrame(interval);
		}
		// stop timer
		QueryPerformanceCounter(&t2);
		interval = (float)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart;
	}
}

void Engine::OneFrame(float delta) {
	GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
	GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
	Texture2D *bf = renderer_context->GetBackBuffer();

	float black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	pipeline->ClearRenderTarget(bf->AsRenderTarget(), black);
	pipeline->ClearDepthStencil(scene_manager->GetDepthStencilBuffer()->AsDepthStencil(), true, 1.0f, 0.0f, 0);

	input_system->OneFrame(delta);

	if(cur_state == START_MENU) {
		start_menu->OneFrame(delta);

		if(input_system->IsKeyDown(InputSystem::K_SPACE) || input_system->IsKeyDown(InputSystem::K_ENTER)) {
			start_menu->OnLeave();

			EngineProgram *cur_program = start_menu->GetProgram(start_menu->GetSelectedIndex());
			cur_program->RestoreEntityEnabledState();
			cur_program->OnEnter();
			cur_state = PROGRAM;
		}
	} else if(cur_state == PROGRAM) {
		EngineProgram *cur_program = start_menu->GetProgram(start_menu->GetSelectedIndex());
		cur_program->GetEntitySystem()->OneFrame(delta);
		cur_program->OneFrame(delta);
		if(input_system->IsKeyDown(InputSystem::K_ESCAPE)) {
			cur_program->OnLeave();
			cur_program->SaveEntityEnabledState();
			std::vector<Entity *> entities;
			cur_program->GetEntitySystem()->GetAll(&entities);
			for(uint32_t i=0; i<entities.size(); i++) {
				entities[i]->SetEnabled(false);
			}

			cur_state = START_MENU;
			start_menu->OnEnter();
		}
	}

	material_system->OneFrame(delta);
	entity_system->OneFrame(delta);
	sprite_system->OneFrame(delta);
	text_system->OneFrame(delta);
	renderer_context->SwapBuffer();
}

void Engine::Stop() {
	stop = true;
}

void Engine::Initialize(const s2string &_window_name, const RendererSetting &renderer_setting) {
	InitWindow(_window_name, renderer_setting.window_width, renderer_setting.window_height, renderer_setting.full_screen);
	window_name = _window_name;
	D3D11Context * context = new D3D11Context();
	renderer_context = context;
	context->SetSetting(renderer_setting);
	context->Initialize(hwnd);

	input_system = new InputSystem(hwnd);
	entity_system = new EntitySystem();
	sprite_system = new SpriteSystem();
	scene_manager = new SceneManager();
	text_system = new TextSystem();
	material_system = new MaterialSystem(renderer_context->GetResourceManager());

	std::vector<EngineProgram *> programs;
	program_manager->GetAll(&programs);
	for(uint32_t i=0; i<programs.size(); i++) {
		EngineProgram *cur_program = programs[i];
		cur_program->Initialize();
		cur_program->SaveEntityEnabledState();
		std::vector<Entity *> entities;
			cur_program->GetEntitySystem()->GetAll(&entities);
			for(uint32_t i=0; i<entities.size(); i++) {
				entities[i]->SetEnabled(false);
			}
	}

	start_menu = new StartMenu(program_manager);
}

void Engine::Shutdown() {
	if(window_name == PISSED_STR)
		return;

	delete program_manager;

	delete scene_manager;
	delete entity_system;

	delete material_system;
	delete text_system;
	delete sprite_system;

	delete input_system;

	RendererSetting renderer_setting = renderer_context->GetSetting();
	delete renderer_context;

	// Fix the display settings if leaving full screen mode.
	if(renderer_setting.full_screen)
		ChangeDisplaySettings(NULL, 0);

	// Remove the window.
	DestroyWindow(hwnd);
	hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(window_name.c_str(), hinstance);
	hinstance = NULL;
}

void Engine::InitWindow(const s2string &window_name, uint32_t window_width, uint32_t window_height, bool fullscreen) {
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	hinstance = GetModuleHandle(NULL);
	CHECK(hinstance)<<"Cannot get instance handle.";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = window_name.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	ATOM result =  RegisterClassEx(&wc);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if(fullscreen)	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)GetSystemMetrics(SM_CXSCREEN);
		dmScreenSettings.dmPelsHeight = (unsigned long)GetSystemMetrics(SM_CYSCREEN);
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		posX = posY = 0;
	} else {
		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - window_width)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - window_height) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, (LPCSTR )window_name.c_str(), (LPCSTR )window_name.c_str(),
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP ,
		posX, posY, window_width, window_height, NULL, NULL, hinstance, NULL);
	CHECK(hwnd)<<"Cannot create window handle";

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	RECT clip;
	GetWindowRect(hwnd, &clip);
	ClipCursor(&clip);
}

}
