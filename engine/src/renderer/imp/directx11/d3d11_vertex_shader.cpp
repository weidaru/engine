#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#pragma comment(lib, "D3DCompiler.lib")

#include "d3d11_vertex_shader.h"

#define WIN32_LEAN_AND_MEAN
#include <dxgi.h>
#include <d3d11.h>
#include <D3Dcompiler.h>
#undef ERROR

#include <glog/logging.h>
#include <stdio.h>

#include "d3d11_constant_buffer.h"
#include "d3d11_graphic_resource_manager.h"
#include "d3d11_enum_converter.h"
#include "d3d11_shader_reflection.h"
#include "d3d11_shader_helper.h"


namespace s2 {

D3D11VertexShader::D3D11VertexShader(D3D11GraphicResourceManager *_manager) :
		manager(_manager), shader(0), reflect(0), blob(0),
		cb_container(0), sampler_container(0), sr_container(0) {
}

/*TODO: Find a way to cache the program as it will need to be compiled every time a 
 *shader is initialized which is inefficient.
 */
bool D3D11VertexShader::Initialize(const s2string &path, const s2string &entry_point) {
	CHECK(shader == 0) << "Cannot initialize twice.";

	//Just compile from file for now and always pack it as row major matrix.
	unsigned int flag = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifndef NDEBUG
	flag |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob* shader_blob = 0;
    ID3DBlob* error_blob = 0;
	HRESULT result = 1;
	
	{
		FILE* file = fopen(path.c_str(), "rb");
		if(!file) {
			S2StringFormat(&error, "Cannot open file %s", path.c_str());
			return false;
		}
		fseek(file, 0, SEEK_END);
		long size = ftell(file);
		fseek(file, 0, SEEK_SET);
		char *buffer = new char[size];
		fread(buffer, size, 1, file);
		result = D3DCompile(buffer, size, path.c_str(), 0, 0, entry_point.c_str(), "vs_5_0", flag, 0, &shader_blob, &error_blob);
		delete[] buffer;
	}

	if(FAILED(result)) {
		error  =(char *)(error_blob ? error_blob->GetBufferPointer() : "Fail to retrive error message.");
		if(shader_blob)
			shader_blob->Release();
		if(error_blob)
			error_blob->Release();
		return false;
	}
	
	manager->GetDevice()->CreateVertexShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), 0, &shader);
	
	//Setup reflection
	reflect = new D3D11ShaderReflection(path, shader_blob);
	blob = shader_blob;
	if(error_blob)
		error_blob->Release();
	
	//Setup constant buffer.
	cb_container = new ConstantBufferContainer(manager, reflect);
	
	//Setup samplers.
	sampler_container = new SamplerContainer(reflect);
	
	//Setup shader resources
	sr_container = new ShaderResourceContainer(reflect);
	
	return true;
}

D3D11VertexShader::~D3D11VertexShader() {
	delete sr_container;

	delete sampler_container;

	delete cb_container;

	delete reflect;

	if (blob) {
		blob->Release();
	}
	if (shader) {
		shader->Release();
	}
}

void D3D11VertexShader::Check() {
	CHECK(shader) << "VertexShader is not initialized.";
}

bool D3D11VertexShader::SetUniform(const s2string &name, const void * value, unsigned int size) {
	Check();
	return cb_container->SetUniform(name, value, size, &error);
}

bool D3D11VertexShader::SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value) {
	Check();
	return cb_container->SetUniform(name, cpp_type, value, &error);
}

bool D3D11VertexShader::SetSampler(const s2string &name, Sampler *sampler) {
	Check();
	return sampler_container->SetSampler(name, sampler, &error);
}

D3D11Sampler * D3D11VertexShader::GetSampler(const s2string &name) {
	Check();
	return sampler_container->GetSampler(name, &error);
}

bool D3D11VertexShader::SetShaderResource(const s2string &name, ShaderResource *shader_resource) {
	Check();
	return sr_container->SetShaderResource(name, shader_resource, &error);
}

D3D11ShaderResource * D3D11VertexShader::GetShaderResource(const s2string &name) {
	return sr_container->GetShaderResource(name, &error);
}

void D3D11VertexShader::Setup() {
	if(shader) {
		ID3D11DeviceContext *context = manager->GetDeviceContext();
		D3D11ShaderHelper::ShaderType type = D3D11ShaderHelper::VERTEX;
		context->VSSetShader(shader, 0, 0);
		
		//Set constant buffer.
		cb_container->Setup(context, type);
		
		//Set samplers.
		sampler_container->Setup(context, type);
		
		//Set shader resources.
		sr_container->Setup(context, type);
	}
}

}









