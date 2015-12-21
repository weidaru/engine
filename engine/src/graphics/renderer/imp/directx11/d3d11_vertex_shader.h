#ifndef D3D11_VERTEX_SHADER_H_
#define D3D11_VERTEX_SHADER_H_

#include "graphics/renderer/vertex_shader.h"

#include "d3d11_shader_bytecode.h"

struct ID3D11VertexShader;

namespace s2 {

class D3D11GraphicResourceManager;
	
class D3D11VertexShader : public VertexShader {
public:
	D3D11VertexShader(D3D11GraphicResourceManager *_manager);
	virtual ~D3D11VertexShader();

	virtual bool Initialize(ShaderBytecode *bytecode) override;
	virtual bool Initialize(const s2string &path, const s2string &entry_point) override;

	virtual const ShaderBytecode * GetShaderBytecode() const override { return bytecode; }
	virtual ShaderBytecode * GetShaderBytecode() override { return bytecode; }

	ID3D11VertexShader * GetInternal() { return shader; }

private:
	D3D11GraphicResourceManager *manager;

	D3D11ShaderBytecode *bytecode;
	bool delete_bytecode;

	ID3D11VertexShader *shader;
};
	
}



#endif			//!D3D11_VERTEX_SHADER_H_
