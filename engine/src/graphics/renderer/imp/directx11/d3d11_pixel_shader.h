#ifndef D3D11_PIXEL_SHADER_H_
#define D3D11_PIXEL_SHADER_H_

#include "graphics/renderer/pixel_shader.h"

#include "d3d11_shader_bytecode.h"

struct ID3D11PixelShader;

namespace s2 {

class D3D11GraphicResourceManager;
	
class D3D11PixelShader : public PixelShader {
public:
	D3D11PixelShader(D3D11GraphicResourceManager *_manager);
	virtual ~D3D11PixelShader();

	virtual bool Initialize(ShaderBytecode *bytecode) override;
	virtual bool Initialize(const s2string &path, const s2string &entry_point) override;

	virtual ShaderBytecode * GetShaderBytecode() { return bytecode; }

	ID3D11PixelShader * GetInternal() { return shader; }

private:
	D3D11GraphicResourceManager *manager;

	D3D11ShaderBytecode *bytecode;
	bool delete_bytecode;

	ID3D11PixelShader *shader;
};
	
}


#endif		//D3D11_PIXEL_SHADER_H_