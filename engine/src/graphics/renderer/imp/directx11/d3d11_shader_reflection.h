#ifndef D3D11_SHADER_REFLECTION_H_
#define D3D11_SHADER_REFLECTION_H_

#include "utils/s2string.h"

#include <vector>
#include <map>
#include <stdint.h>

struct ID3D11ShaderReflection;
struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;
struct _D3D11_SHADER_DESC;
typedef struct _D3D11_SHADER_DESC D3D11_SHADER_DESC;
struct _D3D11_SHADER_TYPE_DESC;
typedef struct _D3D11_SHADER_TYPE_DESC D3D11_SHADER_TYPE_DESC;
struct ID3D11ShaderReflectionType;

namespace s2 {

class TypeInfo;

/**
 * You know what, Directx assumes binary compatible in c++. Aha!
 * Guess all the microsoft stuffs only play with themselves, this does make sense.
 * On the hlsl side things(constants) are always packed in 16 bytes. On c++ side, things vary.
 * Anyway, as long as we know the alignment and do not use virtual functions or multiple inheritance, things should play safe.
 * We need to enforce the 16 bytes packing rule on c++ side.
 */
class D3D11ShaderReflection {
public:
	struct ConstantBuffer {
		uint32_t slot_index;
		s2string name;
		uint32_t size;
		
		std::vector<s2string> uniforms;
	};

	struct Uniform {
		uint32_t cb_slot_index;
		s2string cb_name;
		
		s2string name;
		s2string type_name;
		uint32_t offset;
		uint32_t size;
	};
	
	//Directx 11 registers are all 4-component-32-bit.
	struct Parameter {
		uint32_t index;
		s2string type_name;			//Be either float, int or uint32_t
		uint32_t size;		
		s2string semantic;
		uint32_t semantic_index;

		int stream;						//Will be set to >=0 if that parameter is an output of a geometry shader.
	};
	
	struct Sampler {
		uint32_t slot_index;
		s2string name;
		bool is_compare_sampler;
	};
	
	enum ShaderResourceType {
		TEXTURE
	};
	
	struct ShaderResource {
		uint32_t slot_index;
		s2string name;
		ShaderResourceType type;
	};

private:
	typedef std::vector<Parameter> ParameterVector;
	typedef std::vector<ConstantBuffer> CBVector;
	typedef std::map<s2string, Uniform> UniformMap;
	typedef std::vector<Sampler> SamplerVector;
	typedef std::vector<ShaderResource> ShaderResourceVector;
	
public:
	D3D11ShaderReflection(const s2string &_filepath, ID3DBlob *shader_blob);
	~D3D11ShaderReflection();
	
	const D3D11ShaderReflection::ConstantBuffer & GetConstantBuffer(uint32_t index) const;
	uint32_t GetConstantBufferSize() const;
	//All the thing in constant buffer(cbuffer or tbuffer)
	const D3D11ShaderReflection::Uniform & GetUniform(const s2string &name) const;
	bool HasUniform(const s2string &name) const;
	
	const D3D11ShaderReflection::Parameter & GetInput(uint32_t index) const;
	uint32_t GetInputSize() const;
	const D3D11ShaderReflection::Parameter & GetOutput(uint32_t index) const;
	uint32_t GetOutputSize() const;
	
	const D3D11ShaderReflection::Sampler & GetSampler(uint32_t index) const;
	const D3D11ShaderReflection::Sampler & GetSampler(const s2string &name) const;
	bool HasSampler(const s2string &name) const;
	uint32_t GetSamplerSize() const;
	uint32_t GetSamplerIndex(const s2string  &name) const;
	
	const D3D11ShaderReflection::ShaderResource & GetShaderResource(uint32_t index) const;
	const D3D11ShaderReflection::ShaderResource & GetShaderResource(const s2string &name) const;
	bool HasShaderResource(const s2string &name) const;
	uint32_t GetShaderResourceSize() const;
	uint32_t GetShaderResourceIndex(const s2string &name) const;
	
private:
	void PopulateCBAndUniforms(const D3D11_SHADER_DESC &desc);
	void PopulateInputs(const D3D11_SHADER_DESC &desc);
	void PopulateOutputs(const D3D11_SHADER_DESC &desc);
	void PopulateResources(const D3D11_SHADER_DESC &desc);

	//No copy
	D3D11ShaderReflection(const D3D11ShaderReflection&);
	D3D11ShaderReflection & operator=(const D3D11ShaderReflection&);
	
private:
	s2string filepath;

	ID3D11ShaderReflection *reflect;
	
	CBVector cbs;
	UniformMap uniforms;
	
	ParameterVector inputs;
	ParameterVector outputs;
	SamplerVector samplers;
	ShaderResourceVector shader_resources;
};

}



#endif		//D3D11_SHADER_REFLECTION_H_