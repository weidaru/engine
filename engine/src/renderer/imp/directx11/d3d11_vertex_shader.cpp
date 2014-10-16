#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
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
#include "d3d11_vertex_buffer.h"
#include "d3d11_index_buffer.h"
#include "d3d11_graphic_resource_manager.h"
#include "d3d11_enum_converter.h"


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

D3D11VertexShader::D3D11VertexShader(D3D11GraphicResourceManager *_manager) :
		manager(_manager), shader(0){
	cbs.resize(D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, 0);
}

/*TODO: Find a way to cache the program as it will need to be compiled every time a 
 *shader is initialized which is inefficient.
 */
bool D3D11VertexShader::Initialize(const s2string &path) {

	//Just compile from file for now.
	unsigned int flag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifndef NDEBUG
	flag |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob* shader_blob = 0;
    ID3DBlob* error_blob = 0;
	HRESULT result = 1;
	
	{
		FILE* file = fopen(path.c_str(), "rb");
		if(!file) {
			char buffer[1024*5];
			sprintf(buffer, "Cannot open file %s", path.c_str());
			error = buffer;
			return false;
		}
		fseek(file, 0, SEEK_END);
		long size = ftell(file);
		fseek(file, 0, SEEK_SET);
		char *buffer = new char[size];
		fread(buffer, size, size, file);
		result = D3DCompile(buffer, size, path.c_str(), 0, 0, "main", "vs_5_0", flag, 0, &shader_blob, &error_blob);
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
		manager->GetDevice()->CreateVertexShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), 0, &shader);
		if(shader_blob)
			shader_blob->Release();
		if(error_blob)
			error_blob->Release();
		return true;
	}
}

D3D11VertexShader::~D3D11VertexShader() {
	if(shader) {
		shader->Release();
		shader = 0;
	}
}

void D3D11VertexShader::Check() {
	CHECK(shader) << "VertexShader is not initialized.";
}

bool D3D11VertexShader::SetConstantBuffer(const s2string &name, ConstantBuffer *_cb) {
	Check();
	D3D11ConstantBuffer *cb = NiceCast(D3D11ConstantBuffer *, _cb);
	cbs[0] = cb;
	return true;
}

ConstantBuffer * D3D11VertexShader::GetConstantBuffer(const s2string &name) {
	Check();
	CHECK(false)<<"Disable for now";
	return 0;
}

bool D3D11VertexShader::SetSampler(const s2string &name, Sampler *sampler) {
	Check();
	CHECK(false)<<"Disable for now";
	return false;
}

Sampler * D3D11VertexShader::GetSampler(const s2string &name) {
	Check();
	CHECK(false)<<"Disable for now";
	return 0;
}

bool D3D11VertexShader::SetResource(const s2string &name, Texture1D *resource) {
	Check();
	CHECK(false)<<"Disable for now";
	return false;
}

bool D3D11VertexShader::SetResource(const s2string &name, Texture2D *resource) {
	Check();
	CHECK(false)<<"Disable for now";
	return false;
}

bool D3D11VertexShader::SetResource(const s2string &name, Texture3D *resource) {
	Check();
	CHECK(false)<<"Disable for now";
	return false;
}

Resource * D3D11VertexShader::GetResource(const s2string &name) {
	Check();
	CHECK(false)<<"Disable for now";
	return 0;
}

void D3D11VertexShader::Flush() {
	
	if(shader) {
		HRESULT result = 1;
		ID3D11DeviceContext *context = manager->GetDeviceContext();
		context->VSSetShader(shader, 0, 0);
		
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

void D3D11VertexShader::GetLastError(s2string *str) {
	*str = error;
}

}









