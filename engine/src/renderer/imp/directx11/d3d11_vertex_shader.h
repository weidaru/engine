#ifndef D3D11_VERTEX_SHADER_H_
#define D3D11_VERTEX_SHADER_H_

#include "d3d11_sampler.h"
#include "d3d11_texture2d.h"

#include "renderer/vertex_shader.h"

#include <vector>

struct ID3D11VertexShader;
struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;

namespace s2 {

class D3D11GraphicResourceManager;
class D3D11ConstantBuffer;
class D3D11VertexBuffer;
class D3D11IndexBuffer;
class D3D11ShaderReflection;
class D3D11Sampler;

class ConstantBufferContainer;
class SamplerContainer;
	
class D3D11VertexShader : public VertexShader {
public:
	D3D11VertexShader(D3D11GraphicResourceManager *_manager);
	virtual bool Initialize(const s2string &path, const s2string &entry_point);
	virtual ~D3D11VertexShader();

	virtual bool SetUniform(const s2string &name, const void * value, unsigned int size);
	virtual bool SetSampler(const s2string &name, Sampler *sampler);
	virtual D3D11Sampler* GetSampler(const s2string &name);
	virtual bool SetTexture2D(const s2string &name, Texture2D *resource);
	virtual D3D11Texture2D * GetTexture2D(const s2string &name);
	
	virtual const s2string & GetLastError() { return error; } 
	
	/*****************D3D11 exclusive.******************/
	void Setup();
	const D3D11ShaderReflection & GetReflection() const { return *reflect; }
	ID3DBlob * GetBlob() const { return blob;}
	
protected:
	//That is everything in constant buffer, in the sense of Directx 11.
	virtual bool SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value);
	
private:
	void Check();
	void Clear();

private:
	D3D11GraphicResourceManager *manager;
	
	ID3D11VertexShader *shader;
	D3D11ShaderReflection *reflect;
	ID3DBlob *blob;
	
	ConstantBufferContainer *cb_container;
	SamplerContainer *sampler_container;

	s2string error;
};
	
}



#endif
