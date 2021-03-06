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
			
			ParseShaderType(*type);
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

//This handles not array type (scalar, vector, matrix ) regardlessly.
void D3D11ShaderReflection::_ParseShaderType(
				ID3D11ShaderReflectionType &type, 
				const D3D11_SHADER_TYPE_DESC &desc) {
	if(desc.Class == D3D_SVC_STRUCT) {
		TypeInfo::Members members;
		
		s2string name = desc.Name;
		for(uint32_t i=0; i<desc.Members; i++) {
			TypeInfo::Member m;
			ID3D11ShaderReflectionType *member_type = type.GetMemberTypeByIndex(i);
			D3D11_SHADER_TYPE_DESC member_desc;
			member_type->GetDesc(&member_desc);
			m.name = type.GetMemberTypeName(i);
			m.type_name = member_desc.Name;
			m.offset = member_desc.Offset;
			members.push_back(m);
			
			ParseShaderType(*member_type);
		}
		
		const TypeInfo &last_member = GetTypeInfo(members.back().type_name);
		uint32_t size = Pack4Byte(members.back().offset + last_member.GetSize());
		typeinfo_manager.CreateStruct(name, size, members);
	} else if(desc.Class == D3D_SVC_VECTOR) {
		s2string name= desc.Name;
		s2string scalar_typename = GetScalarTypeName(desc.Type);
		
		typeinfo_manager.CreateVector(name, scalar_typename, desc.Columns);
	} else if(desc.Class ==  D3D_SVC_MATRIX_ROWS) {
		s2string name = desc.Name;
		s2string scalar_typename = GetScalarTypeName(desc.Type);
		typeinfo_manager.CreateMatrix(name, scalar_typename, desc.Rows, desc.Columns);
	} else{
		CHECK(false)<<"Unsupported "<<desc.Type;
	}

}


void D3D11ShaderReflection::ParseShaderType(ID3D11ShaderReflectionType &type) {
	D3D11_SHADER_TYPE_DESC desc;
	HRESULT result = type.GetDesc(&desc);
	CHECK(!FAILED(result))<<"Fail to get shader variable type info.";
	
	if(!HasTypeInfo(desc.Name)) {
		_ParseShaderType(type, desc);
	}
	
	if(desc.Elements > 0 ) {									//Add array type too
		s2string array_typename(ParseArrayTypeName(desc.Name, desc.Elements));
		if(!HasTypeInfo(array_typename)) {
			typeinfo_manager.CreateArray(array_typename, desc.Name, desc.Elements);
		}
	}
}

void D3D11ShaderReflection::PopulateScalarTypes() {
	CHECK(sizeof(float)==4)<<"Opps, float is not 4 bytes!!";
	CHECK(sizeof(int)==4)<<"Opps, int is not 4 bytes!!";
	CHECK(sizeof(uint32_t)==4)<<"Opps, uint32_t is not 4 bytes!!";
	CHECK(sizeof(double)==8)<<"Opps, double is not 8 bytes!!";

	typeinfo_manager.CreateScalar("bool", 4);
	typeinfo_manager.MakeCompatible("bool", "bool");
	typeinfo_manager.MakeCompatible("bool", "int");
	typeinfo_manager.MakeCompatible("bool", "uint32_t");
	
	typeinfo_manager.CreateScalar("int", 4);
	typeinfo_manager.MakeCompatible("int", "int");

	typeinfo_manager.CreateScalar("uint", 4);
	typeinfo_manager.MakeCompatible("uint", "uint32_t");

	typeinfo_manager.CreateScalar("float", 4);
	typeinfo_manager.MakeCompatible("float", "float");

	typeinfo_manager.CreateScalar("double", 8);
	typeinfo_manager.MakeCompatible("double", "double");
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
	
	PopulateScalarTypes();

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
	for(uint32_t i=0; i<samplers.size(); i++) {
		if(shader_resources[i].name == name) {
			return true;
		}
	}
	return false;
}

uint32_t D3D11ShaderReflection::GetShaderResourceIndex(const s2string &name) const {
	for(uint32_t i=0; i<samplers.size(); i++) {
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

const TypeInfo & D3D11ShaderReflection::GetTypeInfo(const s2string &shader_typename) const {
	return typeinfo_manager.GetTypeInfo(shader_typename);
}

bool D3D11ShaderReflection::HasTypeInfo(const s2string &shader_typename) const {
	return typeinfo_manager.HasTypeInfo(shader_typename);
}

}









