#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#define WIN32_LEAN_AND_MEAN
#include <dxgi.h>
#include <d3d11.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#undef ERROR

#include <glog/logging.h>
#include <stdio.h>

#include "d3d11_shader_reflection.h"

namespace s2 {

namespace {
	const char *kFloat = "float";
	const char *kInt = "int";
	const char *kUint = "uint";
	const char *kDouble = "double";
	const char *kBool = "bool";
}

void D3D11ShaderReflection::PopulateCBAndUniforms(const D3D11_SHADER_DESC &desc) {
	HRESULT result = 1;
	for(uint32_t i=0; i<desc.ConstantBuffers; i++) {
		ID3D11ShaderReflectionConstantBuffer *cb_reflect = reflect->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC cb_desc;
		result = cb_reflect->GetDesc(&cb_desc);
		CHECK(!FAILED(result))<<"Cannot get constant buffer description for"<<filepath;
		if(cb_desc.Type != D3D_CT_CBUFFER)
			continue;
		cbs.push_back(ConstantBuffer());
		ConstantBuffer &cb = cbs.back();
		
		//Find its slot
		D3D11_SHADER_INPUT_BIND_DESC cb_resource_desc;
		reflect->GetResourceBindingDescByName(cb_desc.Name, &cb_resource_desc);
		cb.slot_index = cb_resource_desc.BindPoint;
		cb.name = cb_desc.Name;
		cb.size = cb_desc.Size;
		cb.uniforms.resize(cb_desc.Variables);
		
		for(uint32_t j=0; j<cb_desc.Variables; j++) {
			ID3D11ShaderReflectionVariable *v_reflect = cb_reflect->GetVariableByIndex(j);
			D3D11_SHADER_VARIABLE_DESC v_desc;
			v_reflect->GetDesc(&v_desc);
			D3D11_SHADER_TYPE_DESC type_desc;
			ID3D11ShaderReflectionType *type = v_reflect->GetType();
			type->GetDesc(&type_desc);
			Uniform &u = uniforms[v_desc.Name];
			u.cb_slot_index = cb.slot_index;
			u.cb_name = cb.name;
			u.name = v_desc.Name;
			u.type_name = type_desc.Name;
			u.offset = v_desc.StartOffset;
			u.size = v_desc.Size;
		}
	}
}

namespace {

s2string ComponentToTypeName(D3D_REGISTER_COMPONENT_TYPE input) {
	switch(input) {
	case D3D_REGISTER_COMPONENT_UINT32:
		return kUint;
	case D3D_REGISTER_COMPONENT_SINT32:
		return kInt;
	case D3D_REGISTER_COMPONENT_FLOAT32:
		return kFloat;
	default:
		CHECK(false)<<"Unexpected type. "<<input;
		return "";
	}
}

uint32_t GetUsedComponentSize(char mask) {
	uint32_t c;
	for (c = 0; mask; c++) 
		mask &= mask - 1;
	return c*4;
}

}

void D3D11ShaderReflection::PopulateInputs(const D3D11_SHADER_DESC &desc) {
	HRESULT result = 1;
	inputs.resize(desc.InputParameters);
	for(uint32_t i=0;i<desc.InputParameters; i++) {
		Parameter &p = inputs[i];
		D3D11_SIGNATURE_PARAMETER_DESC in_desc;
		result = reflect->GetInputParameterDesc(i, &in_desc);
		CHECK(!FAILED(result))<<"Cannot get input description for"<<filepath;
		p.index = i;
		p.type_name = ComponentToTypeName(in_desc.ComponentType);
		p.size = GetUsedComponentSize(in_desc.Mask);
		p.semantic = in_desc.SemanticName;
		p.semantic_index = in_desc.SemanticIndex;
		p.stream = -1;
	}
}

void D3D11ShaderReflection::PopulateOutputs(const D3D11_SHADER_DESC &desc) {
	HRESULT result = 1;
	outputs.resize(desc.OutputParameters);
	for(uint32_t i=0;i<desc.OutputParameters; i++) {
		Parameter &p = outputs[i];
		D3D11_SIGNATURE_PARAMETER_DESC out_desc;
		result = reflect->GetOutputParameterDesc(i, &out_desc);
		CHECK(!FAILED(result))<<"Cannot get output description for"<<filepath;
		p.index = i;
		p.type_name = ComponentToTypeName(out_desc.ComponentType);
		p.size = GetUsedComponentSize(out_desc.Mask);
		p.semantic = out_desc.SemanticName;
		p.semantic_index = out_desc.SemanticIndex;
		p.stream = out_desc.Stream;
	}
}

void D3D11ShaderReflection::PopulateResources(const D3D11_SHADER_DESC &desc) {
	HRESULT result = 1;

	for(uint32_t i=0; i<desc.BoundResources; i++) {
		D3D11_SHADER_INPUT_BIND_DESC resource_desc;
		reflect->GetResourceBindingDesc(i, &resource_desc);
		
		if(resource_desc.Type == D3D_SIT_SAMPLER) {		//Consider sampler
			samplers.push_back(Sampler());
			Sampler &cur = samplers.back();
			cur.slot_index = resource_desc.BindPoint;
			cur.name = resource_desc.Name;
			cur.is_compare_sampler = ((resource_desc.uFlags | D3D_SIF_COMPARISON_SAMPLER)!=0);
		} else if(resource_desc.Type == D3D_SIT_TEXTURE) {	//Consider texture
			shader_resources.push_back(ShaderResource());
			ShaderResource &cur = shader_resources.back();
			cur.slot_index = resource_desc.BindPoint;
			cur.name = resource_desc.Name;
			cur.type = D3D11ShaderReflection::TEXTURE;
		}
	}
}

namespace {

uint32_t Pack4Byte(uint32_t size) {
	return size%4==0 ? size : (size/4+1)*4;
}

s2string ParseArrayTypeName(const s2string &type_name, uint32_t length) {
	s2string buf;
	S2StringFormat(&buf, "%s[%d]", type_name.c_str(), length);
	return buf;
}

s2string GetScalarTypeName(D3D_SHADER_VARIABLE_TYPE type) {
	switch(type) {
	case D3D_SVT_BOOL:
		return kBool;
	case D3D_SVT_INT:
		return kInt;
	case D3D_SVT_UINT:
		return kUint;
	case D3D_SVT_FLOAT:
		return kFloat;
	case D3D_SVT_DOUBLE:
		return kDouble;
	default:
		CHECK(false)<<"Unsupported "<<type;
		return "";
	}
}

}

D3D11ShaderReflection::D3D11ShaderReflection(const s2string &_filepath, ID3DBlob *shader_blob)
	: filepath(_filepath), reflect(0) {
	HRESULT result = 1;
	result = D3DReflect( shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), 
		IID_ID3D11ShaderReflection, (void**) &reflect);
	CHECK(!FAILED(result))<<"Cannot get shader reflection for "<<_filepath;	
	
