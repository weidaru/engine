#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#pragma comment(lib, "D3DCompiler.lib")

#define WIN32_LEAN_AND_MEAN
#include <dxgi.h>
#include <d3d11.h>
#include <D3Dcompiler.h>
#undef ERROR

#include <glog/logging.h>


#include "d3d11_shader_helper.h"

#include "d3d11_shader_reflection.h"
#include "d3d11_graphic_resource_manager.h"
#include "d3d11_constant_buffer.h"
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
	for(unsigned int i=0; i<cbs.size(); i++) {
		const D3D11ShaderReflection::ConstantBuffer &cb_reflect = reflect->GetConstantBuffer(i);
		cbs[i].first = cb_reflect.slot_index;
		cbs[i].second = new D3D11ConstantBuffer(manager);
		cbs[i].second->Initialize(cb_reflect.size, 0);
	}
}

ConstantBufferContainer::~ConstantBufferContainer() {
	for(unsigned int i=0; i<cbs.size(); i++) {
		delete cbs[i].second;
	}
}

bool ConstantBufferContainer::SetUniform(const s2string &name, const void * value, unsigned int size, s2string *error) {
	if(!reflect->HasUniform(name)) {
		S2StringFormat(error, "Cannot find uniform %s", name.c_str());
		return false;
	}
	const D3D11ShaderReflection::Uniform &uniform = reflect->GetUniform(name);
	D3D11ConstantBuffer *cb = 0;
	for(unsigned int i=0; i<cbs.size(); i++) {
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
	if(!reflect->CheckCompatible(uniform.type_name, cpp_type)) {
		S2StringFormat(error, "shader type %s and cpp type %s are not compatible,", uniform.type_name.c_str(), cpp_type.GetName().c_str());
		return false;
	}
	D3D11ConstantBuffer *cb = 0;
	for(unsigned int i=0; i<cbs.size(); i++) {
		if(cbs[i].first == uniform.cb_slot_index) {
			cb = cbs[i].second;
		}
	}
	CHECK(cb);
	CHECK(cb->SetData(uniform.offset, value, cpp_type.GetSize()))<<cb->GetLastError();
	return true;
}

void ConstantBufferContainer::Setup(ID3D11DeviceContext *context, D3D11ShaderHelper::ShaderType shader_type) {
	for(unsigned int i=0; i<cbs.size(); i++) {
		D3D11ConstantBuffer *cb = cbs[i].second;
		if(cb == 0) {
			continue;
		}
		cb->Flush();
		ID3D11Buffer *buffer = cb->GetInternal();
		switch(shader_type) {
			case D3D11ShaderHelper::VERTEX:
				context->VSSetConstantBuffers(cbs[i].first, 1, &buffer);
				break;
			case D3D11ShaderHelper::PIXEL:
				context->PSSetConstantBuffers(cbs[i].first, 1, &buffer);
				break;
			case D3D11ShaderHelper::GEOMETRY:
				context->GSSetConstantBuffers(cbs[i].first, 1, &buffer);
			default:
				CHECK(false)<<"Unknown shader_type "<<shader_type;
				break;
		}
	}
}


SamplerContainer::SamplerContainer(D3D11ShaderReflection *_reflect) 
		: reflect(_reflect){
	CHECK_NOTNULL(reflect);
	samplers.resize(reflect->GetSamplerSize());
	for(unsigned int i=0; i<samplers.size(); i++) {
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
	
	unsigned int reflect_index = reflect->GetSamplerIndex(name);
	const D3D11ShaderReflection::Sampler &info = reflect->GetSampler(reflect_index);
	
	for(unsigned int i=0; i<samplers.size(); i++) {
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
	
	unsigned int reflect_index = reflect->GetSamplerIndex(name);
	const D3D11ShaderReflection::Sampler &info = reflect->GetSampler(reflect_index);
	
	for(unsigned int i=0; i<samplers.size(); i++) {
		if(samplers[i].first == reflect_index) {
			return samplers[i].second;
		}
	}
	CHECK(false);
	return 0;
}
	
void SamplerContainer::Setup(ID3D11DeviceContext *context, D3D11ShaderHelper::ShaderType shader_type) {
	for(unsigned int i=0; i<samplers.size(); i++) {
		D3D11Sampler *sampler = samplers[i].second;
		if(sampler == 0) {
			continue;
		}
		ID3D11SamplerState *state = sampler->GetInternal();
		switch(shader_type) {
			case D3D11ShaderHelper::VERTEX:
				context->VSSetSamplers(samplers[i].first, 1, &state);
				break;
			case D3D11ShaderHelper::PIXEL:
				context->PSSetSamplers(samplers[i].first, 1, &state);
				break;
			case D3D11ShaderHelper::GEOMETRY:
				context->GSSetSamplers(samplers[i].first, 1, &state);
				break;
			default:
				CHECK(false)<<"Unknown shader_type "<<shader_type;
				break;
		}
	}
}

ShaderResourceContainer::ShaderResourceContainer(D3D11ShaderReflection *_reflect)
		: reflect(_reflect){
	CHECK_NOTNULL(reflect);
	shader_resources.resize(reflect->GetShaderResourceSize());
	for(unsigned int i=0; i<shader_resources.size(); i++) {
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
	unsigned int reflect_index = reflect->GetShaderResourceIndex(name);
	const D3D11ShaderReflection::ShaderResource &info = reflect->GetShaderResource(reflect_index);
	if(info.type != D3D11ShaderReflection::TEXTURE) {
		S2StringFormat(error, "Shader resource %s is not declared as a texture", name);
		return false;
	}
	for(unsigned int i=0; i<shader_resources.size(); i++) {
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
	unsigned int reflect_index = reflect->GetShaderResourceIndex(name);
	const D3D11ShaderReflection::ShaderResource &info = reflect->GetShaderResource(reflect_index);
	if(info.type != D3D11ShaderReflection::TEXTURE) {
		S2StringFormat(error, "Shader resource %s is not declared as a texture", name);
		return 0;
	}
	for(unsigned int i=0; i<shader_resources.size(); i++) {
		if(shader_resources[i].reflect_index == reflect_index) {
			return shader_resources[i].shader_resource;
		}
	}
	CHECK(false);
	return 0;
}

ShaderResourceContainer::BindingVector ShaderResourceContainer::GetNewBindings() const {
	BindingVector result;
	for(unsigned int i=0; i<shader_resources.size(); i++) {
		const D3D11ShaderReflection::ShaderResource &info = reflect->GetShaderResource(shader_resources[i].reflect_index);
		Resource *resource = 0;
		if (shader_resources[i].shader_resource) {
			resource = shader_resources[i].shader_resource->GetResource();
		}
		result.push_back(std::make_pair(info.slot_index, resource));
	}
	return result;
}

void ShaderResourceContainer::Setup(ID3D11DeviceContext *context, D3D11ShaderHelper::ShaderType shader_type) {
	for(unsigned int i=0; i<shader_resources.size(); i++) {
		const D3D11ShaderReflection::ShaderResource &info = reflect->GetShaderResource(shader_resources[i].reflect_index);
		ID3D11ShaderResourceView *view = 0;
		if (shader_resources[i].shader_resource) {
			view = shader_resources[i].shader_resource->GetShaderResourceView();
		}

		switch(shader_type) {
			case D3D11ShaderHelper::VERTEX:
				context->VSSetShaderResources(info.slot_index, 1, &view);
				break;
			case D3D11ShaderHelper::PIXEL:
				context->PSSetShaderResources(info.slot_index, 1, &view);
				break;
			case D3D11ShaderHelper::GEOMETRY:
				context->GSSetShaderResources(info.slot_index, 1, &view);
				break;
			default:
				CHECK(false)<<"Unknown shader_type "<<shader_type;
				break;
		}
	}
}

}














