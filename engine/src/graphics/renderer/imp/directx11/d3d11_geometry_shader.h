#ifndef D3D11_GEOMETRY_SHADER_H_
#define D3D11_GEOMETRY_SHADER_H_

#include "graphics/renderer/geometry_shader.h"

#include "d3d11_shader_bytecode.h"

struct ID3D11GeometryShader;

namespace s2 {

class D3D11GraphicResourceManager;
	
class D3D11GeometryShader : public GeometryShader {
public:
	D3D11GeometryShader(D3D11GraphicResourceManager *_manager);
	virtual ~D3D11GeometryShader();

	virtual bool Initialize(ShaderBytecode *bytecode) override;
	virtual bool Initialize(const s2string &path, const s2string &entry_point) override;
	virtual bool Initialize(ShaderBytecode *bytecode, int rasterized_stream, const std::vector<StreamOutDescriptor> &streamouts) override;

	virtual const ShaderBytecode * GetShaderBytecode() const override { return bytecode; }
	virtual ShaderBytecode * GetShaderBytecode() override { return bytecode; }

	ID3D11GeometryShader * GetInternal() { return shader; }

private:
	D3D11GraphicResourceManager *manager;

	D3D11ShaderBytecode *bytecode;
	bool delete_bytecode;

	ID3D11GeometryShader *shader;
};
	
}

#endif		//D3D11_GEOMETRY_SHADER_H_