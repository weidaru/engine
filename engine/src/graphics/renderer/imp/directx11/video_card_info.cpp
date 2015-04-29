#pragma comment(lib, "dxgi.lib")

#include "video_card_info.h"
#define WIN32_LEAN_AND_MEAN
#include <dxgi.h>
#undef ERROR
#include <glog/logging.h>

namespace s2 {

void VideoCardInfo::GetProperAdapter(unsigned int screen_width, unsigned int screen_height,  AdapterInfo *info) {
	HRESULT result = 1;

	IDXGIFactory* factory = 0;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	CHECK(!FAILED(result))<<"Cannot create DXGI factory. Error " << GetLastError();

	IDXGIAdapter* adapter = 0;
	result = factory->EnumAdapters(0, &adapter);
	CHECK(!FAILED(result))<<"Cannot create an adapter to video card. Error " << GetLastError();

	IDXGIOutput* adapter_output = 0;
	result = adapter->EnumOutputs(0, &adapter_output);
	CHECK(!FAILED(result))<<"Cannot enumerate the primary adapter output (monitor) Error " << GetLastError();

	DXGI_MODE_DESC* display_mode_list = 0;
	unsigned int mode_count = 0;
	result = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &mode_count, NULL);
	CHECK(!FAILED(result))<<"Cannot get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor). Error " << GetLastError();
	display_mode_list =new DXGI_MODE_DESC[mode_count];
	result = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &mode_count, display_mode_list);
	CHECK(!FAILED(result))<<"Cannot get the modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor). Error " << GetLastError();

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	result = 0;
	for(unsigned int i=0; i<mode_count; i++) {
		if(display_mode_list[i].Width == screen_width &&
		   display_mode_list[i].Height == screen_height) {
				info->refresh_numerator = display_mode_list[i].RefreshRate.Numerator;
				info->refresh_denominator= display_mode_list[i].RefreshRate.Denominator;
				result = 1;
				break;
		}
	}
	CHECK(!FAILED(result))<<"Cannot find the mode with width "<<screen_width
									   <<"and height "<<screen_height;

	DXGI_ADAPTER_DESC adapterDesc;
	result = adapter->GetDesc(&adapterDesc);
	CHECK(!FAILED(result))<<"Cannot get adapter description";

	info->memory_size =  (unsigned int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024); 
	{
		size_t dummy = 0;
		CHECK(!wcstombs_s(&dummy, info->name, sizeof(info->name), adapterDesc.Description, sizeof(info->name)));
	}
	
	delete[] display_mode_list;
	adapter_output->Release();
	adapter->Release();
	factory->Release();
}


}