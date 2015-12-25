#ifndef D3D11_SHADER_DATA_H
#define D3D11_SHADER_DATA_H

#include <vector>
#include <map>
#include <stdint.h>

#include "utils/s2string.h"

#include "graphics/renderer/shader_data.h"

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

enum class ShaderType {
	VERTEX, PIXEL, GEOMETRY
};

class ConstantBufferContainer{
private:
	typedef std::vector<std::pair<uint32_t, D3D11ConstantBuffer *> > CBVector;

public:
	ConstantBufferContainer(D3D11GraphicResourceManager *manager, D3D11ShaderReflection *_reflect);
	~ConstantBufferContainer();
	
	bool SetUniform(const s2string &name, const void * value, uint32_t size, s2string *error);
	bool SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value, s2string *error);
	
	void Setup(D3D11GraphicPipeline *pipeline, ShaderType shader_type);
	void UnBind(D3D11GraphicPipeline *pipeline, ShaderType shader_type);

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
	
	void Setup(D3D11GraphicPipeline *pipeline, ShaderType shader_type);
	void UnBind(D3D11GraphicPipeline *pipeline, ShaderType shader_type);
	
private:
	D3D11ShaderReflection *reflect;
	
	SamplerVector samplers;
};

class ShaderResourceContainer {
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
	
	void Setup(D3D11GraphicPipeline *pipeline, ShaderType shader_type);
	void UnBind(D3D11GraphicPipeline *pipeline, ShaderType shader_type);
	
private:
	D3D11ShaderReflection *reflect;
	
	ShaderResourceVector shader_resources;
};

class D3D11ShaderData : public ShaderData {
public:
	D3D11ShaderData(D3D11GraphicResourceManager *manager, D3D11ShaderReflection *reflect);

	virtual ~D3D11ShaderData();

	virtual bool SetUniform(const s2string &name, const void * value, uint32_t size) override;
	
	virtual bool 					SetSampler(const s2string &name, Sampler *sampler) override;
	virtual Sampler * 			GetSampler(const s2string &name) override;
	virtual bool 					SetShaderResource(const s2string &name, ShaderResource *shader_resource) override;
	virtual ShaderResource *		GetShaderResource(const s2string &name) override;

	void Setup(D3D11GraphicPipeline *pipeline, ShaderType type);
	void UnBind(D3D11GraphicPipeline *pipeline, ShaderType type);
	
protected:
	virtual bool 					SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value) override;

private:
	ConstantBufferContainer cb_container;
	SamplerContainer sampler_container;
	ShaderResourceContainer sr_container;
};
	
}

#endif		//D3D11_SHADER_DATA_H
