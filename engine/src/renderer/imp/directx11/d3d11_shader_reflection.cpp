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

namespace {

s2string ParseArrayTypeName(const s2string &type_name, unsigned int length) {
	char buf[512];
	CHECK(sprintf(buf, "%s[%d]", type_name.c_str(), length)>=0)<<"sprintf catches fire.";
	return buf;
}

unsigned int Pack4Byte(unsigned int size) {
	return size%4==0 ? size : (size/4+1)*4;
}

}

//This handles not array type (scalar, vector, matrix ) regardlessly.
void D3D11ShaderReflection::_ParseShaderType(
				ID3D11ShaderReflectionType &type, 
				const D3D11_SHADER_TYPE_DESC &desc, 
				D3D11ShaderReflection::ShaderTypeInfo *_info) {
	D3D11ShaderReflection::ShaderTypeInfo &info = *_info;
	
	info.name = desc.Name;
	info.members.clear();
	for(unsigned int i=0; i<desc.Members; i++) {
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
	
	const ShaderTypeInfo &last_member = GetTypeInfo(info.members.back());
	info.size = Pack4Byte(info.members.back().offset + last_member.size);
}


void D3D11ShaderReflection::ParseShaderType(ID3D11ShaderReflectionType &type) {
	D3D11_SHADER_TYPE_DESC desc;
	HRESULT result = type.GetDesc(&desc);
	CHECK(!FAILED(result))<<"Fail to get shader variable type info.";
	
	if(!HasTypeInfo(desc.Name)) {
		ShaderTypeInfo &info = types[desc.Name];
		_ParseShaderType(type, desc, &info);
	}
	
	if(desc.Elements > 0 ) {									//Add array type too
		s2string array_typename(ParseArrayTypeName(desc.Name, desc.Elements));
		if(!HasTypeInfo(array_name)) {
			ShaderTypeInfo &array_typeinfo = types[array_typename];
			array_typeinfo.name = array_typename;
			array_typeinfo.size = Pack4Byte(info.size) * desc.Elements;
			char buf[512];
			array_typeinfo.members.clear();
			for(unsigned int i=0; i<desc.Elements; i++) {
				ShaderTypeInfo::Member m;
				sprintf(buf,"%d", i);
				m.name = buf;
				m.type_name = info.type_name;
				m.offset = Pack4Byte(info.size) * i;
				array_typeinfo.members.push_back(m);
			}
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
	PopulateSamplers(desc);
}


D3D11ShaderReflection::~D3D11ShaderReflection() {
	reflect->Release();
}

const D3D11ShaderReflection::ConstantBuffer & D3D11ShaderReflection::GetConstantBuffer(unsigned int index) const {
	return cbs[index];
}

unsigned int D3D11ShaderReflection::GetConstantBufferSize() const {
	return cbs.size();
}

//All the thing in constant buffer(cbuffer or tbuffer)
const D3D11ShaderReflection::Uniform & D3D11ShaderReflection::GetUniform(const s2string &name) const {
	return uniforms[name];
}

bool D3D11ShaderReflection::HasUniform(const s2string &name) const {
	return uniforms.find(name) != uniforms.end();
}

const D3D11ShaderReflection::Parameter & D3D11ShaderReflection::GetInput(unsigned int index) const {
	return inputs[index];
}

unsigned int D3D11ShaderReflection::GetInputSize() const {
	return inputs.size();
}

const D3D11ShaderReflection::Parameter & D3D11ShaderReflection::GetOutput(unsigned int index) const {
	return outputs[index];
}

unsigned int D3D11ShaderReflection::GetOutputSize() const {
	return outputs.size();
}

const D3D11ShaderReflection::Sampler & D3D11ShaderReflection::GetSampler(const s2string &name) const {
	//STUB
	CHECK(false)<<"Disabled";
	return Sampler();
}

bool D3D11ShaderReflection::HasSampler(const s2string &name) const {
	//STUB
	CHECK(false)<<"Disabled";
	return false;
}


const D3D11ShaderReflection::Resource & D3D11ShaderReflection::GetResource(const s2string &name) const {
	//STUB
	CHECK(false)<<"Disabled";
	return Resource();
}

bool D3D11ShaderReflection::HasResource(const s2string &name) const {
	//STUB
	CHECK(false)<<"Disabled";
	return false;
}

bool D3D11ShaderReflection::CheckCompatible(const s2string &shader_typename, const TypeInfo &cpp_type) const {
	return 	type_store.CheckCompatible(shader_typename, cpp_type) || 
				GetPrimitiveTypeInfoStore()->CheckCompatible(shader_typename, cpp_type) ;
}

const ShaderTypeInfo & D3D11ShaderReflection::GetTypeInfo(const s2string &shader_typename) const {
	if(type_store.HasTypeInfo(shader_typename))
		return type_store.GetTypeInfo(shader_typename);
	else
		return GetPrimitiveTypeInfoStore()->GetTypeInfo(shader_typename);
}

bool D3D11ShaderReflection::HasTypeInfo(const s2string &shader_typename) const {
	return 	type_store.HasTypeInfo(shader_typename) || 
				GetPrimitiveTypeInfoStore()->HasTypeInfo(shader_typename) ;
}


ShaderTypeInfoStore * D3D11ShaderReflection::GetPrimitiveTypeInfoStore() {
	static ShaderTypeInfo store;
	return &store;
}


bool ShaderTypeInfoStore::CheckCompatible(const s2string &shader_typename, const TypeInfo &cpp_type) const {
	if(compatibles.find(shader_typename)!=compatibles.end()) {
		CompatibleMap::value_type &vec = compatibles[shader_typename];
		return std::find(vec.begin(), vec.end(), cpp_type) != vec.end(); 
	}
	return false;
}

void ShaderTypeInfoStore::MakeCompatible(const s2string &shader_typename, const TypeInfo &cpp_type) {
	CompatibleMap::value_type &vec = compatibles[shader_typename];
	if(std::find(vec.begin(), vec.end(), cpp_type) == vec.end())
		vec.push_back(cpp_type);
}

const ShaderTypeInfo & ShaderTypeInfoStore::GetTypeInfo(const s2string &shader_typename) const {
	return types[shader_typename];
}

bool ShaderTypeInfoStore::HasTypeInfo(const s2string &shader_typename) const {
	return types.find(shader_typename);
}

ShaderTypeInfo * ShaderTypeInfoStore::CreateTypeInfo(const s2string &shader_typename) {
	CHECK(HasTypeInfo(shader_typename)==false)<<"TypeInfo "<<shader_typename<<" is already there.";
	return &types[shader_typename];
}

}









