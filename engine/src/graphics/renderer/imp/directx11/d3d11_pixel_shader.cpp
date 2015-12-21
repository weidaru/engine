#pragma comment(lib, "d3d11.lib")

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_pixel_shader.h"

#include "d3d11_graphic_resource_manager.h"

namespace s2 {

D3D11PixelShader::D3D11PixelShader(D3D11GraphicResourceManager *_manager)
	: manager(_manager), bytecode(0), delete_bytecode(false), shader(0){

}

D3D11PixelShader::~D3D11PixelShader() {
	if(shader) {
		shader->Release();
	}
	
	if(delete_bytecode) {
		delete bytecode;
	}
}

bool D3D11PixelShader::Initialize(ShaderBytecode *_bytecode) {
	CHECK(bytecode==0)<<"Cannot initialize twice";

	bytecode = static_cast<D3D11ShaderBytecode *>(_bytecode);

	ID3DBlob *blob = bytecode->GetBlob();
	HRESULT result = 1;
	result = manager->GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), 0, &shader);
	if(FAILED(result)) {
		S2StringFormat(&error, "Cannot create pixel shader. Error code %d", ::GetLastError());
		return false;
	} else {
		return true;
	}
}

bool D3D11PixelShader::Initialize(const s2string &path, const s2string &entry_point) {
	CHECK(bytecode==0)<<"Cannot initialize twice";

	D3D11ShaderBytecode *b = new D3D11ShaderBytecode(manager);
	if( b->Initialize(path, entry_point, ShaderType::PIXEL) == false ) {
		error = b->GetLastError();
		delete b;
		return false;
	} else {
		delete_bytecode = true;
		return Initialize(b);
	}
}

}