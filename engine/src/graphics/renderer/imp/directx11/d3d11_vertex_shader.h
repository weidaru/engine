#ifndef D3D11_VERTEX_SHADER_H_
#define D3D11_VERTEX_SHADER_H_

#include "d3d11_sampler.h"
#include "d3d11_resource_view.h"

#include "graphics/renderer/vertex_shader.h"

#include <vector>

struct ID3D11VertexShader;
struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;

namespace s2 {

class D3D11GraphicResourceManager;
class D3D11GraphicPipeline;
class D3D11ShaderReflection;
class ConstantBufferContainer;
class SamplerContainer;
class ShaderResourceContainer;
	
class D3D11VertexShader : public VertexShader {
public:
	D3D11VertexShader(D3D11GraphicResourceManager *_manager);
	virtual bool Initialize(const s2string &path, const s2string &entry_point);
	virtual ~D3D11VertexShader();

	virtual bool SetUniform(const s2string &name, const void * value, uint32_t size);
	virtual bool SetSampler(const s2string &name, Sampler *sampler);
	virtual D3D11Sampler* GetSampler(const s2string &name);
	virtual bool SetShaderResource(const s2string &name, ShaderResource *resource);
	virtual D3D11ShaderResource * GetShaderResource(const s2string &name);
	
	virtual const s2string & GetLastError() { return error; } 
	
	/*****************D3D11 exclusive.******************/
	void Setup(D3D11GraphicPipeline *pipeline);
	void Unbind(D3D11GraphicPipeline *pipeline);
	const D3D11ShaderReflection & GetReflection() const { return *reflect; }
	ID3DBlob * GetBlob() const { return blob;}
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
	
	ID3D11VertexShader *shader;
	D3D11ShaderReflection *reflect;
	ID3DBlob *blob;
	
	ConstantBufferContainer *cb_container;
	SamplerContainer *sampler_container;
	ShaderResourceContainer *sr_container;

	s2string error;
};
	
}



#endif
