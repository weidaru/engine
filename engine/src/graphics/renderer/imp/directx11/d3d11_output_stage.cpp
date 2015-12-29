#pragma comment(lib, "d3d11.lib")


#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_output_stage.h"

#include "d3d11_graphic_resource_manager.h"
#include "d3d11_graphic_pipeline.h"
#include "d3d11_context.h"
#include "d3d11_texture2d.h"
#include "d3d11_resource_view.h"
#include "d3d11_geometry_shader.h"
#include "d3d11_shader_reflection.h"

#include <algorithm>

#ifdef NDEBUG
#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif

namespace s2 {

D3D11OutputStage::D3D11OutputStage(D3D11GraphicResourceManager *_manager, ID3D11DeviceContext *_context)
	: manager(_manager), context(_context), rasterized_stream(-1){
	rts.resize(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
	stream_outs.resize(D3D11_SO_STREAM_COUNT);
}

D3D11OutputStage::~D3D11OutputStage() {
}

void D3D11OutputStage::SetRenderTarget(uint32_t index, RenderTarget *_target) {
	D3D11RenderTarget * target = NiceCast(D3D11RenderTarget *, _target);

	rts[index].data = target;

	SyncRenderTargetsAndDepthStencil(context);
}

void D3D11OutputStage::SyncRenderTargetsAndDepthStencil(ID3D11DeviceContext *context) {
	if(context == 0) {
		return;
	}
	ID3D11RenderTargetView *rt_array[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
	for (uint32_t i = 0; i<rts.size(); i++) {
		if (rts[i].data != 0) {
			rt_array[i] = rts[i].data->GetRenderTargetView();
		}
		else {
			rt_array[i] = 0;
		}
	}
	context->OMSetRenderTargets(rts.size(), rt_array, ds.data==0 ? 0 : ds.data->GetDepthStencilView());
}

void D3D11OutputStage::SetRenderTarget(uint32_t start_index, const std::vector<RenderTarget *> &input) {
	for(uint32_t i=0; i<input.size(); i++) {
		rts[start_index+i].data = static_cast<D3D11RenderTarget *>(input[i]);
	}
	
	SyncRenderTargetsAndDepthStencil(context);
}


D3D11RenderTarget * D3D11OutputStage::GetRenderTarget(uint32_t index) {
	return rts[index].data;
}

uint32_t D3D11OutputStage::GetRenderTargetCapacity() const {
	return rts.size();
}

void D3D11OutputStage::SetDepthStencil(DepthStencil *_depth_stencil) {
	D3D11DepthStencil * depth_stencil = NiceCast(D3D11DepthStencil *, _depth_stencil);
	if (_depth_stencil != 0) {
		CHECK(depth_stencil) << "buffer cannot be cast to D3D11DepthStencil";
	}
	ds.data = depth_stencil;

	SyncRenderTargetsAndDepthStencil(context);
}

D3D11DepthStencil * D3D11OutputStage::GetDepthStencil() {
	return ds.data;
}

void D3D11OutputStage::SyncStreamOuts(ID3D11DeviceContext *context) {
	if(context == 0) {
		return;
	}

	ID3D11Buffer **so_array = new ID3D11Buffer *[stream_outs.size()];
	UINT *offset = new UINT[stream_outs.size()];
	for (uint32_t i = 0; i < stream_outs.size(); i++) {
		if (stream_outs[i].data != 0) {
			so_array[i] = stream_outs[i].data->GetBuffer();
		}
		else {
			so_array[i] = 0;
		}
		offset[i] = 0;
	}
	context->SOSetTargets(stream_outs.size(), so_array, offset);
	delete [] so_array;
	delete [] offset;
}

void D3D11OutputStage::SetStreamOut(uint32_t index, uint32_t stream_index, StreamOut *_stream_out) {
	D3D11StreamOut * stream_out = NiceCast(D3D11StreamOut *, _stream_out);
	stream_outs[index].data = stream_out;
	stream_outs[index].stream_index = stream_index;

	SyncStreamOuts(context);
}

void D3D11OutputStage::SetStreamOut(uint32_t start_index, const std::vector<StreamOutBinding> &input) {
	for(uint32_t i=0; i<input.size(); i++) {
		stream_outs[start_index+i].data = static_cast<D3D11StreamOut *>(input[i].streamout);
		stream_outs[start_index+1].stream_index = input[i].stream_index;
	}

	SyncStreamOuts(context);
}

D3D11StreamOut * D3D11OutputStage::GetStreamOut(uint32_t index, uint32_t *stream_index) {
	if (stream_index) {
		*stream_index = stream_outs[index].stream_index;
	}

	return stream_outs[index].data;
}

void D3D11OutputStage::SetRasterizedStream(int index) {
	rasterized_stream = index;
}

int D3D11OutputStage::GetRasterizedStream() {
	return rasterized_stream;
}

void D3D11OutputStage::ClearRenderTarget(RenderTarget *_target, const float rgba[4]) {
	if(context==0) {
		return;
	}

	D3D11RenderTarget * target = NiceCast(D3D11RenderTarget *, _target);
	if (_target != 0) {
		CHECK(target) << "target cannot be cast to D3D11RenderTarget";
	}

	context->ClearRenderTargetView(target->GetRenderTargetView(), rgba);

}

void D3D11OutputStage::ClearDepthStencil(DepthStencil *_depth_stencil, bool clear_depth, float depth, bool clear_stencil, int stencil) {
	if(context == 0) {
		return;
	}

	D3D11DepthStencil * depth_stencil = NiceCast(D3D11DepthStencil *, _depth_stencil);
	if (depth_stencil != 0) {
		CHECK(depth_stencil) << "buffer cannot be cast to D3D11DepthStencil";
	}

	if (clear_depth || clear_stencil) {
		uint32_t flag = clear_depth ? D3D11_CLEAR_DEPTH : 0;
		flag |= clear_stencil ? D3D11_CLEAR_STENCIL : 0;

		context->ClearDepthStencilView(depth_stencil->GetDepthStencilView(), flag, depth, (UINT8)stencil);
	}
}

void D3D11OutputStage::PushState() {
	saved_states.push(State());
	State &new_state = saved_states.top();

	new_state.rts = rts;
	new_state.ds = ds;
	new_state.rasterized_stream = rasterized_stream;
	new_state.stream_outs = stream_outs;
}

void D3D11OutputStage::PopState() {
	State &state = saved_states.top();
	rts = state.rts;
	ds = state.ds;
	rasterized_stream = state.rasterized_stream;
	stream_outs = state.stream_outs;
	saved_states.pop();

	Sync(context);
}

void D3D11OutputStage::ClearSavedState() {
	while(saved_states.empty() == false) {
		saved_states.pop();
	}
}

void D3D11OutputStage::Sync(ID3D11DeviceContext *context) {
	SyncRenderTargetsAndDepthStencil(context);
	SyncStreamOuts(context);
}

}