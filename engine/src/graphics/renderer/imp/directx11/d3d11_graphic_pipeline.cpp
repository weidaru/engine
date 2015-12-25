#pragma comment(lib, "d3d11.lib")


#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_graphic_pipeline.h"

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
	:	manager(_manager),  context(_context),
		input_stage(_manager, this),
		vs(0), ps(0), gs(0),
		vs_data(0), ps_data(0), gs_data(0),
		rast_state(0), ds_state(0), blend_state(0),
		output_stage(_manager, this) {
	CHECK_NOTNULL(manager);
	CHECK_NOTNULL(context);

	const RendererSetting &renderer_setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	RasterizationOption option;
	option.viewports.clear();
	option.viewports.push_back(RasterizationOption::Rectangle(0.0f, 0.0f, 
								(float)renderer_setting.window_width, (float)renderer_setting.window_height));
	SetRasterizationOption(option);
	
	SetDepthStencilOption(DepthStencilOption());
	
	SetBlendOption(BlendOption());
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
	
	input_stage.SetPrimitiveTopology(newvalue);
}

GraphicPipeline::PrimitiveTopology D3D11GraphicPipeline::GetPrimitiveTopology() {
	return input_stage.GetPrimitiveTopology();
}

void D3D11GraphicPipeline::SetInputLayout(InputLayout *layout) {
	
	input_stage.SetInputLayout(layout);
}

InputLayout * D3D11GraphicPipeline::GetInputLayout() {
	return input_stage.GetInputLayout();
}

void D3D11GraphicPipeline::SetVertexBuffer(uint32_t index, VertexBuffer *buf) {
	
	input_stage.SetVertexBuffer(index, buf);
}

VertexBuffer * D3D11GraphicPipeline::GetVertexBuffer(uint32_t index) {
	return input_stage.GetVertexBuffer(index);
}

void D3D11GraphicPipeline::SetIndexBuffer(IndexBuffer *_buf, uint32_t vertex_base) {
	
	input_stage.SetIndexBuffer( _buf, vertex_base);
}

