#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_graphic_pipeline.h"

#include <stdio.h>
#include <set>

#include "d3d11_vertex_shader.h"
#include "d3d11_pixel_shader.h"
#include "d3d11_enum_converter.h"
#include "d3d11_graphic_resource_manager.h"
#include "d3d11_shader_helper.h"

#include "engine.h"

#ifdef NDEBUG
	#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
	#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif

namespace s2 {

D3D11GraphicPipeline::D3D11GraphicPipeline(D3D11GraphicResourceManager *_manager)
	: 	manager(_manager), 
		input_stage(_manager),
		rast_state(0),
		ds_state(0),
		blend_state(0),
		output_stage(_manager),
		active_vs_srs(D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT ), 
		active_ps_srs(D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT ), 
		active_rts(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT){
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
	if(rast_state)
		rast_state->Release();
	new_rast=true;
	rast_state=0;
	
	if(ds_state)
		ds_state->Release();
	new_ds=true;
	ds_state=0; 
	
	if(blend_state)
		blend_state->Release();
	new_blend=true;
	blend_state=0;
}

void D3D11GraphicPipeline::SetPrimitiveTopology(PrimitiveTopology newvalue) {
	input_stage.SetPrimitiveTopology(newvalue);
}

GraphicPipeline::PrimitiveTopology D3D11GraphicPipeline::GetPrimitiveTopology() {
	return input_stage.GetPrimitiveTopology();
}

void D3D11GraphicPipeline::SetVertexBuffer(unsigned int index, unsigned int start_input_index, VertexBuffer *buf) {
	input_stage.SetVertexBuffer(index, start_input_index, buf);
}

D3D11VertexBuffer * D3D11GraphicPipeline::GetVertexBuffer(unsigned int index, unsigned int *start_input_index) {
	return input_stage.GetVertexBuffer(index, start_input_index);
}

void D3D11GraphicPipeline::SetIndexBuffer(IndexBuffer *_buf) {
	input_stage.SetIndexBuffer( _buf);
}

D3D11IndexBuffer * D3D11GraphicPipeline::GetIndexBuffer() {
	return input_stage.GetIndexBuffer();
}

void D3D11GraphicPipeline::SetVertexShader(VertexShader *shader) {
	vs = NiceCast(D3D11VertexShader *, shader);
	if(shader) {
		CHECK(vs)<<"Shader cannot be cast to D3D11VertexShader";
	}
}

VertexShader * D3D11GraphicPipeline::GetVertexShader() {
	return vs;
}

void D3D11GraphicPipeline::SetPixelShader(PixelShader *shader) {
	ps = NiceCast(D3D11PixelShader *, shader);
	if(shader) {
		CHECK(ps)<<"Shader cannot be cast to D3D11PixelShader";
	}
}

PixelShader * D3D11GraphicPipeline::GetPixelShader() {
	return ps;
}

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
	CHECK(!FAILED(result))<<"Fail to create blend state. Error code "<< ::GetLastError();
	return state;
}
	
}

void D3D11GraphicPipeline::SetRasterizationOption(const RasterizationOption &option) {
	rast_opt = option;
	if(rast_state)
		rast_state->Release();
	rast_state = ParseRasterizationOption(manager->GetDevice(), rast_opt);
	new_rast = true;
}

const RasterizationOption & D3D11GraphicPipeline::GetRasterizationOption() const {
	return rast_opt;
}

void D3D11GraphicPipeline::SetDepthStencilOption(const DepthStencilOption &option) {
	ds_opt = option;
	if(ds_state)
		ds_state->Release();
	ds_state = ParseDepthStencilOption(manager->GetDevice(), ds_opt);
	new_ds = true;
}

const DepthStencilOption & D3D11GraphicPipeline::GetDepthStencilOption() const {
	return ds_opt;
}

void D3D11GraphicPipeline::SetBlendOption(const BlendOption &option) {
	blend_opt = option;
	if(blend_state)
		blend_state->Release();
	blend_state = ParseBlendOption(manager->GetDevice(), blend_opt);
	new_blend = true;
}

