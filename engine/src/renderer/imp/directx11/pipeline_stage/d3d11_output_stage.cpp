#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_output_stage.h"

#include "renderer/imp/directx11/d3d11_graphic_resource_manager.h"
#include "renderer/imp/directx11/d3d11_context.h"
#include "renderer/imp/directx11/d3d11_texture2d.h"

#ifdef NDEBUG
	#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
	#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif

namespace s2 {

D3D11OutputStage::D3D11OutputStage(D3D11GraphicResourceManager *_manager) 
		: manager(_manager){
	Clear();
}

D3D11OutputStage::~D3D11OutputStage() {
	Clear();
}

void D3D11OutputStage::Clear() {
	new_output = true;
	rts.clear();
	rts.resize(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
}

void D3D11OutputStage::SetRenderTarget(unsigned int index, Texture2D *target) {
	new_output = true;
	rts[index].tex = NiceCast(D3D11Texture2D *, target);
}

Resource * D3D11OutputStage::GetRenderTarget(unsigned int index) {
	return rts[index].tex;
}

void D3D11OutputStage::SetDepthStencilBuffer(Texture2D *buffer) {
	new_output = true;
	ds.tex = NiceCast(D3D11Texture2D *, buffer);
}

Resource* D3D11OutputStage::GetDepthStencilBuffer() {
	return ds.tex;
}

void D3D11OutputStage::SetRenderTargetClearOption(unsigned int index, bool enable, const float rgba[4]) {
	rts[index].enable_clear = enable;
	rts[index].rgba[0] = rgba[0];
	rts[index].rgba[1] = rgba[1];
	rts[index].rgba[2] = rgba[2];
	rts[index].rgba[3] = rgba[3];
}

void D3D11OutputStage::GetRenderTargetClearOption(unsigned int index, bool *enable, float *rgba) {
	*enable = rts[index].enable_clear;
	rgba[0] = rts[index].rgba[0];
	rgba[1] = rts[index].rgba[1];
	rgba[2] = rts[index].rgba[2];
	rgba[3] = rts[index].rgba[3];
}

void D3D11OutputStage::SetDepthStencilBufferClearOption(bool enable_depth_clear, bool enable_stencil_clear,  float depth, uint8_t stencil) {
	ds.enable_depth_clear = enable_depth_clear;
	ds.enable_stencil_clear = enable_stencil_clear;
	ds.depth = depth;
	ds.stencil = stencil;
}

void D3D11OutputStage::GetDepthStencilBufferClearOption(bool *enable_depth_clear, bool *enable_stencil_clear,  float *depth, uint8_t *stencil) {
	*enable_depth_clear = ds.enable_depth_clear;
	*enable_stencil_clear = ds.enable_stencil_clear;
	*depth = ds.depth;
	*stencil = ds.stencil;
}

void D3D11OutputStage::Setup() {
	if(new_output)
		SetOutput();
}

void D3D11OutputStage::ClearRenderTargets() {
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	for(unsigned int i=0; i<rts.size(); i++) {
		if(rts[i].enable_clear)
			context->ClearRenderTargetView(rts[i].tex->GetRenderTargetView(), rts[i].rgba);
	}
	if(ds.enable_depth_clear || ds.enable_stencil_clear) {
		unsigned int flag = ds.enable_depth_clear ? D3D11_CLEAR_DEPTH:0;
		flag |= ds.enable_stencil_clear ? D3D11_CLEAR_STENCIL:0;
		
		context->ClearDepthStencilView(ds.tex->GetDepthStencilView(), flag, ds.depth, ds.stencil);
	}
}

void D3D11OutputStage::SetOutput() {
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	//Set render target.
	int last_index = -1;
	for(int i=rts.size()-1; i>=0; i--) {
		if(rts[i].tex) {
			last_index = i;
			break;
		}
	}
	if(last_index != -1) {
		ID3D11RenderTargetView **array = new ID3D11RenderTargetView *[last_index+1];
		for(int i=0; i<=last_index; i++) {
			array[i] = rts[i].tex->GetRenderTargetView();
		}
		context->OMSetRenderTargets(last_index+1, array, ds.tex?ds.tex->GetDepthStencilView():0);
		delete[] array;
	}
}


}