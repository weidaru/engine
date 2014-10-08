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
		manager(_manager), ib(0), shader(0), topology(TRIANGLE_LIST){
	vbs.resize(D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
	cbs.resize(D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, 0);
}

/*TODO: Find a way to cache the program as it will need to be compiled every time a 
 *shader is initialized which is inefficient.
 */
bool D3D11VertexShader::Initialize(const s2string &path, PrimitiveTopology _topology) {
	topology = _topology;

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

bool D3D11VertexShader::SetPrimitiveTopology(PrimitiveTopology newvalue) {
	topology = newvalue;
	return true;
}

VertexShader::PrimitiveTopology D3D11VertexShader::GetPrimitiveTopology() {
	return topology;
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

bool D3D11VertexShader::SetVertexBuffer(unsigned int index, VertexBuffer *_buf, VertexBufferUsage usage, const s2string &type_name) {
	Check();
	D3D11VertexBuffer *buf = NiceCast(D3D11VertexBuffer *, _buf);
	//Skip typecheck for now.
	int i = 0;
	vbs[i].vb = buf;
	vbs[i].usage = usage;
	vbs[i].type_name = type_name;
	return true;
}

VertexBuffer * D3D11VertexShader::GetVertexBuffer(unsigned int index, VertexBufferUsage *usage, s2string *type_name) {
	Check();
	CHECK(false)<<"Disable for now";
	return 0;
}

bool D3D11VertexShader::SetIndexBuffer(IndexBuffer *_buf) {
	Check();
	D3D11IndexBuffer *buf = NiceCast(D3D11IndexBuffer *, _buf);
	ib = buf;
	return true;
}

IndexBuffer * D3D11VertexShader::GetIndexBuffer() {
	return ib;
}

void D3D11VertexShader::Flush() {
	
	if(shader) {
		HRESULT result = 1;
		ID3D11DeviceContext *context = manager->GetDeviceContext();
		context->VSSetShader(shader, 0, 0);
		
		//Set primitive topology
		context->IASetPrimitiveTopology(D3D11EnumConverter::TopologyToD3D11Topology(topology));
		
		//Set vertex buffer.
		{
			int last_index = -1;
			for(int i=vbs.size()-1; i>=0; i--) {
				if(vbs[i].vb) {
					last_index = i;
					break;
				}
			}
			if(last_index != -1) {
				ID3D11Buffer **buf_array = new ID3D11Buffer *[last_index+1];
				unsigned int *stride_array = new unsigned int[last_index+1];
				unsigned int *offset_array = new unsigned int[last_index+1];
				for(int i=0; i<=last_index; i++) {
					buf_array[i] = vbs[i].vb->GetInternal();
					stride_array[i] = 4;			//This information needs to retrive from type_name, use 4 for now.
					offset_array[i] = 0;
				}
				context->IASetVertexBuffers(0, last_index+1, buf_array, stride_array, offset_array);
				delete[] stride_array;
				delete[] offset_array;
			}
		}
		
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
		
		
		//Set index buffer
		if(ib) {
			context->IASetIndexBuffer(ib->GetInternal(), DXGI_FORMAT_R32_UINT, 0);
		}
	}
}

void D3D11VertexShader::GetLastError(s2string *str) {
	*str = error;
}

}









