#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_graphic_pipeline.h"

#include <stdio.h>

#include "d3d11_vertex_shader.h"
#include "d3d11_pixel_shader.h"
#include "d3d11_enum_converter.h"
#include "d3d11_graphic_resource_manager.h"

#ifdef NDEBUG
	#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
	#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif

namespace s2 {

D3D11GraphicPipeline::D3D11GraphicPipeline(D3D11GraphicResourceManager *_manager)
	: 	manager(_manager), 
		new_input(true), ib(0), vbs(D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT), topology(),
		vs(0), ps(0), 
		new_rast(true), rast_state(0), 
		new_ds(true), ds_state(0), 
		new_blend(true), blend_state(0),
		new_output(true), rts(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT) {

}

D3D11GraphicPipeline::~D3D11GraphicPipeline() {
	if(rast_state)
		rast_state->Release();
	if(ds_state)
		ds_state->Release();
	if(blend_state)
		blend_state->Release();
}


bool D3D11GraphicPipeline::SetPrimitiveTopology(PrimitiveTopology newvalue) {
	new_input = true;
	topology = newvalue;
	return true;
}

GraphicPipeline::PrimitiveTopology D3D11GraphicPipeline::GetPrimitiveTopology() {
	return topology;
}

bool D3D11GraphicPipeline::SetVertexBuffer(unsigned int index, VertexBuffer *_buf, VertexBufferUsage usage, const s2string &type_name) {
	new_input = true;
	D3D11VertexBuffer *buf = NiceCast(D3D11VertexBuffer *, _buf);
	//Skip typecheck for now.
	int i = 0;
	vbs[i].vb = buf;
	vbs[i].usage = usage;
	vbs[i].type_name = type_name;
	return true;
}

D3D11VertexBuffer * D3D11GraphicPipeline::GetVertexBuffer(unsigned int index, VertexBufferUsage *usage, s2string *type_name) {
	CHECK(false)<<"Disable for now";
	return 0;
}

bool D3D11GraphicPipeline::SetIndexBuffer(IndexBuffer *_buf) {
	new_input = true;
	D3D11IndexBuffer *buf = NiceCast(D3D11IndexBuffer *, _buf);
	ib = buf;
	return true;
}

D3D11IndexBuffer * D3D11GraphicPipeline::GetIndexBuffer() {
	return ib;
}

bool D3D11GraphicPipeline::SetVertexShader(VertexShader *shader) {
	new_vs = true;
	vs = NiceCast(D3D11VertexShader *, shader);
	return true;
}

VertexShader * D3D11GraphicPipeline::GetVertexShader() {
	return vs;
}

bool D3D11GraphicPipeline::SetPixelShader(PixelShader *shader) {
	new_ps = true;
	ps = NiceCast(D3D11PixelShader *, shader);
	return true;
}

PixelShader * D3D11GraphicPipeline::GetPixelShader() {
	return ps;
}

namespace {

ID3D11RasterizerState * ParseRasterizationOption(ID3D11Device *device, const RasterizationOption &option, s2string *error) {
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
	desc.ScissorEnable = option.enable_scissor;
	ID3D11RasterizerState *state = 0;
	HRESULT result = 1;
	result = device->CreateRasterizerState(&desc, &state);
	if(FAILED(result)) {
		char buf[256];
		sprintf(buf, "Fail to create rasterization state. Error code %d.", ::GetLastError());
		*error = buf;
	}
	
	return state;
}

ID3D11DepthStencilState * ParseDepthStencilOption(ID3D11Device *device,const DepthStencilOption &option, s2string *error) {
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
	if(FAILED(result)) {
		char buf[256];
		sprintf(buf, "Fail to create depth stencil state. Error code %d.", ::GetLastError());
		*error = buf;
	}
	return state;
}

ID3D11BlendState * ParseBlendOption(ID3D11Device *device, const BlendOption &option, s2string *error) {
	D3D11_BLEND_DESC desc;
	desc.AlphaToCoverageEnable = false;		//Disabled for now, see class BlendOption.
	desc.IndependentBlendEnable= option.rt_options.size()>1;
	if(option.rt_options.size()>8) {
		char buf[256];
		sprintf(buf, "Can only handle 8 render targets. The current number is %d", option.rt_options.size());
		*error = buf;
		return 0;
	}
	
	for(unsigned int i=0; i<option.rt_options.size(); i++) {
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
	if(FAILED(result)) {
		char buf[256];
		sprintf(buf, "Fail to create blend state. Error code %d.", ::GetLastError());
		*error = buf;
	}
	return state;
}
	
}

bool D3D11GraphicPipeline::SetRasterizationOption(const RasterizationOption &option) {
	rast_opt = option;
	ID3D11RasterizerState *old_state = rast_state;
	rast_state = ParseRasterizationOption(manager->GetDevice(), rast_opt, &error);
	if(rast_state == 0) {
		rast_state = old_state;
		return false;
	} else {
		new_rast = true;
		old_state->Release();
		return true;
	}
}

void D3D11GraphicPipeline::GetRasterizationOption(RasterizationOption *option) {
	*option = rast_opt;
}

bool D3D11GraphicPipeline::SetDepthStencilOption(const DepthStencilOption &option) {
	ds_opt = option;
	ID3D11DepthStencilState *old_state = ds_state;
	ds_state = ParseDepthStencilOption(manager->GetDevice(), ds_opt, &error);
	if(ds_state == 0) {
		ds_state = old_state;
		return false;
	} else {
		new_ds = true;
		old_state->Release();
		return true;
	}
}

void D3D11GraphicPipeline::GetDepthStencilOption(DepthStencilOption *option) {
	*option = ds_opt;
}

bool D3D11GraphicPipeline::SetBlendOption(const BlendOption &option) {
	blend_opt = option;
	ID3D11BlendState *old_state = blend_state;
	blend_state = ParseBlendOption(manager->GetDevice(), blend_opt, &error);
	if(blend_state == 0) {
		blend_state = old_state;
		return false;
	} else {
		new_blend = true;
		old_state->Release();
		return true;
	}
}

void D3D11GraphicPipeline::GetBlendOption(BlendOption *option) {
	*option = blend_opt;
}

bool D3D11GraphicPipeline::SetRenderTarget(unsigned int index, Texture2D *target) {
	new_output = true;
	rts[index].tex = NiceCast(D3D11Texture2D *, target);
	return true;
}

Resource * D3D11GraphicPipeline::GetRenderTarget(unsigned int index) {
	return rts[index].tex;
}

bool D3D11GraphicPipeline::SetDepthStencilBuffer(Texture2D *buffer) {
	new_output = true;
	ds.tex = NiceCast(D3D11Texture2D *, buffer);
	return true;
}

Resource* D3D11GraphicPipeline::GetDepthStencilBuffer() {
	return ds.tex;
}

bool D3D11GraphicPipeline::SetRenderTargetClearOption(unsigned int index, bool enable, const float rgba[4]) {
	rts[index].enable_clear = enable;
	rts[index].rgba[0] = rgba[0];
	rts[index].rgba[1] = rgba[1];
	rts[index].rgba[2] = rgba[2];
	rts[index].rgba[3] = rgba[3];
	return true;
}

bool D3D11GraphicPipeline::GetRenderTargetClearOption(unsigned int index, bool *enable, float *rgba) {
	*enable = rts[index].enable_clear;
	rgba[0] = rts[index].rgba[0];
	rgba[1] = rts[index].rgba[1];
	rgba[2] = rts[index].rgba[2];
	rgba[3] = rts[index].rgba[3];
	return true;
}

void D3D11GraphicPipeline::SetDepthStencilBufferClearOption(bool enable,  float depth, uint8_t stencil) {
	ds.enable_clear = enable;
	ds.depth = depth;
	ds.stencil = stencil;
}

void D3D11GraphicPipeline::GetDepthStencilBufferClearOption(bool *enable,  float *depth, uint8_t *stencil) {
	*enable = ds.enable_clear;
	*depth = ds.depth;
	*stencil = ds.stencil;
}

void D3D11GraphicPipeline::SetRasterizationOption() {
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	RasterizationOption &option = rast_opt;
	
	if(!option.viewports.empty()) {
		D3D11_VIEWPORT *viewports = new D3D11_VIEWPORT[option.viewports.size()];
		for(unsigned int i=0; option.viewports.size(); i++) {
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
		for(unsigned int i=0; i<option.scissors.size(); i++) {
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

void D3D11GraphicPipeline::SetDepthStencilOption() {
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	context->OMSetDepthStencilState(ds_state, ds_opt.stencil_replace_value);
}

void D3D11GraphicPipeline::SetBlendOption() {
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	context->OMSetBlendState(blend_state , blend_opt.factor, 0);
}

void D3D11GraphicPipeline::SetInput() {
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	//Set primitive topology
	context->IASetPrimitiveTopology(D3D11EnumConverter::TopologyToD3D11Topology(topology));
	
	//Set vertex buffer.
	{
		int last_index = -1;
		for(int i=vbs.size()-1; i>=0; i--) {
			if(vbs[i].vb) {
				last_index = i;
				break;
			}
		}
		if(last_index != -1) {
			ID3D11Buffer **buf_array = new ID3D11Buffer *[last_index+1];
			unsigned int *stride_array = new unsigned int[last_index+1];
			unsigned int *offset_array = new unsigned int[last_index+1];
			for(int i=0; i<=last_index; i++) {
				buf_array[i] = vbs[i].vb->GetInternal();
				stride_array[i] = 4;			//This information needs to retrive from type_name, use 4 for now.
				offset_array[i] = 0;
			}
			context->IASetVertexBuffers(0, last_index+1, buf_array, stride_array, offset_array);
			delete[] stride_array;
			delete[] offset_array;
		}
	}
	
	//Set index buffer
	if(ib) {
		context->IASetIndexBuffer(ib->GetInternal(), DXGI_FORMAT_R32_UINT, 0);
	}
}

void D3D11GraphicPipeline::SetOutput() {
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	//Set render target.
	{
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

void D3D11GraphicPipeline::Draw() {
	SetInput();

	if(vs && new_vs) {
		vs->Flush();
		new_vs = false;
	}
	if(ps && new_ps) {
		ps->Flush();
		new_ps = false;
	}
	
	//Flush rasterization option.
	if(new_rast) {
		SetRasterizationOption();
		new_rast = false;
	}
	
	
	//Flush depth stencil option.
	if(new_ds) {
		SetDepthStencilOption();
		new_ds = false;
	}
	
	//Flush blend option.
	if(new_blend) {
		SetBlendOption();
		new_blend = false;
	}
	if(new_output) {
		SetOutput();
		new_output = false;
	}
	
	//Do clear.
	
	
	//Start drawing.
	
}

void D3D11GraphicPipeline::GetLastError(s2string *str) {
	*str = error;
}

}



