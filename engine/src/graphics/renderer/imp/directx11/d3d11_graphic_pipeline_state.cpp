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

D3D11GraphicPipelineState::D3D11GraphicPipelineState(D3D11GraphicResourceManager *_manager, bool shell_only) {
	manager = _manager;
	CHECK_NOTNULL(manager);

	input_layout = 0;
	vs = 0;
	ps = 0;
	gs = 0;

	rast_state= 0;
	ds_state=0;
	blend_state=0;

	if(shell_only == false) {
		const RendererSetting &renderer_setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
		RasterizationOption option;
		option.viewports.clear();
		option.viewports.push_back(RasterizationOption::Rectangle(0.0f, 0.0f, 
									(float)renderer_setting.window_width, (float)renderer_setting.window_height));
	
		SetRasterizationOption(rast_opt);
	
		SetDepthStencilOption(DepthStencilOption());
	
		SetBlendOption(BlendOption());
	}
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


void D3D11GraphicPipelineState::SetInputLayout(InputLayout *newvalue) {
	input_layout = newvalue;	
}

InputLayout * D3D11GraphicPipelineState::GetInputLayout() const {
	return input_layout;
}

void D3D11GraphicPipelineState::SetVertexShader(VertexShader *shader) {
	vs = shader;
}

VertexShader * D3D11GraphicPipelineState::GetVertexShader() const {
	return vs;
}


void D3D11GraphicPipelineState::SetPixelShader(PixelShader *shader) {
	ps = shader;
}

PixelShader * D3D11GraphicPipelineState::GetPixelShader() const {
	return ps;
}

void D3D11GraphicPipelineState::SetGeometryShader(GeometryShader *shader) {
	gs = shader;
}

GeometryShader * D3D11GraphicPipelineState::GetGeometryShader() const {
	return gs;
}

void D3D11GraphicPipelineState::SetRasterizationOption(const RasterizationOption &option) {
	rast_opt = option;
	if(rast_state) {
		rast_state->Release();
	}
	rast_state = D3D11GraphicPipeline::ParseRasterizationOption(manager->GetDevice(), rast_opt);
}

void D3D11GraphicPipelineState::SetRasterizationState(const RasterizationOption &option, ID3D11RasterizerState * state) {
	rast_opt = option;
	if(rast_state) {
		rast_state->Release();
	}
	if(state) {
		state->AddRef();
	}
	
	rast_state = state;
}

const RasterizationOption & D3D11GraphicPipelineState::GetRasterizationOption() const {
	return rast_opt;
}

void D3D11GraphicPipelineState::SetDepthStencilOption(const DepthStencilOption &option) {
	ds_opt = option;
	if(ds_state) {
		ds_state->Release();
	}
	ds_state = D3D11GraphicPipeline::ParseDepthStencilOption(manager->GetDevice(), ds_opt);
}

void D3D11GraphicPipelineState::SetDepthStencilState(const DepthStencilOption &option, ID3D11DepthStencilState *state) {
	ds_opt = option;
	if(ds_state) {
		ds_state->Release();
	}
	if(state) {
		state->AddRef();
	}
	ds_state = state;
}

const DepthStencilOption & D3D11GraphicPipelineState::GetDepthStencilOption() const {
	return ds_opt;
}

void D3D11GraphicPipelineState::SetBlendOption(const BlendOption &option) {
	blend_opt = option;
	if(blend_state) {
		blend_state->Release();
	}
	blend_state = D3D11GraphicPipeline::ParseBlendOption(manager->GetDevice(), blend_opt);
}

void D3D11GraphicPipelineState::SetBlendState(const BlendOption &option, ID3D11BlendState *state) {
	blend_opt = option;
	if(blend_state) {
		blend_state->Release();
	}
	if(state) {
		state->AddRef();
	}

	blend_state = state;
}

const BlendOption & D3D11GraphicPipelineState::GetBlendOption() const {
	return blend_opt;
}

}



