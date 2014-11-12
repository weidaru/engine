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
#include "d3d11_shader_reflection.h"


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
		manager(_manager), shader(0), reflect(0), blob(0) {
}

/*TODO: Find a way to cache the program as it will need to be compiled every time a 
 *shader is initialized which is inefficient.
 */
bool D3D11VertexShader::Initialize(const s2string &path, const s2string &entry_point) {

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
	
	//Setup reflection and constant buffer.
	reflect = new D3D11ShaderReflection(path, shader_blob);
	CHECK(reflect->GetConstantBufferSize()<=D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT)
				<<"Constant buffer overflow. Max size is "<<D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT <<" get "<<reflect->GetConstantBufferSize();
	cbs.resize(reflect->GetConstantBufferSize());
	for(unsigned int i=0; i<cbs.size(); i++) {
		const D3D11ShaderReflection::ConstantBuffer &cb_reflect = reflect->GetConstantBuffer(i);
		cbs[i] = new D3D11ConstantBuffer(manager);
		cbs[i]->Initialize(cb_reflect.size, 0);
	}
	
	blob = shader_blob;
	if(error_blob)
		error_blob->Release();
	return true;
}

D3D11VertexShader::~D3D11VertexShader() {
	if(shader) {
		shader->Release();
		shader = 0;
	}
	if(blob) {
		blob->Release();
		blob = 0;
	}
	delete reflect;
	for(unsigned int i=0; i<cbs.size(); i++) {
		delete cbs[i];
	}
}

void D3D11VertexShader::Check() {
	CHECK(shader) << "VertexShader is not initialized.";
}

bool D3D11VertexShader::SetUniform(const s2string &name, const void * value, unsigned int size) {
	Check();
	if(!reflect->HasUniform(name)) {
		S2StringFormat(&error, "Cannot find uniform %s", name.c_str());
		return false;
	}
	const D3D11ShaderReflection::Uniform &uniform = reflect->GetUniform(name);
	D3D11ConstantBuffer &cb = *cbs[uniform.cb_index];
	CHECK(cb.SetData(uniform.offset, value ,size))<<cb.GetLastError();
	return true;
}

bool D3D11VertexShader::SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value) {
	Check();
	if(!reflect->HasUniform(name)) {
		S2StringFormat(&error, "Cannot find uniform %s", name.c_str());
		return false;
	}
	const D3D11ShaderReflection::Uniform &uniform = reflect->GetUniform(name);
	if(!reflect->CheckCompatible(uniform.type_name, cpp_type)) {
		S2StringFormat(&error, "shader type %s and cpp type %s are not compatible,", uniform.type_name.c_str(), cpp_type.GetName().c_str());
		return false;
	}
	D3D11ConstantBuffer &cb = *cbs[uniform.cb_index];
	CHECK(cb.SetData(uniform.offset, value, cpp_type.GetSize()))<<cb.GetLastError();
	return true;
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

void D3D11VertexShader::Setup() {
	if(shader) {
		ID3D11DeviceContext *context = manager->GetDeviceContext();
		context->VSSetShader(shader, 0, 0);
		
		//Set constant buffer.
		if(!cbs.empty()) {
			ID3D11Buffer **array = new ID3D11Buffer *[cbs.size()];
			for(unsigned int i=0; i<cbs.size(); i++) {
				cbs[i]->Flush();
				array[i] = cbs[i]->GetInternal();
			}
			context->VSSetConstantBuffers(0, cbs.size(), array);
			delete[] array;
		}

	}
}

}









