#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include "d3d11_pixel_shader.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <D3Dcompiler.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_constant_buffer.h"
#include "d3d11_graphic_resource_manager.h"
#include "d3d11_shader_reflection.h"
#include "d3d11_shader_helper.h"


#ifdef NDEBUG
	#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
	#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif

/**
 * Right now the binding of resources will be hard coded thus static. 
 * TODO: Add reflection to support dynamic binding. 
 */
namespace s2 {

D3D11PixelShader::D3D11PixelShader(D3D11GraphicResourceManager *_manager)
			: 	manager(_manager), shader(0), reflect(0), blob(0), 
				cb_container(0), sampler_container(0), sr_container(0) {

}

D3D11PixelShader::~D3D11PixelShader() {
	Clear();
}

void D3D11PixelShader::Check() {
	CHECK(shader)<<"Pixel shader is not initialized.";
}

void D3D11PixelShader::Clear() {
	delete sr_container;
	sr_container = 0;

	delete sampler_container;
	sampler_container = 0;
	
	delete cb_container;
	cb_container = 0;
	
	delete reflect;
	reflect = 0;
	
	if(blob) {
		blob->Release();
		blob =0;
	}
	
	if(shader) {
		shader->Release();
		shader = 0;
	}
}

/**
 * 
 */
bool D3D11PixelShader::Initialize(const s2string &path, const s2string &entry_point) {
	Clear();

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
		result = D3DCompile(buffer, size, path.c_str(), 0, 0, entry_point.c_str(), "ps_5_0", flag, 0, &shader_blob, &error_blob);
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
	manager->GetDevice()->CreatePixelShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), 0, &shader);
	//Setup reflection and constant buffer.
	reflect = new D3D11ShaderReflection(path, shader_blob);
	blob = shader_blob;
	if(error_blob)
		error_blob->Release();
	
	//Setup constant buffers
	cb_container = new ConstantBufferContainer(manager, reflect);
	
	//Setup samplers.
	sampler_container = new SamplerContainer(reflect);
	
	//Setup shader resources.
	sr_container = new ShaderResourceContainer(reflect);
	
	return true;
}

bool D3D11PixelShader::SetUniform(const s2string &name, const void * value, unsigned int size) {
	Check();
	return cb_container->SetUniform(name, value, size, &error);
}

bool D3D11PixelShader::SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value) {
	Check();
	return cb_container->SetUniform(name, cpp_type, value,  &error);
}

bool D3D11PixelShader::SetSampler(const s2string &name, Sampler *sampler) {
	Check();
	return sampler_container->SetSampler(name, sampler, &error);
}

Sampler * D3D11PixelShader::GetSampler(const s2string &name) {
	Check();
	return sampler_container->GetSampler(name, &error);
}


bool D3D11PixelShader::SetTexture2D(const s2string &name, Texture2D *texture) {
	Check();
	return sr_container->SetTexture2D(name, texture, &error);
}

Texture2D * D3D11PixelShader::GetTexture2D(const s2string &name) {
	Check();
	return sr_container->GetTexture2D(name, &error);
}
	
void D3D11PixelShader::Setup() {
	if(shader) {
		ID3D11DeviceContext *context = manager->GetDeviceContext();
		D3D11ShaderHelper::ShaderType type = D3D11ShaderHelper::PIXEL;
		
		context->PSSetShader(shader, 0, 0);
		
		//Set constant buffer.
		cb_container->Setup(context, type);
		
		//Set sampler
		sampler_container->Setup(context, type);
		
		//Set shader resources.
		sr_container->Setup(context, type);
	}
}

}







