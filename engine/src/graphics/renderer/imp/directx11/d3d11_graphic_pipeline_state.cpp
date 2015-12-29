#pragma comment(lib, "d3d11.lib")

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_graphic_pipeline_state.h"

#include <stdio.h>

#include "d3d11_graphic_pipeline.h"
#include "d3d11_graphic_resource_manager.h"

#include "engine.h"

namespace s2 {

D3D11GraphicPipelineState::D3D11GraphicPipelineState(D3D11GraphicResourceManager *_manager) {
	manager = _manager;
	CHECK_NOTNULL(manager);

	topology = TRIANGLE_LIST;
	input_layout = 0;
	vbs.resize(32);
	ib = 0;
	vertex_base = 0;
	vs = 0;
	ps = 0;
	gs = 0;
	vs_data = 0;
	ps_data = 0;
	gs_data = 0;

	const RendererSetting &renderer_setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	RasterizationOption option;
	option.viewports.clear();
	option.viewports.push_back(RasterizationOption::Rectangle(0.0f, 0.0f, 
								(float)renderer_setting.window_width, (float)renderer_setting.window_height));
	SetRasterizationOption(rast_opt);
	SetDepthStencilOption(DepthStencilOption());
	SetBlendOption(BlendOption());

	rts.resize(8);
	ds = 0;
	stream_outs.resize(4);
	rasterized_stream = 0;
}

D3D11GraphicPipelineState::~D3D11GraphicPipelineState() {
	if(rast_state) {
		rast_state->Release();
	}
	
	if(ds_state) {
		ds_state->Release();
	}
	
	if(blend_state) {
		blend_state->Release();
	}
}

void D3D11GraphicPipelineState::SetPrimitiveTopology(PrimitiveTopology newvalue) {
	topology = newvalue;
}

GraphicPipeline::PrimitiveTopology D3D11GraphicPipelineState::GetPrimitiveTopology() {
	return topology;
}

void D3D11GraphicPipelineState::SetInputLayout(InputLayout *newvalue) {
	input_layout = newvalue;	
}

InputLayout * D3D11GraphicPipelineState::GetInputLayout() {
	return input_layout;
}

void D3D11GraphicPipelineState::SetVertexBuffer(uint32_t index, VertexBuffer *buf) {
	vbs[index].buffer = buf;
	vbs[index].stride = 0;
	vbs[index].offset = 0;
}

void D3D11GraphicPipelineState::SetVertexBuffer(uint32_t start_index, const std::vector<VertexBuffer *> &input) {
	for(uint32_t i=0; i<input.size(); i++) {
		vbs[start_index+i].buffer = input[i];
		vbs[start_index+i].stride = 0;
		vbs[start_index+i].offset = 0;
	}
}

void D3D11GraphicPipelineState::SetVertexBuffer(uint32_t index, VertexBuffer *buf, uint32_t stride, uint32_t offset) {
	vbs[index].buffer = buf;
	vbs[index].stride = stride;
	vbs[index].offset = offset;
}

void D3D11GraphicPipelineState::SetVertexBuffer(uint32_t start_index,  
				const std::vector<VertexBufferBinding> &input) {
	for(uint32_t i=0; i<input.size(); i++) {
		vbs[start_index+i] = input[i];
	}
}

VertexBuffer * D3D11GraphicPipelineState::GetVertexBuffer(uint32_t index, uint32_t *stride, uint32_t *offset) {
	const VertexBufferBinding &info = vbs[index];
	if(stride) {
		*stride = info.stride;
	}
	if(offset) {
		*offset = info.offset;
	}
	return info.buffer;
}

void D3D11GraphicPipelineState::SetIndexBuffer(IndexBuffer *_buf, uint32_t _vertex_base) {
	ib = _buf;
	vertex_base = _vertex_base;
}

IndexBuffer * D3D11GraphicPipelineState::GetIndexBuffer(uint32_t *_vertex_base) {
	if(_vertex_base) {
		*_vertex_base = vertex_base;
	}
	return ib;
}

void D3D11GraphicPipelineState::SetVertexShader(VertexShader *shader) {
	vs = shader;
}

VertexShader * D3D11GraphicPipelineState::GetVertexShader() {
	return vs;
}

void D3D11GraphicPipelineState::SetVertexShaderData(ShaderData *data) {
	vs_data = data;
}

ShaderData * D3D11GraphicPipelineState::GetVertexShaderData() {
	return vs_data;
}

void D3D11GraphicPipelineState::SetPixelShader(PixelShader *shader) {
	ps = shader;
}

PixelShader * D3D11GraphicPipelineState::GetPixelShader() {
	return ps;
}

void D3D11GraphicPipelineState::SetPixelShaderData(ShaderData *data) {
	ps_data = data;
}

ShaderData * D3D11GraphicPipelineState::GetPixelShaderData() {
	return ps_data;
}

void D3D11GraphicPipelineState::SetGeometryShader(GeometryShader *shader) {
	gs = shader;
}

GeometryShader * D3D11GraphicPipelineState::GetGeometryShader() {
	return gs;
}

void D3D11GraphicPipelineState::SetGeometryShaderData(ShaderData *data) {
	gs_data = data;
}

ShaderData * D3D11GraphicPipelineState::GetGeometryShaderData() {
	return ps_data;
}
	
void D3D11GraphicPipelineState::SetRasterizationOption(const RasterizationOption &option) {
	rast_opt = option;
	rast_state = D3D11GraphicPipeline::ParseRasterizationOption(manager->GetDevice(), rast_opt);
}

const RasterizationOption & D3D11GraphicPipelineState::GetRasterizationOption() const {
	return rast_opt;
}

void D3D11GraphicPipelineState::SetDepthStencilOption(const DepthStencilOption &option) {
	ds_opt = option;
	ds_state = D3D11GraphicPipeline::ParseDepthStencilOption(manager->GetDevice(), ds_opt);
}

const DepthStencilOption & D3D11GraphicPipelineState::GetDepthStencilOption() const {
	return ds_opt;
}

void D3D11GraphicPipelineState::SetBlendOption(const BlendOption &option) {
	blend_opt = option;
	blend_state = D3D11GraphicPipeline::ParseBlendOption(manager->GetDevice(), blend_opt);
}

const BlendOption & D3D11GraphicPipelineState::GetBlendOption() const {
	return blend_opt;
}

void D3D11GraphicPipelineState::SetRenderTarget(uint32_t index, RenderTarget *target) {
	rts[index] = target;
}

void D3D11GraphicPipelineState::SetRenderTarget(uint32_t start_index, const std::vector<RenderTarget *> &input){
	for(uint32_t i=0; i<input.size(); i++) {
		rts[start_index+i] = input[i];
	}
}

RenderTarget * D3D11GraphicPipelineState::GetRenderTarget(uint32_t index) {
	return rts[index];
}

void D3D11GraphicPipelineState::SetDepthStencil(DepthStencil *buffer) {
	ds = buffer;
}

DepthStencil* D3D11GraphicPipelineState::GetDepthStencil() {
	return ds;
}

void D3D11GraphicPipelineState::SetStreamOut(uint32_t index, uint32_t stream_index, StreamOut *stream_out) {	
	StreamOutBinding &info = stream_outs[index];

	info.stream_index = stream_index;
	info.streamout = stream_out;
}

void D3D11GraphicPipelineState::SetStreamOut(uint32_t start_index, const std::vector<StreamOutBinding> &input) {
	for(uint32_t i=0; i<input.size(); i++) {
		stream_outs[start_index+i] = input[i];
	}

}

StreamOut * D3D11GraphicPipelineState::GetStreamOut(uint32_t index, uint32_t *stream_index) {
	StreamOutBinding &info = stream_outs[index];
	
	if(stream_index) {
		*stream_index = info.stream_index;
	}
	return info.streamout;
}

void D3D11GraphicPipelineState::SetRasterizedStream(int index) {
	rasterized_stream = index;
}

int D3D11GraphicPipelineState::GetRasterizedStream() {
	return rasterized_stream;
}

void D3D11GraphicPipelineState::Flush(GraphicPipeline *_pipeline) {
	D3D11GraphicPipeline *pipeline = static_cast<D3D11GraphicPipeline *>(_pipeline);

	pipeline->SetPrimitiveTopology(topology);
	pipeline->SetInputLayout(input_layout);
	pipeline->SetVertexBuffer(0, vbs);
	pipeline->SetIndexBuffer(ib, vertex_base);
	pipeline->SetVertexShader(vs);
	pipeline->SetPixelShader(ps);
	pipeline->SetGeometryShader(gs);
	pipeline->SetVertexShaderData(vs_data);
	pipeline->SetPixelShaderData(ps_data);
	pipeline->SetGeometryShaderData(gs_data);
	pipeline->SetRasterizationOption(rast_opt, rast_state);
	pipeline->SetDepthStencilOption(ds_opt, ds_state);
	pipeline->SetBlendOption(blend_opt, blend_state);
	pipeline->SetRenderTarget(0, rts);
	pipeline->SetStreamOut(0, stream_outs);
	pipeline->SetRasterizedStream(rasterized_stream);
}

}



