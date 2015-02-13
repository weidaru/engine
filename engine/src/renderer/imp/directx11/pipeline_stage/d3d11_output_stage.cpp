#pragma comment(lib, "d3d11.lib")


#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_output_stage.h"

#include "renderer/imp/directx11/d3d11_graphic_resource_manager.h"
#include "renderer/imp/directx11/d3d11_context.h"
#include "renderer/imp/directx11/d3d11_texture2d.h"
#include "renderer/imp/directx11/d3d11_resource_view.h"

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

void D3D11OutputStage::SetRenderTarget(unsigned int index, RenderTarget *_target) {
	D3D11RenderTarget * target = NiceCast(D3D11RenderTarget *, _target);
	if(_target != 0) {
		CHECK(target)<<"target cannot be cast to D3D11RenderTarget";
	}
	if(target) {
		for(unsigned int i=0; i<rts.size(); i++) {
			if(rts[i].render_target == target) {
				rts[i].render_target = 0;
				rts[i].is_new = true;
			}
		}
	}
	
	rts[index].render_target = target;
	rts[index].is_new = true;
}

D3D11RenderTarget * D3D11OutputStage::GetRenderTarget(unsigned int index) {
	return rts[index].render_target;
}

unsigned int D3D11OutputStage::GetRenderTargetCapacity() const {
	return rts.size();
}

void D3D11OutputStage::SetDepthStencil(DepthStencil *_depth_stencil) {
	D3D11DepthStencil * depth_stencil = NiceCast(D3D11DepthStencil *, _depth_stencil);
	if (depth_stencil != 0) {
		CHECK(depth_stencil) << "buffer cannot be cast to D3D11DepthStencil";
	}
	ds.depth_stencil = depth_stencil;
	ds.is_new = true;
}

D3D11DepthStencil * D3D11OutputStage::GetDepthStencil() {
	return ds.depth_stencil;
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
		Resource *resource = 0;
		if (rts[i].render_target) {
			resource = rts[i].render_target->GetResource();
		}
		result.push_back(std::make_pair(i, resource));
	}
	return result;
}

void D3D11OutputStage::ClearRenderTarget(RenderTarget *_target, const float rgba[4]) {
	D3D11RenderTarget * target = NiceCast(D3D11RenderTarget *, _target);
	if (_target != 0) {
		CHECK(target) << "target cannot be cast to D3D11RenderTarget";
	}

	ID3D11DeviceContext *context = manager->GetDeviceContext();
	context->ClearRenderTargetView(target->GetRenderTargetView(), rgba);
}

void D3D11OutputStage::ClearDepthStencil(DepthStencil *_depth_stencil, bool clear_depth, float depth, bool clear_stencil, int stencil) {
	D3D11DepthStencil * depth_stencil = NiceCast(D3D11DepthStencil *, _depth_stencil);
	if (depth_stencil != 0) {
		CHECK(depth_stencil) << "buffer cannot be cast to D3D11DepthStencil";
	}

	if(clear_depth|| clear_stencil) {
		unsigned int flag = clear_depth ? D3D11_CLEAR_DEPTH:0;
		flag |= clear_stencil ? D3D11_CLEAR_STENCIL:0;
		
		ID3D11DeviceContext *context = manager->GetDeviceContext();
		context->ClearDepthStencilView(depth_stencil->GetDepthStencilView(), flag, depth, (UINT8)stencil);
	}
}

void D3D11OutputStage::SetOutput() {
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	ID3D11RenderTargetView **array = new ID3D11RenderTargetView *[rts.size()];
	for(unsigned int i=0; i<rts.size(); i++) {
		if(rts[i].render_target != 0) {
			array[i] = rts[i].render_target->GetRenderTargetView();
		} else {
			array[i] = 0;
		}
		rts[i].is_new = false;
	}
	if (ds.depth_stencil) {
		context->OMSetRenderTargets(rts.size(), array, ds.depth_stencil->GetDepthStencilView());
	} else {
		context->OMSetRenderTargets(rts.size(), array, 0);
	}
	
	ds.is_new = false;
	delete[] array;
}


}