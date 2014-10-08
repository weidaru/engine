#include "engine.h"

#include "input/input_system.h"
#include "renderer/imp/directx11/d3d11_context.h"

#include <glog/logging.h>

LRESULT CALLBACK WndProc(HWND _hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

	case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

	default:
		{
			return DefWindowProc(_hwnd, umessage, wparam, lparam);
		}
	}
}

static const char * PISSED_STR = "If someone name it like this, I will be pissed.";

namespace s2 {

Engine::Engine() 
	: hinstance(0), hwnd(0), renderer_context(0), input_system(0), window_name(PISSED_STR) {

}

Engine::~Engine() {
	if(window_name == PISSED_STR)
		return;
	delete input_system;

	RendererSetting renderer_setting;
	renderer_context->GetSetting(&renderer_setting);
	delete renderer_context;

	// Show the mouse cursor.
	ShowCursor(true);

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

void Engine::Run() {
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	while(!done) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT) {
			done = true;
		} else {
			result = OneFrame(0.1f);
			if(!result) {
				done = true;
			}
		}

	}

	return;
}

bool Engine::OneFrame(float delta) {
	
	return true;
}


void Engine::Initialize(const s2string &window_name, const RendererSetting &renderer_setting) {
	InitWindow(window_name, renderer_setting.window_width, renderer_setting.window_height, renderer_setting.full_screen);
	D3D11Context * context = new D3D11Context();
	renderer_context = context;

	context->Initialize(hwnd);
	input_system = new InputSystem();
	input_system->Initialize();
}

void Engine::InitWindow(const s2string &window_name, unsigned int window_width, unsigned int window_height, bool fullscreen) {
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
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUPWINDOW | WS_CAPTION,
		posX, posY, window_width, window_height, NULL, NULL, hinstance, NULL);
	CHECK(hwnd)<<"Cannot create window handle";

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	
	// Hide the mouse cursor.
	ShowCursor(false);
}

}

