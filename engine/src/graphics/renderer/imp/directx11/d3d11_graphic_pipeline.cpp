#pragma comment(lib, "d3d11.lib")


#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_graphic_pipeline.h"
#include "d3d11_graphic_pipeline_state.h"

#include <stdio.h>
#include <functional>

#include "d3d11_vertex_shader.h"
#include "d3d11_pixel_shader.h"
#include "d3d11_geometry_shader.h"
#include "d3d11_enum_converter.h"
#include "d3d11_graphic_resource_manager.h"
#include "d3d11_resource_view.h"
#include "d3d11_input_layout.h"
#include "d3d11_shader_data.h"

#include "engine.h"

#ifdef NDEBUG
	#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
	#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif

namespace s2 {

D3D11GraphicPipeline::D3D11GraphicPipeline(D3D11GraphicResourceManager *_manager,  ID3D11DeviceContext *_context)
	:	manager(_manager),  context(_context) {
	CHECK_NOTNULL(manager);
	CHECK_NOTNULL(context);

	vbs.resize(D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
	topology = TRIANGLE_LIST;
	input_layout = 0;

	vs = 0;
	ps = 0;
	gs = 0;

	vs_data = 0;
	ps_data = 0;
	gs_data = 0;
	rast_state = 0;
	ds_state = 0;
	blend_state = 0;

	stencil_ref  = 0xFF;
	blend_factor[0] = 1.0f;
	blend_factor[1] = 1.0f;
	blend_factor[2] = 1.0f;
	blend_factor[3] = 1.0f;

	const RendererSetting &renderer_setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	RasterizationOption option;
	option.viewports.clear();
	option.viewports.push_back(RasterizationOption::Rectangle(0.0f, 0.0f, 
								(float)renderer_setting.window_width, (float)renderer_setting.window_height));
	SetRasterizationOption(option);
	SetDepthStencilOption(DepthStencilOption());
	SetBlendOption(BlendOption());

	rts.resize(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
	stream_outs.resize(D3D11_SO_STREAM_COUNT);
}

D3D11GraphicPipeline::~D3D11GraphicPipeline() {
	if(rast_state) {
		rast_state->Release();
	}
	
	if(ds_state) {
		ds_state->Release();
	}
	
	if(blend_state) {
		blend_state->Release();
	}

	if(context) {
		context->Release();
	}
}

void D3D11GraphicPipeline::SetPrimitiveTopology(PrimitiveTopology newvalue) {
	topology = newvalue;
	context->IASetPrimitiveTopology(D3D11EnumConverter::TopologyToD3D11Topology(topology));
}

GraphicPipeline::PrimitiveTopology D3D11GraphicPipeline::GetPrimitiveTopology() {
	return topology;
}

void D3D11GraphicPipeline::SetInputLayout(InputLayout *_layout) {
	input_layout = static_cast<D3D11InputLayout *>(_layout);
	if(input_layout) {
		context->IASetInputLayout(input_layout->GetInternal());
	} else {
		context->IASetInputLayout(0);
	}
}

void D3D11GraphicPipeline::SetVertexBuffer(uint32_t index, VertexBuffer *buf) {
	SetVertexBuffer(index, buf, 0, 0);
}

void D3D11GraphicPipeline::SetVertexBuffer(uint32_t start_index, const std::vector<VertexBuffer *> &_input) {
		std::vector<VertexBufferBinding> input(_input.size());
	for(uint32_t i=0; i<_input.size(); i++) {
		input.push_back({_input[i], 0, 0});
	}
	SetVertexBuffer(start_index, input);
}

void D3D11GraphicPipeline::SetVertexBuffer(uint32_t index, VertexBuffer *_buf, uint32_t stride, uint32_t offset) {
	D3D11VertexBuffer *buffer = NiceCast(D3D11VertexBuffer *, _buf);
	VBInfo &info = vbs[index];
	info.vb = buffer;
	if(stride == 0 && buffer != 0) {
		info.stride = buffer->GetResource()->GetElementBytewidth() ;
	} else {
		info.stride = stride;
	}
	info.offset = offset;

	//Set vertex buffer.
	ID3D11Buffer *buffer_raw = buffer ? buffer->GetBuffer() : 0;
	context->IASetVertexBuffers(index, 1, &buffer_raw, &info.stride, &info.offset);
}

void D3D11GraphicPipeline::SetVertexBuffer(uint32_t start_index,  
				const std::vector<VertexBufferBinding> &input) {
	for(uint32_t i=0; i<input.size(); i++) {
		const VertexBufferBinding &item = input[i];
		VBInfo &info = vbs[start_index+i];
		info.vb = static_cast<D3D11VertexBuffer *>(item.buffer);
		if(item.stride == 0 && item.buffer != 0) {
			info.stride = info.vb->GetResource()->GetElementBytewidth();
		} else {
			info.stride = item.stride;
		}
		info.offset = item.offset;
	}
	
	ID3D11Buffer **buffer_array  = new ID3D11Buffer*[input.size()];
	uint32_t *strides = new uint32_t[input.size()];
	uint32_t *offsets = new uint32_t[input.size()];

	for(uint32_t i=0; i<input.size(); i++) {
		VBInfo &info = vbs[start_index+i];

		buffer_array[i] = info.vb ?  info.vb->GetBuffer() : 0;
		offsets[i] = info.offset;
		strides[i] = info.stride;
	}
	context->IASetVertexBuffers(start_index, input.size(), buffer_array, strides, offsets);

	delete[] buffer_array;
	delete[] strides;
	delete[] offsets;
}

VertexBuffer * D3D11GraphicPipeline::GetVertexBuffer(uint32_t index, uint32_t *stride, uint32_t *offset) {
	VBInfo &info = vbs[index];
	if(stride) {
		*stride = info.stride;
	}
	if(offset) {
		*offset = info.offset;
	}

	return info.vb;
}

void D3D11GraphicPipeline::SetIndexBuffer(IndexBuffer *_buf, uint32_t vertex_base) {
	D3D11IndexBuffer *buffer = NiceCast(D3D11IndexBuffer *, _buf);

	ib.vertex_base = vertex_base;
	ib.buffer = buffer;
	//Set index buffer
	ID3D11Buffer *buffer_raw = buffer ? buffer->GetBuffer() : 0;
	context->IASetIndexBuffer(buffer_raw, DXGI_FORMAT_R32_UINT, 0);
}

IndexBuffer * D3D11GraphicPipeline::GetIndexBuffer(uint32_t *vertex_base) {
	*vertex_base = ib.vertex_base;
	return ib.buffer;
}

void D3D11GraphicPipeline::SetVertexShader(VertexShader *shader) {
	if (shader == 0) {
		context->VSSetShader(0, 0, 0);
	} else {
		vs = NiceCast(D3D11VertexShader *, shader);
		if (vs) {
			context->VSSetShader(vs->GetInternal(), 0, 0);
		} else {
			context->VSSetShader(0, 0, 0);
		}
	}
}

void D3D11GraphicPipeline::SetVertexShaderData(ShaderData *data) {
	if(data == 0) {
		D3D11ShaderData::UnBind(this, ShaderType::VERTEX);
	} else {
		vs_data = static_cast<D3D11ShaderData *>(data);
		vs_data->Setup(this, ShaderType::VERTEX);
	}
}

ShaderData * D3D11GraphicPipeline::GetVertexShaderData() {
	return vs_data;
}

void D3D11GraphicPipeline::SetPixelShader(PixelShader *shader) {
	if (shader == 0) {
		context->PSSetShader(0, 0, 0);
		
	} else {
		ps = NiceCast(D3D11PixelShader *, shader);
		if (vs) {
			context->PSSetShader(ps->GetInternal(), 0, 0);
		} else {
			context->PSSetShader(0, 0, 0);
		}
	}
}

void D3D11GraphicPipeline::SetPixelShaderData(ShaderData *data) {
	if(data == 0) {
		D3D11ShaderData::UnBind(this, ShaderType::PIXEL);
	} else {
		ps_data = static_cast<D3D11ShaderData *>(data);
		ps_data->Setup(this, ShaderType::PIXEL);
	}
}

ShaderData * D3D11GraphicPipeline::GetPixelShaderData() {
	return ps_data;
}

void D3D11GraphicPipeline::SetGeometryShader(GeometryShader *shader) {
	if (shader == 0) {
		context->GSSetShader(0, 0, 0);
		
	} else {
		gs = NiceCast(D3D11GeometryShader *, shader);
		if (vs) {
			context->GSSetShader(gs->GetInternal(), 0, 0);
		} else {
			context->GSSetShader(0, 0, 0);
		}
	}
}


void D3D11GraphicPipeline::SetGeometryShaderData(ShaderData *data) {
	if(data == 0) {
		D3D11ShaderData::UnBind(this, ShaderType::GEOMETRY);
	} else {
		gs_data = static_cast<D3D11ShaderData *>(data);
		gs_data->Setup(this, ShaderType::GEOMETRY);
	}
}

ShaderData * D3D11GraphicPipeline::GetGeometryShaderData() {
	return ps_data;
}

ID3D11RasterizerState * D3D11GraphicPipeline::ParseRasterizationOption(
				ID3D11Device *device, const RasterizationOption &option) {
	D3D11_RASTERIZER_DESC desc;
	switch(option.fill_mode) {
	case RasterizationOption::WIREFRAME:
		desc.FillMode = D3D11_FILL_WIREFRAME;
		break;
	case RasterizationOption::SOLID:
		desc.FillMode = D3D11_FILL_SOLID;
		break;
	default:
		break;
	}
	switch(option.cull_mode) {
	case RasterizationOption::NONE:
		desc.CullMode = D3D11_CULL_NONE;
		break;
	case RasterizationOption::FRONT:
		desc.CullMode = D3D11_CULL_FRONT;
		break;
	case RasterizationOption::BACK:
		desc.CullMode = D3D11_CULL_BACK;
		break;
	default:
		break;
	}
	switch(option.face_orient) {
	case RasterizationOption::CLOCKWISE:
		desc.FrontCounterClockwise = false;
		break;
	case RasterizationOption::COUNTER_CLOCKWISE:
		desc.FrontCounterClockwise = true;
		break;
	default:
		break;
	}
	desc.DepthBias = 0;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthBiasClamp = 0.0f;
	desc.DepthClipEnable = true;
	desc.ScissorEnable = option.enable_scissor;
	
	//TODO: Have some control over these fields.
	desc.MultisampleEnable = false;
	desc.AntialiasedLineEnable = false;
	ID3D11RasterizerState *state = 0;
	HRESULT result = 1;
	result = device->CreateRasterizerState(&desc, &state);
	CHECK(!FAILED(result))<<"Fail to create rasterization state. Error code "<< ::GetLastError();
	
	return state;
}

ID3D11DepthStencilState * D3D11GraphicPipeline::ParseDepthStencilOption(
			ID3D11Device *device,const DepthStencilOption &option) {
	D3D11_DEPTH_STENCIL_DESC desc;
	//Depth
	desc.DepthEnable = option.enable_depth;
	desc.DepthWriteMask = option.enable_depth_write ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11EnumConverter::DSCompareToD3D11Compare(option.depth_compare);
	
	//StencilEnable
	desc.StencilEnable = option.enable_stencil;
	desc.StencilReadMask = option.stencil_read_mask;
	desc.StencilWriteMask = option.stencil_write_mask;
	desc.FrontFace.StencilFailOp = D3D11EnumConverter::StencilOpToD3D11StencilOp(option.front_fail);
	desc.FrontFace.StencilDepthFailOp = D3D11EnumConverter::StencilOpToD3D11StencilOp(option.front_stencil_pass_depth_fail);
	desc.FrontFace.StencilPassOp = D3D11EnumConverter::StencilOpToD3D11StencilOp(option.front_pass);
	desc.FrontFace.StencilFunc = D3D11EnumConverter::DSCompareToD3D11Compare(option.front_compare);
	desc.BackFace.StencilFailOp = D3D11EnumConverter::StencilOpToD3D11StencilOp(option.back_fail);
	desc.BackFace.StencilDepthFailOp = D3D11EnumConverter::StencilOpToD3D11StencilOp(option.back_stencil_pass_depth_fail);
	desc.BackFace.StencilPassOp = D3D11EnumConverter::StencilOpToD3D11StencilOp(option.back_pass);
	desc.BackFace.StencilFunc = D3D11EnumConverter::DSCompareToD3D11Compare(option.back_compare);
	
	HRESULT result = 1;
	ID3D11DepthStencilState *state=0;
	result = device->CreateDepthStencilState(&desc, &state);
	CHECK(!FAILED(result))<<"Fail to create depth stencil state. Error code "<< ::GetLastError();
	return state;
}

ID3D11BlendState * D3D11GraphicPipeline::ParseBlendOption(
					ID3D11Device *device, const BlendOption &option) {
	D3D11_BLEND_DESC desc;
	desc.AlphaToCoverageEnable = false;		//Disabled for now, see class BlendOption.
	desc.IndependentBlendEnable= option.rt_options.size()>1;
	
	for(uint32_t i=0; i<option.rt_options.size(); i++) {
		const BlendOption::RenderTargetBlendOption &opt = option.rt_options[i];
		desc.RenderTarget[i].BlendEnable = true;
		desc.RenderTarget[i].SrcBlend = D3D11EnumConverter::BlendFactorToD3D11Blend(opt.src_factor);
		desc.RenderTarget[i].DestBlend = D3D11EnumConverter::BlendFactorToD3D11Blend(opt.dest_factor);
		desc.RenderTarget[i].BlendOp = D3D11EnumConverter::BlendOpToD3D11BlendOp(opt.op);
		desc.RenderTarget[i].SrcBlendAlpha = D3D11EnumConverter::BlendFactorToD3D11Blend(opt.src_alpha_factor);
		desc.RenderTarget[i].DestBlendAlpha = D3D11EnumConverter::BlendFactorToD3D11Blend(opt.dest_alpha_factor);
		desc.RenderTarget[i].BlendOpAlpha = D3D11EnumConverter::BlendOpToD3D11BlendOp(opt.alpha_op);
		desc.RenderTarget[i].RenderTargetWriteMask = opt.write_mask;
	}
	
	HRESULT result = 1;
	ID3D11BlendState *state = 0;
	result = device->CreateBlendState(&desc, &state);
	CHECK(!FAILED(result))<<"Fail to create blend state. Error code "<< ::GetLastError();
	return state;
}
	
void D3D11GraphicPipeline::SetRasterizationOption(const RasterizationOption &option) {
	
	rast_opt = option;
	if(rast_state) {
		rast_state->Release();
	}
	rast_state = ParseRasterizationOption(manager->GetDevice(), rast_opt);
	SyncRasterizationOption();
}

void D3D11GraphicPipeline::SetRasterizationOption(const RasterizationOption &opt, ID3D11RasterizerState *state) {
	CHECK_NOTNULL(state);
	state->AddRef();
	rast_opt = opt;
	if(rast_state) {
		rast_state->Release();
	}
	rast_state = state;
	SyncRasterizationOption();
}


void D3D11GraphicPipeline::SetDepthStencilOption(const DepthStencilOption &option) {
	
	ds_opt = option;
	if(ds_state)
		ds_state->Release();
	ds_state = ParseDepthStencilOption(manager->GetDevice(), ds_opt);
	SyncDepthStencilOption();
}

void D3D11GraphicPipeline::SetDepthStencilOption(const DepthStencilOption &opt, ID3D11DepthStencilState *state){
	CHECK_NOTNULL(state);
	state->AddRef();
	ds_opt = opt;
	if(ds_state) {
		ds_state->Release();
	}
	ds_state = state;
	SyncDepthStencilOption();
}

void D3D11GraphicPipeline::SetBlendOption(const BlendOption &option) {
	blend_opt = option;
	if(blend_state)
		blend_state->Release();
	blend_state = ParseBlendOption(manager->GetDevice(), blend_opt);
	SyncBlendOption();
}

void D3D11GraphicPipeline::SetBlendOption(const BlendOption &opt, ID3D11BlendState *state){
	CHECK_NOTNULL(state);
	state->AddRef();
	if(blend_state) {
		blend_state->Release();
	}
	blend_state = state;
	SyncBlendOption();
}

void D3D11GraphicPipeline::SyncRenderTargetsAndDepthStencil() {
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


void D3D11GraphicPipeline::SetRenderTargetAndDepthStencil(
		uint32_t index, RenderTarget *_target, DepthStencil *_ds) {
	D3D11RenderTarget * target = NiceCast(D3D11RenderTarget *, _target);

	rts[index].data = target;
	ds.data = static_cast<D3D11DepthStencil *>(_ds);

	SyncRenderTargetsAndDepthStencil();
}

void D3D11GraphicPipeline::SetRenderTargetAndDepthStencil(
		uint32_t start_index, const std::vector<RenderTarget *> &input, DepthStencil *_ds){
	for(uint32_t i=0; i<input.size(); i++) {
		rts[start_index+i].data = static_cast<D3D11RenderTarget *>(input[i]);
	}
	ds.data = static_cast<D3D11DepthStencil *>(_ds);
	
	SyncRenderTargetsAndDepthStencil();
}

RenderTarget * D3D11GraphicPipeline::GetRenderTarget(uint32_t index) {
	return rts[index].data;
}

DepthStencil* D3D11GraphicPipeline::GetDepthStencil() {
	return ds.data;
}

void D3D11GraphicPipeline::SyncStreamOuts() {
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

void D3D11GraphicPipeline::SetStreamOut(uint32_t index, uint32_t stream_index, StreamOut *_stream_out) {	
	D3D11StreamOut * stream_out = NiceCast(D3D11StreamOut *, _stream_out);
	stream_outs[index].data = stream_out;
	stream_outs[index].stream_index = stream_index;

	SyncStreamOuts();
}

void D3D11GraphicPipeline::SetStreamOut(uint32_t start_index, const std::vector<StreamOutBinding> &input) {
	for(uint32_t i=0; i<input.size(); i++) {
		stream_outs[start_index+i].data = static_cast<D3D11StreamOut *>(input[i].streamout);
		stream_outs[start_index+1].stream_index = input[i].stream_index;
	}

	SyncStreamOuts();
}

StreamOut * D3D11GraphicPipeline::GetStreamOut(uint32_t index, uint32_t *stream_index) {
	if (stream_index) {
		*stream_index = stream_outs[index].stream_index;
	}

	return stream_outs[index].data;
}

void D3D11GraphicPipeline::SyncRasterizationOption() {
	CHECK(rast_state);
	RasterizationOption &option = rast_opt;
	
	if(!option.viewports.empty()) {
		D3D11_VIEWPORT *viewports = new D3D11_VIEWPORT[option.viewports.size()];
		for(uint32_t i=0; i<option.viewports.size(); i++) {
			viewports[i].TopLeftX = option.viewports[i].top_left_x;
			viewports[i].TopLeftY = option.viewports[i].top_left_y;
			viewports[i].Width = option.viewports[i].width;
			viewports[i].Height = option.viewports[i].height;
			viewports[i].MinDepth = 0.0f;
			viewports[i].MaxDepth = 1.0f;
		}
		context->RSSetViewports(option.viewports.size(), viewports);
		delete viewports;
	}

	if(!option.scissors.empty()) {
		D3D11_RECT *scissors = new D3D11_RECT[option.scissors.size()];
		for(uint32_t i=0; i<option.scissors.size(); i++) {
			scissors[i].left = (long)option.scissors[i].top_left_x;
			scissors[i].top = (long)option.scissors[i].top_left_y;
			scissors[i].right = scissors[i].left + (long)option.scissors[i].width;
			scissors[i].bottom = scissors[i].top + (long)option.scissors[i].height;
		}
		context->RSSetScissorRects(option.scissors.size(), scissors);
		delete scissors;
	}
	
	context->RSSetState(rast_state);
}

void D3D11GraphicPipeline::SyncDepthStencilOption() {
	CHECK(ds_state);
	context->OMSetDepthStencilState(ds_state, stencil_ref);
	
}

void D3D11GraphicPipeline::SyncBlendOption() {
	CHECK(blend_state);
	context->OMSetBlendState(blend_state , blend_factor, blend_opt.sample_mask);
}

void D3D11GraphicPipeline::SetStencilRef(uint8_t _stencil_ref) {
	stencil_ref = _stencil_ref;
	SyncDepthStencilOption();
}

uint8_t D3D11GraphicPipeline::GetStencilRef() {
	return stencil_ref;
}

void D3D11GraphicPipeline::SetBlendFactor(float factor[4]) {
	blend_factor[0] = factor[0];
	blend_factor[1] = factor[1];
	blend_factor[2] = factor[2];
	blend_factor[3] = factor[3];
	SyncBlendOption();
}

std::tuple<float, float, float, float> D3D11GraphicPipeline::GetBlendFactor() {
	return std::make_tuple(blend_factor[0], blend_factor[1], blend_factor[2], blend_factor[3]);
}

void D3D11GraphicPipeline::ClearRenderTarget(RenderTarget *_target, const float rgba[4]) {
	D3D11RenderTarget * target = NiceCast(D3D11RenderTarget *, _target);
	if (_target != 0) {
		CHECK(target) << "target cannot be cast to D3D11RenderTarget";
	}

	context->ClearRenderTargetView(target->GetRenderTargetView(), rgba);
}

void D3D11GraphicPipeline::ClearDepthStencil(
		DepthStencil *_ds, bool clear_depth, float depth, bool clear_stencil, int stencil) {
	D3D11DepthStencil * depth_stencil = NiceCast(D3D11DepthStencil *, _ds);
	if (depth_stencil != 0) {
		CHECK(depth_stencil) << "buffer cannot be cast to D3D11DepthStencil";
	}

	if (clear_depth || clear_stencil) {
		uint32_t flag = clear_depth ? D3D11_CLEAR_DEPTH : 0;
		flag |= clear_stencil ? D3D11_CLEAR_STENCIL : 0;

		context->ClearDepthStencilView(depth_stencil->GetDepthStencilView(), flag, depth, (UINT8)stencil);
	}
}

void D3D11GraphicPipeline::Draw(
		uint32_t vertex_start, uint32_t vertex_count) {
	context->Draw(vertex_count, vertex_start);
}

void D3D11GraphicPipeline::DrawIndex(
		uint32_t index_start, uint32_t index_count) {
	uint32_t vertex_base = 0;
	GetIndexBuffer(&vertex_base);
	context->DrawIndexed(index_count, index_start, vertex_base);
}

void D3D11GraphicPipeline::DrawInstance(
		uint32_t vertex_start, uint32_t vertex_count, uint32_t instance_start, uint32_t instance_count) {
	context->DrawInstanced(vertex_count, instance_count, vertex_start, instance_start);
}

void D3D11GraphicPipeline::DrawInstanceIndex(
		uint32_t index_start, uint32_t index_count, uint32_t instance_start, uint32_t instance_count) {
	uint32_t vertex_base=0;
	GetIndexBuffer(&vertex_base);
	context->DrawIndexedInstanced(index_count, instance_count, index_start, vertex_base, instance_start);
}

void D3D11GraphicPipeline::SetState(const GraphicPipelineState &_state) {
	const D3D11GraphicPipelineState &state = static_cast<const D3D11GraphicPipelineState &>(_state);

	SetInputLayout(state.GetInputLayout());
	SetVertexShader(state.GetVertexShader());
	SetPixelShader(state.GetPixelShader());
	SetGeometryShader(state.GetGeometryShader());
	
	SetRasterizationOption(state.GetRasterizationOption(), state.GetRasterizationState());
	SetDepthStencilOption(state.GetDepthStencilOption(), state.GetDepthStencilState());
	SetBlendOption(state.GetBlendOption(), state.GetBlendState());
}

GraphicPipelineState * D3D11GraphicPipeline::QueryState() const {
	D3D11GraphicPipelineState *state = new D3D11GraphicPipelineState(manager, true);
	state->SetInputLayout(input_layout);
	state->SetVertexShader(vs);
	state->SetPixelShader(ps);
	state->SetGeometryShader(gs);
	state->SetRasterizationState(rast_opt, rast_state);
	state->SetDepthStencilState(ds_opt, ds_state);
	state->SetBlendState(blend_opt, blend_state);

	return state;
}

}



