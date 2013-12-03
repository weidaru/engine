#ifndef D3D_VERTEX_SHADER_H_
#define D3D_VERTEX_SHADER_H_

#include <string>
#include <map>

struct ID3D11VertexShader;
struct ID3D11ShaderReflection;
struct ID3D10Blob;
struct ID3D11Device;

class D3DConstantBufferBindInfo;
class D3DShaderResourceBindInfo;
class D3DSamplerBindInfo;

class D3DVertexShader {
private:

public:
	D3DVertexShader();
	~D3DVertexShader();
	
	bool LoadFromFile(ID3D11Device *device, const std::string &file_name, const std::string &entry_func_name);
	
	D3DConstantBufferBindInfo * GetConstantBufferBindInfo();
	D3DShaderResourceBindInfo * GetShaderResourceBindInfo();
	D3DSamplerBindInfo * GetSamplerBindInfo();

	ID3D11VertexShader * GetInternal() { return vs_; }
	ID3D11ShaderReflection * GetReflection() { return reflection_; }
	ID3D10Blob * GetVertexBuffer() { return vs_buffer_; }

	
private:
	void CreateConstantBufferReflection();
	void CreateShaderResourceReflection();
	void CreateSamplerReflection();

private:
	ID3D11VertexShader *vs_;
	ID3D11ShaderReflection *reflection_;
	ID3D10Blob *vs_buffer_;

};

#endif 		//DED_VERTEX_SHADER_H_