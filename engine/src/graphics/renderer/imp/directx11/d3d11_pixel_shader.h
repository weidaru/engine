#ifndef D3D11_PIXEL_SHADER_H_
#define D3D11_PIXEL_SHADER_H_

#include "graphics/renderer/pixel_shader.h"
#include "d3d11_sampler.h"
#include "d3d11_resource_view.h"

struct ID3D11PixelShader;
struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;

namespace s2 {

class D3D11GraphicResourceManager;
class D3D11GraphicPipeline;
class D3D11ShaderReflection;
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
	virtual D3D11Sampler * GetSampler(const s2string &name);
	virtual bool SetShaderResource(const s2string &name, ShaderResource  *shader_resource);
	virtual D3D11ShaderResource * GetShaderResource(const s2string &name);
	
	virtual const s2string & GetLastError() { return error; }
	
	/*****************D3D11 exclusive.******************/
	const D3D11ShaderReflection & GetReflection() const { return *reflect; }
	ID3DBlob * GetBlob() { return blob; }
	void Setup(D3D11GraphicPipeline *pipeline);
	void Unbind(D3D11GraphicPipeline *pipeline);
	const ConstantBufferContainer & GetConstantBufferContainer() const { return *cb_container; }
	const SamplerContainer & GetSamplerContainer() const { return *sampler_container; }
	const ShaderResourceContainer & GetShaderResourceContainer() const { return *sr_container; }
	
protected:
	//That is everything in constant buffer, in the sense of Directx 11.
	virtual bool SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value);
	
private:
	void Check();
	
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