	D3D11_SHADER_DESC desc;
	result = reflect->GetDesc(&desc);
	CHECK(!FAILED(result))<<"Cannot get shader reflection desc for "<<_filepath;	

	PopulateCBAndUniforms(desc);
	PopulateInputs(desc);
	PopulateOutputs(desc);
	PopulateResources(desc);
}


D3D11ShaderReflection::~D3D11ShaderReflection() {
	reflect->Release();
}

const D3D11ShaderReflection::ConstantBuffer & D3D11ShaderReflection::GetConstantBuffer(uint32_t index) const {
	return cbs[index];
}

uint32_t D3D11ShaderReflection::GetConstantBufferSize() const {
	return cbs.size();
}

//All the thing in constant buffer(cbuffer or tbuffer)
const D3D11ShaderReflection::Uniform & D3D11ShaderReflection::GetUniform(const s2string &name) const {
	return uniforms.at(name);
}

bool D3D11ShaderReflection::HasUniform(const s2string &name) const {
	return uniforms.find(name) != uniforms.end();
}

const D3D11ShaderReflection::Parameter & D3D11ShaderReflection::GetInput(uint32_t index) const {
	return inputs[index];
}

uint32_t D3D11ShaderReflection::GetInputSize() const {
	return inputs.size();
}

const D3D11ShaderReflection::Parameter & D3D11ShaderReflection::GetOutput(uint32_t index) const {
	return outputs[index];
}

uint32_t D3D11ShaderReflection::GetOutputSize() const {
	return outputs.size();
}

const D3D11ShaderReflection::Sampler & D3D11ShaderReflection::GetSampler(uint32_t index) const {
	return samplers[index];
}

const D3D11ShaderReflection::Sampler & D3D11ShaderReflection::GetSampler(const s2string &name) const {
	return samplers[GetSamplerIndex(name)];
}

bool D3D11ShaderReflection::HasSampler(const s2string &name) const {
	for(uint32_t i=0; i<samplers.size(); i++) {
		if(samplers[i].name == name) {
			return true;
		}
	}
	return false;
}

uint32_t D3D11ShaderReflection::GetSamplerIndex(const s2string  &name) const {
	for(uint32_t i=0; i<samplers.size(); i++) {
		if(samplers[i].name == name) {
			return i;
		}
	}
	CHECK(false)<<"Cannot find sampler "<<name;
	
	return 0;
}

uint32_t D3D11ShaderReflection::GetSamplerSize() const {
	return samplers.size();
}


const D3D11ShaderReflection::ShaderResource & D3D11ShaderReflection::GetShaderResource(uint32_t index) const {
	return shader_resources[index];
}

const D3D11ShaderReflection::ShaderResource & D3D11ShaderReflection::GetShaderResource(const s2string &name) const {
	return shader_resources[GetShaderResourceIndex(name)];
}

bool D3D11ShaderReflection::HasShaderResource(const s2string &name) const {
	for(uint32_t i=0; i<shader_resources.size(); i++) {
		if(shader_resources[i].name == name) {
			return true;
		}
	}
	return false;
}

uint32_t D3D11ShaderReflection::GetShaderResourceIndex(const s2string &name) const {
	for(uint32_t i=0; i<shader_resources.size(); i++) {
		if(shader_resources[i].name == name) {
			return i;
		}
	}
	CHECK(false)<<"Cannot find shader resource "<<name;
	return 0;
}

uint32_t D3D11ShaderReflection::GetShaderResourceSize() const {
	return shader_resources.size();
}

namespace {

bool IsArray(const s2string &type_name) {
	return type_name.find('[') != std::string::npos;
}

}

}









