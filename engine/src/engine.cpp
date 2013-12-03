#include "engine.h"

#include "input/input_system.h"
#include "d3d/d3d_resource_manager.h"

#include <glog/logging.h>

Engine::Engine() {

}

void Engine::Run() {
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	while(!done)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = OneFrame(0.1f);
			if(!result)
			{
				done = true;
			}
		}

	}

	return;
}

bool Engine::OneFrame(float delta) {
	InputSystem::GetSingleton()->OneFrame();
	return true;
}


void Engine::Init(const EngineOption &option) {
	option_ = option;
	InitWindow(option.window_option);
	InputSystem::GetSingleton()->Init();
	D3DResourceManager::GetSingleton()->Init();
}

void Engine::Release() {
	D3DResourceManager::GetSingleton()->Release();
	InputSystem::GetSingleton()->Release();

	ReleaseWindow();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
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
			return DefWindowProc(hwnd, umessage, wparam, lparam);
		}
	}
}

void Engine::InitWindow(const WindowOption &option) {
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	hinstance_ = GetModuleHandle(NULL);
	CHECK(hinstance_)<<"Cannot get instance handle.";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance_;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = option.window_name.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	ATOM result =  RegisterClassEx(&wc);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if(option.full_screen)	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)GetSystemMetrics(SM_CXSCREEN);
		dmScreenSettings.dmPelsHeight = (unsigned long)GetSystemMetrics(SM_CYSCREEN);
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		posX = posY = 0;
	}
	else
	{
		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - option.screen_width)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - option.screen_height) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	hwnd_ = CreateWindowEx(WS_EX_APPWINDOW, (LPCSTR )option.window_name.c_str(), (LPCSTR )option.window_name.c_str(), 
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUPWINDOW | WS_CAPTION,
		posX, posY, option.screen_width, option.screen_height, NULL, NULL, hinstance_, NULL);
	CHECK(hwnd_)<<"Cannot create window handle";

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(hwnd_, SW_SHOW);
	SetForegroundWindow(hwnd_);
	SetFocus(hwnd_);

	// Hide the mouse cursor.
	ShowCursor(false);
}

void Engine::ReleaseWindow() {
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if(option_.window_option.full_screen) 
		ChangeDisplaySettings(NULL, 0);

	// Remove the window.
	DestroyWindow(hwnd_);
	hwnd_ = NULL;

	// Remove the application instance.
	UnregisterClass(option_.window_option.window_name.c_str(), hinstance_);
	hinstance_ = NULL;
}
