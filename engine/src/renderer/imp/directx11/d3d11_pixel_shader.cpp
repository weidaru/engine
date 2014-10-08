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
	rts.resize(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
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
bool D3D11PixelShader::Initialize(const s2string &path) {
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
		result = D3DCompile(buffer, size, path.c_str(), 0, 0, "main", "ps_5_0", flag, 0, &shader_blob, &error_blob);
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
		if(shader_blob)
			shader_blob->Release();
		if(error_blob)
			error_blob->Release();
		return true;
	}
}

bool D3D11PixelShader::SetConstantBuffer(const s2string &name, ConstantBuffer *cb) {
	Check();
	CHECK(false)<<"Disable for now";
	return false;
}

ConstantBuffer * D3D11PixelShader::GetConstantBuffer(const s2string &name) {
	Check();
	CHECK(false)<<"Disable for now";
	return 0;
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

bool D3D11PixelShader::SetRenderTarget(unsigned int index, Texture2D *target) {
	Check();
	rts[index].tex = NiceCast(D3D11Texture2D *, target);
	return true;
}

Resource * D3D11PixelShader::GetRenderTarget(unsigned int index) {
	Check();
	return rts[index].tex;
}

bool D3D11PixelShader::SetDepthStencilBuffer(Texture2D *buffer) {
	Check();
	ds.tex = NiceCast(D3D11Texture2D *, buffer);
	return true;
}

Resource* D3D11PixelShader::GetDepthStencilBuffer() {
	Check();
	return ds.tex;
}

bool D3D11PixelShader::SetRenderTargetClearOption(unsigned int index, bool enable, const float rgba[4]) {
	Check();
	rts[index].enable_clear = enable;
	rts[index].rgba[0] = rgba[0];
	rts[index].rgba[1] = rgba[1];
	rts[index].rgba[2] = rgba[2];
	rts[index].rgba[3] = rgba[3];
	return true;
}

bool D3D11PixelShader::GetRenderTargetClearOption(unsigned int index, bool *enable, float *rgba) {
	Check();
	*enable = rts[index].enable_clear;
	rgba[0] = rts[index].rgba[0];
	rgba[1] = rts[index].rgba[1];
	rgba[2] = rts[index].rgba[2];
	rgba[3] = rts[index].rgba[3];
	return true;
}

void D3D11PixelShader::SetDepthStencilBufferClearOption(bool enable,  float depth, uint8_t stencil) {
	Check();
	ds.enable_clear = enable;
	ds.depth = depth;
	ds.stencil = stencil;
}

void D3D11PixelShader::GetDepthStencilBufferClearOption(bool *enable,  float *depth, uint8_t *stencil) {
	Check();
	*enable = ds.enable_clear;
	*depth = ds.depth;
	*stencil = ds.stencil;
}
	
void D3D11PixelShader::GetLastError(s2string *str) {
	*str = error;
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
		
		//Set render target.
		{
			int last_index = -1;
			for(int i=rts.size()-1; i>=0; i--) {
				if(rts[i].tex) {
					last_index = i;
					break;
				}
			}
			if(last_index != -1) {
				ID3D11RenderTargetView **array = new ID3D11RenderTargetView *[last_index+1];
				for(int i=0; i<=last_index; i++) {
					array[i] = rts[i].tex->GetRenderTargetView();
				}
				context->OMSetRenderTargets(last_index+1, array, ds.tex?ds.tex->GetDepthStencilView():0);
				delete[] array;
			}
		}
	}
}

}







