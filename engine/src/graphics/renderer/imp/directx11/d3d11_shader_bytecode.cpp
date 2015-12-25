#pragma comment(lib, "d3d11.lib")

#pragma comment(lib, "D3DCompiler.lib")

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <D3Dcompiler.h>
#undef ERROR

#include "d3d11_shader_bytecode.h"
#include "d3d11_graphic_resource_manager.h"
#include "d3d11_shader_reflection.h"

namespace s2 {

D3D11ShaderBytecode::D3D11ShaderBytecode(D3D11GraphicResourceManager *_manager)
	: manager(_manager), reflect(0), blob(0) {

}

D3D11ShaderBytecode::~D3D11ShaderBytecode() {
	delete reflect;
	if(blob) {
		blob->Release();
	}
}

bool D3D11ShaderBytecode::Initialize(const s2string &path, const s2string &entry_point) {
	uint32_t flag = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
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
		if(shader_blob) {
			shader_blob->Release();
		}
		if(error_blob) {
			error_blob->Release();
		}
		return false;
	}
	
	manager->GetDevice()->CreateVertexShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), 0, &shader);
	
	//Setup reflection
	reflect = new D3D11ShaderReflection(path, shader_blob);
	blob = shader_blob;
	if(error_blob) {
		error_blob->Release();
	}

	return true;
}


}


