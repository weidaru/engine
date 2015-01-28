#ifndef D3D11_SHADER_HELPER_h
#define D3D11_SHADER_HELPER_h

#include <vector>
#include <map>

#include "utils/s2string.h"

struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

namespace s2 {

class TypeInfo;
class D3D11GraphicResourceManager;
class D3D11ConstantBuffer;
class D3D11ShaderReflection;
class D3D11Sampler;
class D3D11Texture2D;
class Sampler;
class Texture2D;
class Resource;

struct D3D11ShaderHelper {
	enum ShaderType {VERTEX, PIXEL, GEOMETRY};
};

class ConstantBufferContainer{
private:
	typedef std::vector<std::pair<unsigned int, D3D11ConstantBuffer *> > CBVector;

public:
	ConstantBufferContainer(D3D11GraphicResourceManager *manager, D3D11ShaderReflection *_reflect);
	~ConstantBufferContainer();
	
	bool SetUniform(const s2string &name, const void * value, unsigned int size, s2string *error);
	bool SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value, s2string *error);
	
	void Setup(ID3D11DeviceContext *context, D3D11ShaderHelper::ShaderType shader_type);
	
private:
	D3D11ShaderReflection *reflect;

	CBVector cbs;
};

class SamplerContainer {
private:
	typedef std::vector<std::pair<unsigned int, D3D11Sampler *> > SamplerVector;

public:
	SamplerContainer(D3D11ShaderReflection *_reflect);
	
	bool SetSampler(const s2string &name, Sampler *sampler, s2string *error);
	D3D11Sampler* GetSampler(const s2string &name, s2string *error);
	
	void Setup(ID3D11DeviceContext *context, D3D11ShaderHelper::ShaderType shader_type);
	
private:
	D3D11ShaderReflection *reflect;
	
	SamplerVector samplers;
};

class ShaderResourceContainer {
public:
	typedef std::vector<std::pair<unsigned int, Resource *> > BindingVector;

private:
	struct Info {
		unsigned int reflect_index;
		Resource *resource;
		ID3D11ShaderResourceView *view;
	};

	typedef std::vector<Info> ShaderResourceVector;

public:
	ShaderResourceContainer(D3D11ShaderReflection *_reflect);

	bool SetTexture2D(const s2string &name, Texture2D *_texture, s2string *error);
	D3D11Texture2D * GetTexture2D(const s2string &name, s2string *error);
	
	void Setup(ID3D11DeviceContext *context, D3D11ShaderHelper::ShaderType shader_type);
	
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
