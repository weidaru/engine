#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#pragma comment(lib, "D3DCompiler.lib")

#define WIN32_LEAN_AND_MEAN
#include <dxgi.h>
#include <d3d11.h>
#include <D3Dcompiler.h>
#undef ERROR

#include <glog/logging.h>


#include "d3d11_shader_data.h"

#include "d3d11_shader_reflection.h"
#include "d3d11_graphic_resource_manager.h"
#include "d3d11_graphic_pipeline.h"
#include "d3d11_constant_buffer.h"
#include "d3d11_resource_view.h"
#include "d3d11_sampler.h"
#include "d3d11_shader_bytecode.h"

#include "utils/type_info.h"

#ifdef NDEBUG
	#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
	#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif

namespace s2 {

ConstantBufferContainer::ConstantBufferContainer(D3D11GraphicResourceManager *manager, D3D11ShaderReflection *_reflect) 
			: reflect(_reflect){
	CHECK_NOTNULL(reflect);
	cbs.resize(reflect->GetConstantBufferSize());
	for(uint32_t i=0; i<cbs.size(); i++) {
		const D3D11ShaderReflection::ConstantBuffer &cb_reflect = reflect->GetConstantBuffer(i);
		cbs[i].first = cb_reflect.slot_index;
		cbs[i].second = new D3D11ConstantBuffer(manager);
		cbs[i].second->Initialize(cb_reflect.size, 0);
	}
}

ConstantBufferContainer::~ConstantBufferContainer() {
	for(uint32_t i=0; i<cbs.size(); i++) {
		delete cbs[i].second;
	}
}

bool ConstantBufferContainer::SetUniform(const s2string &name, const void * value, uint32_t size, s2string *error) {
	if(!reflect->HasUniform(name)) {
		S2StringFormat(error, "Cannot find uniform %s", name.c_str());
		return false;
	}
	const D3D11ShaderReflection::Uniform &uniform = reflect->GetUniform(name);
	D3D11ConstantBuffer *cb = 0;
	for(uint32_t i=0; i<cbs.size(); i++) {
		if(cbs[i].first == uniform.cb_slot_index) {
			cb = cbs[i].second;
		}
	}
	CHECK(cb);
	CHECK(cb->SetData(uniform.offset, value ,size))<<cb->GetLastError();
	return true;
}

bool ConstantBufferContainer::SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value, s2string *error) {
	if(!reflect->HasUniform(name)) {
		S2StringFormat(error, "Cannot find uniform %s", name.c_str());
		return false;
	}
	const D3D11ShaderReflection::Uniform &uniform = reflect->GetUniform(name);
	D3D11ConstantBuffer *cb = 0;
	for(uint32_t i=0; i<cbs.size(); i++) {
		if(cbs[i].first == uniform.cb_slot_index) {
			cb = cbs[i].second;
		}
	}
	CHECK(cb);
	CHECK(cb->SetData(uniform.offset, value, cpp_type.GetSize()))<<cb->GetLastError();
	return true;
}

void ConstantBufferContainer::Setup(D3D11GraphicPipeline *pipeline, ShaderType shader_type) {
	ID3D11Buffer *buffer_array[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT]={0};
	
	for(uint32_t i=0; i<cbs.size(); i++) {
		D3D11ConstantBuffer *cb = cbs[i].second;
		if(cb == 0) {
			continue;
		}
		cb->Flush(pipeline);
		buffer_array[cbs[i].first] =  cb->GetInternal();
	}

	ID3D11DeviceContext *context = pipeline->GetDeviceContext();
	switch(shader_type) {
		case ShaderType::VERTEX:
			context->VSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, buffer_array);
			break;
		case ShaderType::PIXEL:
			context->PSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, buffer_array);
			break;
		case ShaderType::GEOMETRY:
			context->GSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, buffer_array);
		default:
			CHECK(false)<<"Unknown shader_type "<<static_cast<int>(shader_type);
			break;
	}
}

void ConstantBufferContainer::UnBind(D3D11GraphicPipeline *pipeline, ShaderType shader_type) {
	ID3D11Buffer *buffer_array[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT]={0};
	ID3D11DeviceContext *context = pipeline->GetDeviceContext();
	switch(shader_type) {
		case ShaderType::VERTEX:
			context->VSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, buffer_array);
			break;
		case ShaderType::PIXEL:
			context->PSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, buffer_array);
			break;
		case ShaderType::GEOMETRY:
			context->GSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, buffer_array);
		default:
			CHECK(false)<<"Unknown shader_type "<<static_cast<int>(shader_type);
			break;
	}
}

SamplerContainer::SamplerContainer(D3D11ShaderReflection *_reflect) 
		: reflect(_reflect){
	CHECK_NOTNULL(reflect);
	samplers.resize(reflect->GetSamplerSize());
	for(uint32_t i=0; i<samplers.size(); i++) {
		const D3D11ShaderReflection::Sampler &info =  reflect->GetSampler(i);
		samplers[i].first = i;
		samplers[i].second = 0;
	}
}
	
