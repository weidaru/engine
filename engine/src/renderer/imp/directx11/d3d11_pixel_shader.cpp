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
#include "d3d11_texture2d.h"
#include "d3d11_graphic_resource_manager.h"


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
			: manager(_manager), shader(0){
	cbs.resize(D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, 0);
}

D3D11PixelShader::~D3D11PixelShader() {
	Clear();
}

void D3D11PixelShader::Check() {
	CHECK(shader)<<"Pixel shader is not initialized.";
}

void D3D11PixelShader::Clear() {
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

	//Just compile from file for now.
	unsigned int flag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifndef NDEBUG
	flag |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob* shader_blob = 0;
    ID3DBlob* error_blob = 0;
	HRESULT result = 1;
	
	{
		FILE* file = fopen(path.c_str(), "r");
		fseek(file, 0, SEEK_END);
		long size = ftell(file);
		char *buffer = new char[size];
		fread(buffer, size, size, file);
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
	} else {
		manager->GetDevice()->CreatePixelShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), 0, &shader);
		//Setup reflection and constant buffer.
		
		if(shader_blob)
			shader_blob->Release();
		if(error_blob)
			error_blob->Release();
		return true;
	}
}

bool D3D11PixelShader::SetUniform(const s2string &name, const TypeInfo &type_info, const void *value) {
	Check();
	CHECK(false)<<"Disable for now";
	return false;
}

bool D3D11PixelShader::SetSampler(const s2string &name, Sampler *sampler) {
	Check();
	CHECK(false)<<"Disable for now";
	return false;
}

Sampler * D3D11PixelShader::GetSampler(const s2string &name) {
	Check();
	CHECK(false)<<"Disable for now";
	return 0;
}

bool D3D11PixelShader::SetResource(const s2string &name, Texture1D *resource) {
	Check();
	CHECK(false)<<"Disable for now";
	return false;
}

bool D3D11PixelShader::SetResource(const s2string &name, Texture2D *resource) {
	Check();
	CHECK(false)<<"Disable for now";
	return false;
}

bool D3D11PixelShader::SetResource(const s2string &name, Texture3D *resource) {
	Check();
	CHECK(false)<<"Disable for now";
	return false;
}

Resource * D3D11PixelShader::GetResource(const s2string &name) {
	Check();
	CHECK(false)<<"Disable for now";
	return 0;
}
	
void D3D11PixelShader::Flush() {
	if(shader) {
		HRESULT result = 1;
		ID3D11DeviceContext *context = manager->GetDeviceContext();
		context->PSSetShader(shader, 0, 0);
		
		//Set constant buffer.
		{
			int last_index = -1;
			for(int i=cbs.size()-1; i>=0; i--) {
				if(cbs[i]) {
					last_index = i;
					break;
				}
			}
			
			if(last_index != -1) {
				ID3D11Buffer **array = new ID3D11Buffer *[last_index+1];
				for(int i=0; i<=last_index; i++) {
					array[i] = cbs[i]->GetInternal();
				}
				context->PSSetConstantBuffers(0, last_index+1, array);
				delete[] array;
			}
		}
	}
}

}







