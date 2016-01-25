#ifndef D3D11_SHADER_DATA_H
#define D3D11_SHADER_DATA_H

#include <vector>
#include <map>
#include <stdint.h>

#include "utils/s2string.h"

#include "graphics/renderer/shader_data.h"
#include "graphics/renderer/shader_bytecode.h"

struct ID3D11ShaderResourceView;

namespace s2 {

class TypeInfo;
class D3D11GraphicResourceManager;
class D3D11GraphicPipeline;
class D3D11ConstantBuffer;
class D3D11ShaderReflection;
class D3D11Sampler;
class D3D11ShaderResource;
class D3D11ShaderBytecode;
class D3D11UnorderedAccess;

class ConstantBufferContainer{
private:
	typedef std::vector<std::pair<uint32_t, D3D11ConstantBuffer *> > CBVector;

public:
	ConstantBufferContainer(D3D11GraphicResourceManager *manager, D3D11ShaderReflection *_reflect);
	~ConstantBufferContainer();
	
	bool SetUniform(const s2string &name, const void * value, uint32_t size, s2string *error);
	bool SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value, s2string *error);
	
	void Flush(D3D11GraphicPipeline *pipeline);

	void Setup(D3D11GraphicPipeline *pipeline, ShaderType shader_type);
	static void UnBind(D3D11GraphicPipeline *pipeline, ShaderType shader_type);

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
	static void UnBind(D3D11GraphicPipeline *pipeline, ShaderType shader_type);
	
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
	static void UnBind(D3D11GraphicPipeline *pipeline, ShaderType shader_type);
	
private:
	D3D11ShaderReflection *reflect;
	
	ShaderResourceVector shader_resources;
};

class UnorderedAccessContainer {
private:
	struct Info {
		uint32_t reflect_index;
		int initial_counter;
		D3D11UnorderedAccess *uav;

		Info() {
			reflect_index = 0;
			initial_counter = -1;
			uav = 0;
		}
	};
	typedef std::vector<Info> UnorderedAccessVector;

public:
	UnorderedAccessContainer(D3D11ShaderReflection *_reflect);

	bool SetUnorderedAccess(const s2string &name, UnorderedAccess * _unordered_access, int initial_counter, s2string *error);
	D3D11UnorderedAccess * GetUnorderedAccess(const s2string &name, s2string *error);

	void Setup(D3D11GraphicPipeline *pipeline, ShaderType shader_type);
	static void UnBind(D3D11GraphicPipeline *pipeline, ShaderType shader_type);

private:
	D3D11ShaderReflection *reflect;

	UnorderedAccessVector uavs;
};

class D3D11ShaderData : public ShaderData {
public:
	D3D11ShaderData(D3D11GraphicResourceManager *manager);
	virtual ~D3D11ShaderData();

	virtual bool Initialize(ShaderBytecode *bytecode) override;

	virtual bool SetUniform(const s2string &name, const void * value, uint32_t size) override;
	
	virtual bool 					SetSampler(const s2string &name, Sampler *sampler) override;
	virtual Sampler * 			GetSampler(const s2string &name) override;
	virtual bool 					SetShaderResource(const s2string &name, ShaderResource *shader_resource) override;
	virtual ShaderResource *		GetShaderResource(const s2string &name) override;
	virtual bool SetUnorderedAccess(const s2string &name, UnorderedAccess *unordered_access, int initial_counter) override;
	virtual UnorderedAccess * GetUnorderedAccess(const s2string &name) override;

	virtual void FlushConstantBuffer(GraphicPipeline *pipeline) override;

	void Setup(D3D11GraphicPipeline *pipeline, ShaderType type);
	static void UnBind(D3D11GraphicPipeline *pipeline, ShaderType type);
	
protected:
	virtual bool 					SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value) override;

private:
	D3D11GraphicResourceManager *manager;
	ConstantBufferContainer *cb_container;
	SamplerContainer *sampler_container;
	ShaderResourceContainer *sr_container;
	UnorderedAccessContainer *uav_container;

	D3D11ShaderBytecode *bytecode;
};
	
}

#endif		//D3D11_SHADER_DATA_H
