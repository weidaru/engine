#pragma comment(lib, "d3d11.lib")

#pragma comment(lib, "D3DCompiler.lib")

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <D3Dcompiler.h>
#undef ERROR

#include "d3d11_shader_bytecode.h"

namespace s2 {

D3D11ShaderBytecode::D3D11ShaderBytecode(D3D11GraphicResourceManager *_manager)
	: manager(_manager), reflect(0), blob(0) {

}

D3D11ShaderBytecode::~D3D11ShaderBytecode() {

}

bool D3D11ShaderBytecode::Initialize(const s2string &path, const s2string &entry_point) {

}


}