bool SamplerContainer::SetSampler(const s2string &name, Sampler *_sampler, s2string *error) {
	D3D11Sampler *sampler = 0;
	if(_sampler != 0) {
		sampler = NiceCast(D3D11Sampler *, _sampler);
		CHECK(sampler) << "Cannot cast sampler to D3D11Sampler.";
	}
	if(!reflect->HasSampler(name)) {
		S2StringFormat(error, "Cannot find sampler %s", name.c_str());
		return false;
	}
	
	uint32_t reflect_index = reflect->GetSamplerIndex(name);
	const D3D11ShaderReflection::Sampler &info = reflect->GetSampler(reflect_index);
	
	for(uint32_t i=0; i<samplers.size(); i++) {
		if(samplers[i].first == reflect_index) {
			samplers[i].second = sampler;
			return true;
		}
	}
	CHECK(false);
	return false;
}

D3D11Sampler* SamplerContainer::GetSampler(const s2string &name, s2string *error) {
	if(!reflect->HasSampler(name)) {
		S2StringFormat(error, "Cannot find sampler %s", name.c_str());
		return 0;
	}
	
	uint32_t reflect_index = reflect->GetSamplerIndex(name);
	const D3D11ShaderReflection::Sampler &info = reflect->GetSampler(reflect_index);
	
	for(uint32_t i=0; i<samplers.size(); i++) {
		if(samplers[i].first == reflect_index) {
			return samplers[i].second;
		}
	}
	CHECK(false);
	return 0;
}
	
void SamplerContainer::Setup(D3D11GraphicPipeline *pipeline, ShaderType shader_type)  {
	ID3D11SamplerState * sampler_array[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = {0};
	

	for(uint32_t i=0; i<samplers.size(); i++) {
		D3D11Sampler *sampler = samplers[i].second;
		if(sampler == 0) {
			continue;
		}
		
		sampler_array[samplers[i].first] =  sampler->GetInternal();
	}

	ID3D11DeviceContext *context = pipeline->GetDeviceContext();
	switch(shader_type) {
		case ShaderType::VERTEX:
			context->VSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, sampler_array);
			break;
		case ShaderType::PIXEL:
			context->PSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, sampler_array);
			break;
		case ShaderType::GEOMETRY:
			context->GSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, sampler_array);
			break;
		default:
			CHECK(false)<<"Unknown shader_type "<<static_cast<int>(shader_type);
			break;
	}
}

void SamplerContainer::UnBind(D3D11GraphicPipeline *pipeline, ShaderType shader_type) {
	ID3D11SamplerState * sampler_array[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = {0};
	ID3D11DeviceContext *context = pipeline->GetDeviceContext();
	switch(shader_type) {
		case ShaderType::VERTEX:
			context->VSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, sampler_array);
			break;
		case ShaderType::PIXEL:
			context->PSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, sampler_array);
			break;
		case ShaderType::GEOMETRY:
			context->GSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, sampler_array);
			break;
		default:
			CHECK(false)<<"Unknown shader_type "<<static_cast<int>(shader_type);
			break;
	}
}

ShaderResourceContainer::ShaderResourceContainer(D3D11ShaderReflection *_reflect)
		: reflect(_reflect){
	CHECK_NOTNULL(reflect);
	shader_resources.resize(reflect->GetShaderResourceSize());
	for(uint32_t i=0; i<shader_resources.size(); i++) {
		const D3D11ShaderReflection::ShaderResource &info = reflect->GetShaderResource(i);
		shader_resources[i].reflect_index = i;
		shader_resources[i].shader_resource = 0;
	}
}

bool ShaderResourceContainer::SetShaderResource(const s2string &name, ShaderResource *_shader_resource, s2string *error) {
	D3D11ShaderResource *shader_resource = 0;
	if (_shader_resource != 0) {
		shader_resource = NiceCast(D3D11ShaderResource*, _shader_resource);
		CHECK(shader_resource) << "Cannot cast shader resource to D3D11ShaderResource";
	}
	
	if(!reflect->HasShaderResource(name)) {
		S2StringFormat(error, "Cannot find shader resource %s", name);
		return false;
	}
	uint32_t reflect_index = reflect->GetShaderResourceIndex(name);
	const D3D11ShaderReflection::ShaderResource &info = reflect->GetShaderResource(reflect_index);
	if(info.type != D3D11ShaderReflection::TEXTURE) {
		S2StringFormat(error, "Shader resource %s is not declared as a texture", name);
		return false;
	}
	for(uint32_t i=0; i<shader_resources.size(); i++) {
		if(shader_resources[i].reflect_index == reflect_index) {
			shader_resources[i].shader_resource = shader_resource;
			return true;
		}
	}
	CHECK(false);
	return false;
}

