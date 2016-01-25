#ifndef D3D11_COMPUTE_SHADER_H_
#define D3D11_COMPUTE_SHADER_H_

#include "graphics/renderer/compute_shader.h"

#include "d3d11_shader_bytecode.h"

struct ID3D11ComputeShader;

namespace s2 {

class D3D11GraphicResourceManager;

class D3D11ComputeShader : public ComputeShader {
public:
	 D3D11ComputeShader(D3D11GraphicResourceManager *manager);

	virtual ~D3D11ComputeShader();
	virtual bool Initialize(ShaderBytecode *bytecode) override;
	virtual bool Initialize(const s2string &path, const s2string &entry_point)  override;

	virtual const D3D11ShaderBytecode * GetShaderBytecode() const override { return bytecode; }
	virtual D3D11ShaderBytecode * GetShaderBytecode() override { return bytecode; }

	ID3D11ComputeShader * GetInternal() { return shader; }

private:
	D3D11GraphicResourceManager *manager;

	D3D11ShaderBytecode *bytecode;
	bool delete_bytecode;

	ID3D11ComputeShader *shader;
};


}


#endif // !D3D11_COMPUTE_SHADER_H_
