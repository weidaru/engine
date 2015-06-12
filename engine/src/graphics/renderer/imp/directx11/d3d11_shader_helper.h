#ifndef D3D11_SHADER_HELPER_h
#define D3D11_SHADER_HELPER_h

#include <vector>
#include <map>
#include <stdint.h>

#include "utils/s2string.h"

struct ID3D11ShaderResourceView;

namespace s2 {

class TypeInfo;
class D3D11GraphicResourceManager;
class D3D11GraphicPipeline;
class D3D11ConstantBuffer;
class D3D11ShaderReflection;
class D3D11Sampler;
class D3D11ShaderResource;
class ShaderResource;
class Sampler;
class Resource;

struct D3D11ShaderHelper {
	enum ShaderType {VERTEX, PIXEL, GEOMETRY};
};

class ConstantBufferContainer{
private:
	typedef std::vector<std::pair<uint32_t, D3D11ConstantBuffer *> > CBVector;

public:
	ConstantBufferContainer(D3D11GraphicResourceManager *manager, D3D11ShaderReflection *_reflect);
	~ConstantBufferContainer();
	
	bool SetUniform(const s2string &name, const void * value, uint32_t size, s2string *error);
	bool SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value, s2string *error);
	
	void Setup(D3D11GraphicPipeline *pipeline, D3D11ShaderHelper::ShaderType shader_type);
	void Unbind(D3D11GraphicPipeline *pipeline, D3D11ShaderHelper::ShaderType shader_type);
	
private:
	D3D11ShaderReflection *reflect;

	CBVector cbs;
};

class SamplerContainer {
private:
	typedef std::vector<std::pair<uint32_t, D3D11Sampler *> > SamplerVector;

public:
	SamplerContainer(D3D11ShaderReflection *_reflect);
	
	bool SetSampler(const s2string &name, Sampler *sampler, s2string *error);
	D3D11Sampler* GetSampler(const s2string &name, s2string *error);
	
	void Setup(D3D11GraphicPipeline *pipeline, D3D11ShaderHelper::ShaderType shader_type);
	void Unbind(D3D11GraphicPipeline *pipeline, D3D11ShaderHelper::ShaderType shader_type);
	
private:
	D3D11ShaderReflection *reflect;
	
	SamplerVector samplers;
};

class ShaderResourceContainer {
public:
	typedef std::vector<std::pair<uint32_t, Resource *> > BindingVector;

private:
	struct Info {
		uint32_t reflect_index;
		D3D11ShaderResource *shader_resource;
	};

	typedef std::vector<Info> ShaderResourceVector;

public:
	ShaderResourceContainer(D3D11ShaderReflection *_reflect);

	bool SetShaderResource(const s2string &name, ShaderResource *_shader_resource, s2string *error);
	D3D11ShaderResource * GetShaderResource(const s2string &name, s2string *error);
	
	void Setup(D3D11GraphicPipeline *pipeline, D3D11ShaderHelper::ShaderType shader_type);
	void Unbind(D3D11GraphicPipeline *pipeline, D3D11ShaderHelper::ShaderType shader_type);
	
	/**
	 * New bindings since last draw call.
	 */
	BindingVector GetNewBindings() const;
	
	
private:
	D3D11ShaderReflection *reflect;
	
	ShaderResourceVector shader_resources;
};
	
}

#endif		//D3D11_SHADER_HELPER_h