const BlendOption & D3D11GraphicPipeline::GetBlendOption() const {
	return blend_opt;
}

void D3D11GraphicPipeline::SetRenderTarget(unsigned int index, Texture2D *target) {
	output_stage.SetRenderTarget(index, target);
}

Resource * D3D11GraphicPipeline::GetRenderTarget(unsigned int index) {
	return output_stage.GetRenderTarget(index);
}

void D3D11GraphicPipeline::SetDepthStencilBuffer(Texture2D *buffer) {
	output_stage.SetDepthStencilBuffer(buffer);
}

Resource* D3D11GraphicPipeline::GetDepthStencilBuffer() {
	return output_stage.GetDepthStencilBuffer();
}

void D3D11GraphicPipeline::SetupRasterizationOption() {
	CHECK(rast_state);
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	RasterizationOption &option = rast_opt;
	
	if(!option.viewports.empty()) {
		D3D11_VIEWPORT *viewports = new D3D11_VIEWPORT[option.viewports.size()];
		for(unsigned int i=0; i<option.viewports.size(); i++) {
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

void D3D11GraphicPipeline::SetupDepthStencilOption() {
	CHECK(ds_state);
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	context->OMSetDepthStencilState(ds_state, ds_opt.stencil_replace_value);
	
}

void D3D11GraphicPipeline::SetupBlendOption() {
	CHECK(blend_state);
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	context->OMSetBlendState(blend_state , blend_opt.factor, blend_opt.sample_mask);
}

bool D3D11GraphicPipeline::Validate(s2string *error) const {
	return true;
}

void D3D11GraphicPipeline::ClearRenderTarget(Texture2D *texture, const float rgba[4]) {
	output_stage.ClearRenderTarget(texture, rgba);
}

void D3D11GraphicPipeline::ClearDepthStencilBuffer(Texture2D *texture, bool clear_depth, float depth, bool clear_stencil, int stencil) {
	output_stage.ClearDepthStencilBuffer(texture, clear_depth, depth , clear_stencil, stencil);
}
	
namespace {

typedef std::vector<std::pair<unsigned int, Resource *> > BindingVec;

bool Contains(const BindingVec &vec, Resource *resource) {
	for(unsigned int i=0; i<vec.size(); i++) {
		if(vec[i].second == resource) {
			return true;
		}
	}
	return false;
}

}

D3D11GraphicPipeline::BindingMap::BindingMap(unsigned int size) {
	vec.resize(size, 0);
}

bool D3D11GraphicPipeline::BindingMap::Contains(unsigned int index) {
	return vec[index] == 0;
}

bool D3D11GraphicPipeline::BindingMap::Contains(Resource *resource) {
	return map.find(resource) != map.end();
}

D3D11GraphicPipeline::BindingMap & D3D11GraphicPipeline::BindingMap::Remove(unsigned int index) {
	if(Contains(index)) {
		map.erase(vec[index]);
		vec[index] = 0;
	}
	return *this;
}

D3D11GraphicPipeline::BindingMap & D3D11GraphicPipeline::BindingMap::Remove(Resource *resource) {
	if(Contains(resource)) {
		vec[map[resource]] = 0;
		map.erase(resource);
	}
	return *this;
}

D3D11GraphicPipeline::BindingMap & D3D11GraphicPipeline::BindingMap::Add(unsigned int index, Resource *resource) {
	CHECK(resource)<<"Resource should not be 0";
	vec[index] = resource;
	map[resource] = index;
	return *this;
}

Resource * D3D11GraphicPipeline::BindingMap::GetResource(unsigned int index) {
	return vec[index];
}

unsigned int D3D11GraphicPipeline::BindingMap::GetIndex(Resource *resource) {
	CHECK(Contains(resource))<<"Resource is not in map";
	return map[resource];
}
	
void D3D11GraphicPipeline::Draw(unsigned int vertex_count, unsigned int instance_count) {
	//Find all new bindings.
	BindingVec new_rts;
	BindingVec new_vs_srs;
	BindingVec new_ps_srs;
	
	new_rts = output_stage.GetNewRenderTargetBindings();
	if(vs) {
		new_vs_srs = vs->GetShaderResourceContainer().GetNewBindings();
	}
	if(ps) {
		new_ps_srs = ps->GetShaderResourceContainer().GetNewBindings();
	}
	
	ID3D11DeviceContext *context =  manager->GetDeviceContext();
	
	std::vector<BindingVec *> shader_sr_vectors;
	shader_sr_vectors.push_back(&new_vs_srs);
	shader_sr_vectors.push_back(&new_ps_srs);
	
	//See whether we need to clear some old render target
	for(unsigned int i=0; i<shader_sr_vectors.size(); i++) {
		ShaderResourceContainer::BindingVector &vec = *shader_sr_vectors[i];
		for(unsigned int j=0; j<vec.size(); j++) {
			Resource *resource = vec[j].second;
			if(active_rts.Contains(resource) && Contains(new_rts, resource)==false) {
				SetRenderTarget(active_rts.GetIndex(resource), 0);
			} else if(Contains(new_rts, resource)) {
				LOG(ERROR)<<"Bind resource "<<resource->GetIDAndName()<<
					" as  both  shader resource and render target will result in undefined behaviour.";
			}
		}	
	}

	//See whether we need to clear some old shader resources.
	for(unsigned int i=0; i<new_rts.size(); i++) {
		Resource *resource = new_rts[i].second;
		if(active_vs_srs.Contains(resource)&& Contains(new_vs_srs, resource)==false) {
			ID3D11ShaderResourceView *view = 0;
			context->VSSetShaderResources(active_vs_srs.GetIndex(resource), 1, &view);
		}
		if(active_ps_srs.Contains(resource) && Contains(new_ps_srs, resource)==false) {
			ID3D11ShaderResourceView *view = 0;
			context->PSSetShaderResources(active_ps_srs.GetIndex(resource), 1, &view);
		}
	}
	
	//Refresh the new rt bindings.
	new_rts = output_stage.GetNewRenderTargetBindings();
	
	//Push new bindings to active bindings.
	std::vector<BindingVec *> new_things;
	new_things.push_back(&new_rts);
	new_things.push_back(&new_vs_srs);
	new_things.push_back(&new_ps_srs);
	std::vector<BindingMap *> old_things;
	old_things.push_back(&active_rts);
	old_things.push_back(&active_vs_srs);
	old_things.push_back(&active_ps_srs);
	CHECK(new_things.size() == old_things.size());
	
	for(unsigned int i=0; i<new_things.size(); i++) {
		BindingVec &new_thing = *new_things[i];
		BindingMap &old_thing = *old_things[i];
		for(unsigned int j=0; j<new_thing.size(); j++) {
			Resource *rt = new_thing[j].second;
			unsigned int index = new_thing[j].first;
			if(rt) {
				old_thing.Add(index, rt);
			} else {
				old_thing.Remove(index);
			}
		}	
	}
	
	//Setup input
	if(vs) {
		input_stage.Setup(vs);
	}

	//Setup output
	output_stage.Setup();
	
	if(vs) {
		vs->Setup();
	} else {
		context->VSSetShader(0, 0, 0);
	}
	
	if(ps) {
		ps->Setup();
	} else {
		context->PSSetShader(0, 0, 0);
	}
	
	//Setup rasterization option.
	if(new_rast) {
		SetupRasterizationOption();
		new_rast = false;
	}
	
	//Setup depth stencil option.
	if(new_ds) {
		SetupDepthStencilOption();
		new_ds = false;
	}
	
	//Setup blend option.
	if(new_blend) {
		SetupBlendOption();
		new_blend = false;
	}
	
	//Flush data in input stage and start drawing.
	input_stage.Flush(vertex_count, instance_count);
}


}



