#ifndef D3D11_PIXEL_SHADER_H_
#define D3D11_PIXEL_SHADER_H_

#include "renderer/pixel_shader.h"
#include "d3d11_texture2d.h"

#include <vector>

struct ID3D11PixelShader;
struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;

namespace s2 {

class D3D11GraphicResourceManager;
class D3D11ConstantBuffer;
class D3D11Texture2D;
class D3D11ShaderReflection;
class D3D11Sampler;
class ConstantBufferContainer;
class SamplerContainer;
class ShaderResourceContainer;


class D3D11PixelShader : public PixelShader {
public:
	D3D11PixelShader(D3D11GraphicResourceManager *_manager);
	virtual ~D3D11PixelShader();
	virtual bool Initialize(const s2string &path, const s2string &entry_point);
	
	virtual bool SetUniform(const s2string &name, const void * value, unsigned int size);
	
	virtual bool SetSampler(const s2string &name, Sampler *sampler);
	virtual Sampler * GetSampler(const s2string &name);
	virtual bool SetTexture2D(const s2string &name, Texture2D *texure);
	virtual Texture2D * GetTexture2D(const s2string &name);
	
	virtual const s2string & GetLastError() { return error; }
	const D3D11ShaderReflection & GetReflection() const { return *reflect; }
	ID3DBlob * GetBlob() { return blob; }
	
	/*****************D3D11 exclusive.******************/
	void Setup();
	
protected:
	//That is everything in constant buffer, in the sense of Directx 11.
	virtual bool SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value);
	
private:
	void Check();
	void Clear();
	
private:
	D3D11GraphicResourceManager *manager;
	
	ID3D11PixelShader *shader;
	D3D11ShaderReflection *reflect;
	ID3DBlob *blob;
	s2string error;
	
	ConstantBufferContainer *cb_container;
	SamplerContainer *sampler_container;
	ShaderResourceContainer *sr_container;
};


}


#endif		//D3D11_PIXEL_SHADER_H_