D3D11ShaderResource * ShaderResourceContainer::GetShaderResource(const s2string &name, s2string *error) {
	if(!reflect->HasShaderResource(name)) {
		S2StringFormat(error, "Cannot find shader resource %s", name);
		return 0;
	}
	uint32_t reflect_index = reflect->GetShaderResourceIndex(name);
	const D3D11ShaderReflection::ShaderResource &info = reflect->GetShaderResource(reflect_index);
	if(info.type != D3D11ShaderReflection::TEXTURE) {
		S2StringFormat(error, "Shader resource %s is not declared as a texture", name);
		return 0;
	}
	for(uint32_t i=0; i<shader_resources.size(); i++) {
		if(shader_resources[i].reflect_index == reflect_index) {
			return shader_resources[i].shader_resource;
		}
	}
	CHECK(false);
	return 0;
}

void ShaderResourceContainer::Setup(D3D11GraphicPipeline *pipeline, ShaderType shader_type) {
	ID3D11ShaderResourceView *sr_view_array[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT ] = {};
	

	for(uint32_t i=0; i<shader_resources.size(); i++) {
		const D3D11ShaderReflection::ShaderResource &info = reflect->GetShaderResource(shader_resources[i].reflect_index);
		if(shader_resources[i].shader_resource == 0) {
			continue;
		}

		sr_view_array[info.slot_index] = shader_resources[i].shader_resource->GetShaderResourceView();	
	}

	ID3D11DeviceContext *context = pipeline->GetDeviceContext();
	switch(shader_type) {
		case ShaderType::VERTEX:
			context->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, sr_view_array );
			break;
		case ShaderType::PIXEL:
			context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, sr_view_array );
			break;
		case ShaderType::GEOMETRY:
			context->GSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, sr_view_array );
			break;
		default:
			CHECK(false)<<"Unknown shader_type "<<static_cast<int>(shader_type);
			break;
	}
}

void ShaderResourceContainer::UnBind(D3D11GraphicPipeline *pipeline, ShaderType shader_type) {
	ID3D11ShaderResourceView *sr_view_array[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT ] = {};
	
	ID3D11DeviceContext *context = pipeline->GetDeviceContext();
	switch(shader_type) {
		case ShaderType::VERTEX:
			context->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, sr_view_array );
			break;
		case ShaderType::PIXEL:
			context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, sr_view_array );
			break;
		case ShaderType::GEOMETRY:
			context->GSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, sr_view_array );
			break;
		default:
			CHECK(false)<<"Unknown shader_type "<<static_cast<int>(shader_type);
			break;
	}
}


D3D11ShaderData::D3D11ShaderData(D3D11GraphicResourceManager *_manager) 
	: manager(_manager), cb_container(0), sampler_container(0), sr_container(0){
	CHECK_NOTNULL(manager);
}

D3D11ShaderData::~D3D11ShaderData() {

}


bool D3D11ShaderData::Initialize(ShaderBytecode *_bytecode) {
	if(bytecode != 0) {
		error = "Cannot initialize twice.";
	}
	if(_bytecode == 0) {
		error = "Input is NULL";
	}

	cb_container = new ConstantBufferContainer(manager, bytecode->GetReflection());
	sampler_container = new SamplerContainer(bytecode->GetReflection());
	sr_container = new ShaderResourceContainer(bytecode->GetReflection());
}


bool D3D11ShaderData::SetUniform(const s2string &name, const void * value, uint32_t size) {
	return cb_container->SetUniform(name, value, size, &error);
}
	
bool D3D11ShaderData::SetSampler(const s2string &name, Sampler *sampler) {
	return sampler_container->SetSampler(name, sampler, &error);
}

Sampler * D3D11ShaderData::GetSampler(const s2string &name) {
	return sampler_container->GetSampler(name, &error);
}

bool D3D11ShaderData::SetShaderResource(const s2string &name, ShaderResource *shader_resource) {
	return sr_container->SetShaderResource(name, shader_resource, &error);
}

ShaderResource * D3D11ShaderData::GetShaderResource(const s2string &name) {
	return sr_container->GetShaderResource(name, &error);
}

void D3D11ShaderData::Setup(D3D11GraphicPipeline *pipeline, ShaderType type) {
	cb_container->Setup(pipeline, type);
	sampler_container->Setup(pipeline, type);
	sr_container->Setup(pipeline, type);
}

void D3D11ShaderData::UnBind(D3D11GraphicPipeline *pipeline, ShaderType type) {
	ConstantBufferContainer::UnBind(pipeline, type);
	SamplerContainer::UnBind(pipeline, type);
	ShaderResourceContainer::UnBind(pipeline, type);
}

}














