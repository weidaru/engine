#pragma comment(lib, "d3d11.lib")


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
	rts.resize(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
}

D3D11OutputStage::~D3D11OutputStage() {
}

void D3D11OutputStage::SetRenderTarget(unsigned int index, Texture2D *_target) {
	D3D11Texture2D * target = NiceCast(D3D11Texture2D *, _target);
	if(_target != 0) {
		CHECK(target)<<"target cannot be cast to D3D11Texture2D";
	}
	if(target) {
		for(unsigned int i=0; i<rts.size(); i++) {
			if(rts[i].resource == target) {
				rts[i].resource = 0;
				rts[i].is_new = true;
			}
		}
	}
	
	rts[index].resource = target;
	rts[index].is_new = true;
	if(target) {
		rts[index].view = target->GetRenderTargetView();
		CHECK(rts[index].view)<<"D3D11Texture2D "<<target->GetIDAndName()<<" is not created as render target";
	} else {
		rts[index].view = 0;
		
	}
}

Resource * D3D11OutputStage::GetRenderTarget(unsigned int index) {
	return rts[index].resource;
}

unsigned int D3D11OutputStage::GetRenderTargetCapacity() const {
	return rts.size();
}

void D3D11OutputStage::SetDepthStencilBuffer(Texture2D *_buffer) {
	D3D11Texture2D * buffer = NiceCast(D3D11Texture2D *, _buffer);
	if(_buffer != 0) {
		CHECK(buffer)<<"buffer cannot be cast to D3D11Texture2D";
	}
	ds.resource = buffer;
	ds.is_new = true;
	if(buffer) {
		ds.view = buffer->GetDepthStencilView();
		CHECK(ds.view)<<"D3D11Texture2D "<<buffer->GetIDAndName()<<" is not created as depth stencil";
	} else {
		ds.view = 0;
	}
}

Resource* D3D11OutputStage::GetDepthStencilBuffer() {
	return ds.resource;
}

void D3D11OutputStage::Setup() {
	SetOutput();
}

D3D11OutputStage::RTBindingVector D3D11OutputStage::GetNewRenderTargetBindings() const {
	RTBindingVector result;
	for(unsigned int i=0; i<rts.size(); i++) {
		if(rts[i].is_new == false) {
			continue;
		}
		result.push_back(std::make_pair(i, rts[i].resource));
	}
	return result;
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
	for(unsigned int i=0; i<rts.size(); i++) {
		if(rts[i].resource != 0) {
			array[i] = rts[i].view;
		} else {
			array[i] = 0;
		}
		rts[i].is_new = false;
	}
	context->OMSetRenderTargets(rts.size(), array, ds.view);
	ds.is_new = false;
	delete[] array;
}


}