IndexBuffer * D3D11GraphicPipeline::GetIndexBuffer(uint32_t *vertex_base) {
	return input_stage.GetIndexBuffer(vertex_base);
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

VertexShader * D3D11GraphicPipeline::GetVertexShader() {
	return vs;
}

void D3D11GraphicPipeline::SetVertexShaderData(ShaderData *data) {
	if(data == 0) {
	
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

PixelShader * D3D11GraphicPipeline::GetPixelShader() {
	return ps;
}

	virtual void SetPixelShaderData(ShaderData *data) override;
	virtual ShaderData * GetPixelShaderData() override;

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

GeometryShader * D3D11GraphicPipeline::GetGeometryShader() {
	return gs;
}

	virtual void SetGeometryShaderData(ShaderData *data) override;
	virtual ShaderData * GetGeometryShaderData() override;

namespace {

ID3D11RasterizerState * ParseRasterizationOption(ID3D11Device *device, const RasterizationOption &option) {
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

ID3D11DepthStencilState * ParseDepthStencilOption(ID3D11Device *device,const DepthStencilOption &option) {
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

ID3D11BlendState * ParseBlendOption(ID3D11Device *device, const BlendOption &option) {
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
	
}

void D3D11GraphicPipeline::SetRasterizationOption(const RasterizationOption &option) {
	
	rast_opt = option;
	if(rast_state)
		rast_state->Release();
	rast_state = ParseRasterizationOption(manager->GetDevice(), rast_opt);
	SetupRasterizationOption();
}

const RasterizationOption & D3D11GraphicPipeline::GetRasterizationOption() const {
	return rast_opt;
}

void D3D11GraphicPipeline::SetDepthStencilOption(const DepthStencilOption &option) {
	
	ds_opt = option;
	if(ds_state)
		ds_state->Release();
	ds_state = ParseDepthStencilOption(manager->GetDevice(), ds_opt);
	SetupDepthStencilOption();
}

const DepthStencilOption & D3D11GraphicPipeline::GetDepthStencilOption() const {
	return ds_opt;
}

void D3D11GraphicPipeline::SetBlendOption(const BlendOption &option) {
	
	blend_opt = option;
	if(blend_state)
		blend_state->Release();
	blend_state = ParseBlendOption(manager->GetDevice(), blend_opt);
	SetupBlendOption();
}

const BlendOption & D3D11GraphicPipeline::GetBlendOption() const {
	return blend_opt;
}

void D3D11GraphicPipeline::SetRenderTarget(uint32_t index, RenderTarget *target) {
	
	output_stage.SetRenderTarget(index, target);
}

RenderTarget * D3D11GraphicPipeline::GetRenderTarget(uint32_t index) {
	return output_stage.GetRenderTarget(index);
}

void D3D11GraphicPipeline::SetDepthStencil(DepthStencil *buffer) {
	
	output_stage.SetDepthStencil(buffer);
}

DepthStencil* D3D11GraphicPipeline::GetDepthStencil() {
	return output_stage.GetDepthStencil();
}

void D3D11GraphicPipeline::SetStreamOut(uint32_t index, uint32_t stream_index, StreamOut *stream_out) {
	
	output_stage.SetStreamOut(index, stream_index, stream_out);
}

StreamOut * D3D11GraphicPipeline::GetStreamOut(uint32_t index, uint32_t *stream_index) {
	return output_stage.GetStreamOut(index, stream_index);
}

void D3D11GraphicPipeline::SetRasterizedStream(int index) {
	
	output_stage.SetRasterizedStream(index);
}

int D3D11GraphicPipeline::GetRasterizedStream() {
	return output_stage.GetRasterizedStream();
}

void D3D11GraphicPipeline::SetupRasterizationOption() {
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

void D3D11GraphicPipeline::SetupDepthStencilOption() {
	CHECK(ds_state);
	context->OMSetDepthStencilState(ds_state, ds_opt.stencil_replace_value);
	
}

void D3D11GraphicPipeline::SetupBlendOption() {
	CHECK(blend_state);
	context->OMSetBlendState(blend_state , blend_opt.factor, blend_opt.sample_mask);
}

void D3D11GraphicPipeline::ClearRenderTarget(RenderTarget *rt, const float rgba[4]) {
	output_stage.ClearRenderTarget(rt, rgba);
}

void D3D11GraphicPipeline::ClearDepthStencil(DepthStencil *ds, bool clear_depth, float depth, bool clear_stencil, int stencil) {
	output_stage.ClearDepthStencil(ds, clear_depth, depth , clear_stencil, stencil);
}

void Draw(uint32_t vertex_start, uint32_t vertex_count) {

}

void DrawIndex( uint32_t index_start, uint32_t index_count) {

}

void DrawInstance(uint32_t vertex_start, uint32_t vertex_count, uint32_t instance_start, uint32_t instance_count) {

}

void DrawInstanceIndex(uint32_t index_start, uint32_t index_count, uint32_t instance_start, uint32_t instance_count) {
	
}

void D3D11GraphicPipeline::PushState() {
	input_stage.PushState();
	output_stage.PushState();

	saved_states.push(State());
	State &new_state= saved_states.top();
	new_state.vs = vs;
	new_state.ps = ps;
	new_state.gs = gs;

	new_state.rast_opt = rast_opt;
	new_state.rast_state = rast_state;
	rast_state->AddRef();

	new_state.ds_opt = ds_opt;
	new_state.ds_state = ds_state;
	ds_state->AddRef();

	new_state.blend_opt = blend_opt;
	new_state.blend_state = blend_state;
	blend_state->AddRef();
}

void D3D11GraphicPipeline::PopState() {
	State &state = saved_states.top();

	input_stage.PopState();
	output_stage.PopState();

	SetVertexShader(state.vs);
	SetPixelShader(state.ps);
	SetGeometryShader(state.gs);

	rast_opt = state.rast_opt;
	rast_state->Release();
	rast_state = state.rast_state;

	ds_opt = state.ds_opt;
	ds_state->Release();
	ds_state = state.ds_state;

	blend_opt = state.blend_opt;
	blend_state->Release();
	blend_state = state.blend_state;
	
}

void D3D11GraphicPipeline::ClearSavedState() {
	while(saved_states.empty() == false) {
		State &state = saved_states.top();

		state.rast_state->Release();
		state.ds_state->Release();
		state.blend_state->Release();

		saved_states.pop();
	}
}

}



