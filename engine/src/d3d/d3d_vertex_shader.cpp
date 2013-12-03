#include "d3d_vertex_shader.h"
#include "engine.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11async.h>
#include <d3d11shader.h>
#undef ERROR

#include <glog/logging.h>

namespace {
std::string ConvertErrorMessage(ID3D10Blob *msg) {
	std::string output;
	output.assign((char *)msg->GetBufferPointer(), msg->GetBufferSize());
	return output;
}
}

D3DVertexShader::D3DVertexShader() {
	vs_ = 0;
	reflection_ = 0;
	vs_buffer_ = 0;
}

D3DVertexShader::~D3DVertexShader() {

}

bool D3DVertexShader::LoadFromFile(ID3D11Device *device, const std::string &file_name, const std::string &entry_func_name) {
	ID3D10Blob *msg;
	HRESULT result = D3DX11CompileFromFile(file_name.c_str(), NULL, NULL, entry_func_name.c_str(), "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
				       &vs_buffer_, &msg, NULL);
	CHECK(!FAILED(result))<<"Cannot compile vertex shader from file "<<file_name
						<<" with entry function "<<entry_func_name<<". Error "<<ConvertErrorMessage(msg);
	
	result = device->CreateVertexShader(vs_buffer_->GetBufferPointer(), vs_buffer_->GetBufferSize(), NULL, &vs_);		
	CHECK(!FAILED(result))<<"Cannot create vertex shader. Error "<<GetLastError();
	
	//Shader reflection.
	D3DReflect( vs_buffer_->GetBufferPointer(), vs_buffer_->GetBufferSize(), 
            IID_ID3D11ShaderReflection, (void**) &reflection_);

	CreateConstantBufferReflection();
	CreateShaderResourceReflection();
	CreateSamplerReflection();
	
	msg->Release();
}

void D3DVertexShader::CreateConstantBufferReflection() {
	D3D11_SHADER_DESC desc;
	reflection_->GetDesc(&desc);

}

void D3DVertexShader::CreateShaderResourceReflection() {

}

void D3DVertexShader::CreateSamplerReflection() {

}


	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	