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
	rts.resize(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, 0);
}

void D3D11OutputStage::ResetRenderTargets() {
	for(unsigned int i=0; i<rts.size(); i++) {
		rts[i] = 0;
	}
}

void D3D11OutputStage::SetRenderTarget(unsigned int index, D3D11Texture2D *target) {
	new_output = true;
	for(unsigned int i=0; i<rts.size(); i++) {
		if(rts[i] == target) {
			rts[i] = 0;
		}
	}
	rts[index] = target;
}

Resource * D3D11OutputStage::GetRenderTarget(unsigned int index) {
	return rts[index];
}

void D3D11OutputStage::SetDepthStencilBuffer(D3D11Texture2D *buffer) {
	new_output = true;
	ds = buffer;
}

Resource* D3D11OutputStage::GetDepthStencilBuffer() {
	return ds;
}

void D3D11OutputStage::Setup() {
	if(new_output)
		SetOutput();
	new_output = false;
}

void D3D11OutputStage::ClearRenderTarget(Texture2D *_texture, const float rgba[4]) {
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	D3D11Texture2D *texture = NiceCast(D3D11Texture2D *, _texture);
	CHECK(texture->GetRenderTargetView()) << "Texture is not bound as RenderTarget, cannot clear.";
	context->ClearRenderTargetView(texture->GetRenderTargetView(),rgba);
}

void D3D11OutputStage::ClearDepthStencilBuffer(Texture2D *_buffer, bool clear_depth, float depth, bool clear_stencil, int stencil) {
	if(clear_depth|| clear_stencil) {
		unsigned int flag = clear_depth ? D3D11_CLEAR_DEPTH:0;
		flag |= clear_stencil ? D3D11_CLEAR_STENCIL:0;
		
		ID3D11DeviceContext *context = manager->GetDeviceContext();
		D3D11Texture2D *buffer = NiceCast(D3D11Texture2D *, _buffer);
		CHECK(buffer->GetDepthStencilView()) << "Texture is not bound as DepthStencil, cannot clear";
		context->ClearDepthStencilView(buffer->GetDepthStencilView(), flag, depth, (UINT8)stencil);
	}
}

void D3D11OutputStage::SetOutput() {
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	ID3D11RenderTargetView **array = new ID3D11RenderTargetView *[rts.size()];
	for(int i=0; i<rts.size(); i++) {
		if(rts[i] != 0) {
			array[i] = rts[i]->GetRenderTargetView();
		} else {
			array[i] = 0;
		}
	}
	context->OMSetRenderTargets(rts.size(), array, ds?ds->GetDepthStencilView():0);
	delete[] array;
}


}