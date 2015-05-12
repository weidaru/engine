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

D3D11OutputStage::D3D11OutputStage(D3D11GraphicResourceManager *_manager, D3D11GraphicPipeline *_pipeline)
	: manager(_manager), pipeline(_pipeline), rasterized_stream(-1){
	rts.resize(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
	stream_outs.resize(D3D11_SO_STREAM_COUNT);
}

D3D11OutputStage::~D3D11OutputStage() {
}

void D3D11OutputStage::SetRenderTarget(unsigned int index, RenderTarget *_target) {
	D3D11RenderTarget * target = NiceCast(D3D11RenderTarget *, _target);
	if (_target != 0) {
		CHECK(target) << "target cannot be cast to D3D11RenderTarget";
	}

	if (target) {
		for (unsigned int i = 0; i<rts.size(); i++) {
			if (rts[i].data == target) {
				rts[i].data = 0;
			}
		}
	}

	rts[index].data = target;

	ID3D11DeviceContext *context = pipeline->GetDeviceContext();
	ID3D11RenderTargetView **rt_array = new ID3D11RenderTargetView *[rts.size()];
	for (unsigned int i = 0; i<rts.size(); i++) {
		if (rts[i].data != 0) {
			rt_array[i] = rts[i].data->GetRenderTargetView();
		}
		else {
			rt_array[i] = 0;
		}
	}
	if (ds.data) {
		context->OMSetRenderTargets(rts.size(), rt_array, ds.data->GetDepthStencilView());
	}
	else {
		context->OMSetRenderTargets(rts.size(), rt_array, 0);
	}
}

D3D11RenderTarget * D3D11OutputStage::GetRenderTarget(unsigned int index) {
	return rts[index].data;
}

unsigned int D3D11OutputStage::GetRenderTargetCapacity() const {
	return rts.size();
}

void D3D11OutputStage::SetDepthStencil(DepthStencil *_depth_stencil) {
	D3D11DepthStencil * depth_stencil = NiceCast(D3D11DepthStencil *, _depth_stencil);
	if (_depth_stencil != 0) {
		CHECK(depth_stencil) << "buffer cannot be cast to D3D11DepthStencil";
	}
	ds.data = depth_stencil;

	ID3D11DeviceContext *context = pipeline->GetDeviceContext();
	ID3D11RenderTargetView **rt_array = new ID3D11RenderTargetView *[rts.size()];
	for (unsigned int i = 0; i<rts.size(); i++) {
		if (rts[i].data != 0) {
			rt_array[i] = rts[i].data->GetRenderTargetView();
		}
		else {
			rt_array[i] = 0;
		}

	}
	if (ds.data) {
		context->OMSetRenderTargets(rts.size(), rt_array, ds.data->GetDepthStencilView());
	}
	else {
		context->OMSetRenderTargets(rts.size(), rt_array, 0);
	}
}

D3D11DepthStencil * D3D11OutputStage::GetDepthStencil() {
	return ds.data;
}

void D3D11OutputStage::SetStreamOut(unsigned int index, unsigned int stream_index, StreamOut *_stream_out) {
	D3D11StreamOut * stream_out = NiceCast(D3D11StreamOut *, _stream_out);
	if (_stream_out != 0) {
		CHECK(stream_out) << "stream out cannot be cast to D3D11StreamOut";
	}
	stream_outs[index].is_new = true;
	stream_outs[index].data = stream_out;
	stream_outs[index].stream_index = stream_index;

	ID3D11DeviceContext *context = pipeline->GetDeviceContext();
	ID3D11Buffer **so_array = new ID3D11Buffer *[stream_outs.size()];
	UINT *offset = new UINT[stream_outs.size()];
	for (unsigned int i = 0; i < stream_outs.size(); i++) {
		if (stream_outs[i].data != 0) {
			so_array[i] = stream_outs[i].data->GetBuffer();
		}
		else {
			so_array[i] = 0;
		}
		offset[i] = 0;
	}
	context->SOSetTargets(stream_outs.size(), so_array, offset);
}

D3D11StreamOut * D3D11OutputStage::GetStreamOut(unsigned int index, unsigned int *stream_index) {
	if (stream_index) {
		*stream_index = stream_outs[index].stream_index;
	}

	return stream_outs[index].data;
}

ID3D11GeometryShader * D3D11OutputStage::BuildStreamOutGeometryShader(D3D11GeometryShader *gs){
	const D3D11ShaderReflection &reflect = gs->GetReflection();
	std::vector<D3D11_SO_DECLARATION_ENTRY> entries;

	unsigned int size = reflect.GetOutputSize();

	for (unsigned int i = 0; i < size; i++) {

		const D3D11ShaderReflection::Parameter& output_param = reflect.GetOutput(i);

		int slot_index = -1;
		for (auto it = stream_outs.begin(); it != stream_outs.end(); it++) {
			if (it->is_new && output_param.stream == it->stream_index) {
				slot_index = it - stream_outs.begin();
			}
		}
		if (slot_index != -1) {
			D3D11_SO_DECLARATION_ENTRY entry;
			entry.Stream = output_param.stream;
			entry.SemanticName = output_param.semantic.c_str();
			entry.SemanticIndex = output_param.semantic_index;
			entry.StartComponent = 0;
			entry.ComponentCount = output_param.size / TypeInfoManager::GetSingleton()->Get(output_param.type_name).GetSize();
			entry.OutputSlot = slot_index;
			entries.push_back(entry);
		}
	}

	unsigned real_rasterized_stream;
	if (rasterized_stream < 0) {
		real_rasterized_stream = D3D11_SO_NO_RASTERIZED_STREAM;
	}
	else {
		real_rasterized_stream = rasterized_stream;
	}

	ID3D11GeometryShader *raw_streamout_geometry_shader;
	HRESULT result = 1;
	result = manager->GetDevice()->CreateGeometryShaderWithStreamOutput(
		gs->GetBlob()->GetBufferPointer(),
		gs->GetBlob()->GetBufferSize(),
		&entries[0],
		entries.size(),
		0,
		0,
		real_rasterized_stream,
		0,
		&raw_streamout_geometry_shader
		);
	CHECK(!FAILED(result)) << "Fail to create streamout geometry shader error " << ::GetLastError();

	return raw_streamout_geometry_shader;
}


s2string D3D11OutputStage::DumpStreamOutInfo(const std::vector<SOInfo> &infos) {
	char buffer[1024 * 8];
	char *head = buffer;
	for (unsigned int i = 0; i<infos.size(); i++) {
		if (infos[i].data) {
			unsigned int stream_index = infos[i].stream_index;
			head += sprintf_s(head, 1024 * 8 - (head - buffer), "StreamOut %d at stream %d\n",
				i, stream_index);
		}

	}
	return s2string(buffer);
}

ID3D11GeometryShader * D3D11OutputStage::Setup(D3D11GeometryShader *gs) {
	ID3D11GeometryShader *raw_stream_out_shader = 0;

	if (gs) {
		raw_stream_out_shader = BuildStreamOutGeometryShader(gs);
	}

	return raw_stream_out_shader;
}

ID3D11GeometryShader * D3D11OutputStage::Setup(D3D11GeometryShader *gs, D3D11DrawingState *draw_state) {
	if (gs && draw_state->GetStreamOutGeometryShader() == 0) {
		draw_state->SetStreamOutGeometryShader(BuildStreamOutGeometryShader(gs));
	}

	return draw_state->GetStreamOutGeometryShader();
}

void D3D11OutputStage::SetRasterizedStream(int index) {
	rasterized_stream = index;
}

int D3D11OutputStage::GetRasterizedStream() {
	return rasterized_stream;
}

void D3D11OutputStage::ClearRenderTarget(RenderTarget *_target, const float rgba[4]) {
	D3D11RenderTarget * target = NiceCast(D3D11RenderTarget *, _target);
	if (_target != 0) {
		CHECK(target) << "target cannot be cast to D3D11RenderTarget";
	}

	ID3D11DeviceContext *context = pipeline->GetDeviceContext();
	context->ClearRenderTargetView(target->GetRenderTargetView(), rgba);
}

void D3D11OutputStage::ClearDepthStencil(DepthStencil *_depth_stencil, bool clear_depth, float depth, bool clear_stencil, int stencil) {
	D3D11DepthStencil * depth_stencil = NiceCast(D3D11DepthStencil *, _depth_stencil);
	if (depth_stencil != 0) {
		CHECK(depth_stencil) << "buffer cannot be cast to D3D11DepthStencil";
	}

	if (clear_depth || clear_stencil) {
		unsigned int flag = clear_depth ? D3D11_CLEAR_DEPTH : 0;
		flag |= clear_stencil ? D3D11_CLEAR_STENCIL : 0;

		ID3D11DeviceContext *context = pipeline->GetDeviceContext();
		context->ClearDepthStencilView(depth_stencil->GetDepthStencilView(), flag, depth, (UINT8)stencil);
	}
}

void D3D11OutputStage::Refresh() {
	for (auto it=stream_outs.begin(); it != stream_outs.end(); it++) {
		it->is_new = false;
	}

}


}