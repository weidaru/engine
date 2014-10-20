#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
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
#include <algorithm>

#include "d3d11_shader_reflection.h"

namespace s2 {

void D3D11ShaderReflection::PopulateCBAndUniforms(const D3D11_SHADER_DESC &desc) {
	HRESULT result = 1;
	cbs.resize(desc.ConstantBuffers);
	for(unsigned int i=0; i<desc.ConstantBuffers; i++) {
		ConstantBuffer &cb = cbs[i];
		ID3D11ShaderReflectionConstantBuffer *cb_reflect = reflect->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC cb_desc;
		result = cb_reflect->GetDesc(&cb_desc);
		CHECK(!FAILED(result))<<"Cannot get constant buffer description for"<<filepath;
		cb.index = i;
		cb.name = cb_desc.Name;
		cb.size = cb_desc.Size;
		cb.uniforms.resize(cb_desc.Variables);
		
		for(unsigned int j=0; j<cb_desc.Variables; j++) {
			ID3D11ShaderReflectionVariable *v_reflect = cb_reflect->GetVariableByIndex(j);
			D3D11_SHADER_VARIABLE_DESC v_desc;
			v_reflect->GetDesc(&v_desc);
			D3D11_SHADER_TYPE_DESC type_desc;
			ID3D11ShaderReflectionType *type = v_reflect->GetType();
			type->GetDesc(&type_desc);
			Uniform &u = uniforms[v_desc.Name];
			u.cb_index = cb.index;
			u.cb_name = cb.name;
			u.name = v_desc.Name;
			u.type_name = type_desc.Name;
			u.offset = v_desc.StartOffset;
			u.size = v_desc.Size;
			
			ParseShaderType(*type);
		}
	}
}

namespace {

s2string ComponentToTypeName(D3D_REGISTER_COMPONENT_TYPE input) {
	switch(input) {
	case D3D_REGISTER_COMPONENT_UINT32:
		return "unsigned int";
	case D3D_REGISTER_COMPONENT_SINT32:
		return "int";
	case D3D_REGISTER_COMPONENT_FLOAT32:
		return "float";
	default:
		CHECK(false)<<"Unexpected type. "<<input;
		return "";
	}
}

unsigned int GetUsedComponentSize(char mask) {
	unsigned int c;
	for (c = 0; mask; c++) 
		mask &= mask - 1;
	return c*4;
}

}

void D3D11ShaderReflection::PopulateInputs(const D3D11_SHADER_DESC &desc) {
	HRESULT result = 1;
	inputs.resize(desc.InputParameters);
	for(unsigned int i=0;i<desc.InputParameters; i++) {
		Parameter &p = inputs[i];
		D3D11_SIGNATURE_PARAMETER_DESC in_desc;
		result = reflect->GetInputParameterDesc(i, &in_desc);
		CHECK(!FAILED(result))<<"Cannot get input description for"<<filepath;
		p.index = i;
		p.type_name = ComponentToTypeName(in_desc.ComponentType);
		p.size = GetUsedComponentSize(in_desc.Mask);
		p.semantic = in_desc.SemanticName;
		p.semantic_index = in_desc.SemanticIndex;
	}
}

void D3D11ShaderReflection::PopulateOutputs(const D3D11_SHADER_DESC &desc) {
	HRESULT result = 1;
	inputs.resize(desc.OutputParameters);
	for(unsigned int i=0;i<desc.OutputParameters; i++) {
		Parameter &p = inputs[i];
		D3D11_SIGNATURE_PARAMETER_DESC out_desc;
		result = reflect->GetOutputParameterDesc(i, &out_desc);
		CHECK(!FAILED(result))<<"Cannot get output description for"<<filepath;
		p.index = i;
		p.type_name = ComponentToTypeName(out_desc.ComponentType);
		p.size = GetUsedComponentSize(out_desc.Mask);
		p.semantic = out_desc.SemanticName;
		p.semantic_index = out_desc.SemanticIndex;
	}
}

void D3D11ShaderReflection::PopulateSamplers(const D3D11_SHADER_DESC &desc) {
	//STUB
}

void D3D11ShaderReflection::PopulateResources(const D3D11_SHADER_DESC &desc) {
	//STUB
}

D3D11ShaderReflection::TypeMap * D3D11ShaderReflection::GetPrimitiveTypeMap() {
	static TypeMap map;
	return &map;
}

D3D11ShaderReflection::CompatibleMap * D3D11ShaderReflection::GetCompatibleMap() {
	static CompatibleMap map;
	return &map;
}

void D3D11ShaderReflection::ParseShaderType(ID3D11ShaderReflectionType &type) {
	D3D11_SHADER_TYPE_DESC desc;
	HRESULT result = type.GetDesc(&desc);
	CHECK(!FAILED(result))<<"Fail to get shader variable type info.";
	
	TypeMap &primitive_types = *GetPrimitiveTypeMap();
	if(types.find(desc.Name) != types.end() || primitive_types.find(desc.Name) != primitive_types.end())
		return;
	ShaderTypeInfo info = types[desc.Name];
	info.name = desc.Name;
	/* HLSL Packing things. Consider that.
	info.size = desc.Rows * desc.Columns * 4;
	if(desc.Elements > 0)
		info.size *= desc.Elements;
	*/
	info.members.clear();
	for(unsigned int i=0; i<desc.Elements; i++) {
		ShaderTypeInfo::Member m;
		ID3D11ShaderReflectionType *member_type = type.GetMemberTypeByIndex(i);
		D3D11_SHADER_TYPE_DESC member_desc;
		member_type->GetDesc(&member_desc);
		m.name = type.GetMemberTypeName(i);
		m.type_name = member_desc.Name;
		m.offset = member_desc.Offset;
		info.members.push_back(m);
		
		ParseShaderType(*member_type);
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
	PopulateSamplers(desc);
}


D3D11ShaderReflection::~D3D11ShaderReflection() {

}

const D3D11ShaderReflection::ConstantBuffer & D3D11ShaderReflection::GetConstantBuffer(unsigned int index) {
	return cbs[index];
}

unsigned int D3D11ShaderReflection::GetConstantBufferSize() {
	return cbs.size();
}

//All the thing in constant buffer(cbuffer or tbuffer)
const D3D11ShaderReflection::Uniform & D3D11ShaderReflection::GetUniform(const s2string &name) {
	return uniforms[name];
}

bool D3D11ShaderReflection::HasUniform(const s2string &name) {
	return uniforms.find(name) != uniforms.end();
}

const D3D11ShaderReflection::Parameter & D3D11ShaderReflection::GetInput(unsigned int index) {
	return inputs[index];
}

unsigned int D3D11ShaderReflection::GetInputSize() {
	return inputs.size();
}

const D3D11ShaderReflection::Parameter & D3D11ShaderReflection::GetOutput(unsigned int index) {
	return outputs[index];
}

unsigned int D3D11ShaderReflection::GetOutputSize() {
	return outputs.size();
}

const D3D11ShaderReflection::Sampler & D3D11ShaderReflection::GetSampler(const s2string &name) {
	//STUB
	CHECK(false)<<"Disabled";
	return Sampler();
}

bool D3D11ShaderReflection::HasSampler(const s2string &name) {
	//STUB
	CHECK(false)<<"Disabled";
	return false;
}


const D3D11ShaderReflection::Resource & D3D11ShaderReflection::GetResource(const s2string &name) {
	//STUB
	CHECK(false)<<"Disabled";
	return Resource();
}

bool D3D11ShaderReflection::HasResource(const s2string &name) {
	//STUB
	CHECK(false)<<"Disabled";
	return false;
}





}









