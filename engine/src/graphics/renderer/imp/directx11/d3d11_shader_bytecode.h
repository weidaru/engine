#ifndef D3D11_SHADER_BYTECODE_H_
#define D3D11_SHADER_BYTECODE_H_

#include "graphics/renderer/shader_bytecode.h"

struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;

namespace s2 {

class D3D11GraphicResourceManager;
class D3D11ShaderReflection;

class D3D11ShaderBytecode : public ShaderBytecode {
public:
	D3D11ShaderBytecode(D3D11GraphicResourceManager *_manager);

	virtual ~D3D11ShaderBytecode();

	virtual bool Initialize(const s2string &path, const s2string &entry_point, ShaderType type) override;

	ID3DBlob * GetBlob() { return blob; }
	const D3D11ShaderReflection & GetReflection() const { return *reflect; }
	D3D11ShaderReflection * GetReflection() { return reflect; }

private:
	D3D11GraphicResourceManager *manager;
	D3D11ShaderReflection *reflect;
	ID3DBlob *blob;
};

}

#endif // !D3D11_SHADER_BYTECODE_H_
