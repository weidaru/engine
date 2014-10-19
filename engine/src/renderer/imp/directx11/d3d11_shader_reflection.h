#ifndef D3D11_SHADER_REFLECTION_H_
#define D3D11_SHADER_REFLECTION_H_

#include "utils/s2string.h"
#include "utils/type_info.h"

#include <vector>
#include <map>

struct ID3D11ShaderReflection;
struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;
struct _D3D11_SHADER_DESC;
typedef struct _D3D11_SHADER_DESC D3D11_SHADER_DESC;
struct ID3D11ShaderReflectionType;

namespace s2 {

class D3D11ShaderReflection {
public:
	struct ConstantBuffer {
		unsigned int index;
		s2string name;
		unsigned int size;
		
		std::vector<s2string> uniforms;
	};

	struct Uniform {
		unsigned int cb_index;
		s2string cb_name;
		
		s2string name;
		s2string type_name;
		unsigned int offset;
		unsigned int size;
	};
	
	//Directx 11 registers are all 4-component-32-bit.
	struct Parameter {
		unsigned int index;
		s2string type_name;			//Be either float, int or unsigned int
		unsigned int used_size;		//Used  component size, up to 4
		s2string semantic;
		unsigned int semantic_index;
	};
	
	struct Sampler {
		//Stub
	};
	
	struct Resource {
		//Stub
	};
	
private:
	struct ShaderTypeInfo {
		struct Member {
			s2string name;
			s2string type_name;
			unsigned int offset;
		};

		s2string name;
		unsigned int size;
		std::vector<Member>;
	};
	
private:
	typedef std::vector<Parameter> ParameterVector;
	typedef std::vector<ConstantBuffer> CBVector;
	typedef std::map<s2string, Uniform> UniformMap;
	
	typedef std::map<s2string, ShaderTypeInfo> TypeMap;
	typedef std::map<s2string, std::vector<s2string> > CompatibleMap;
	

public:
	D3D11ShaderReflection(const s2string &_filepath, ID3DBlob *shader_blob);
	~D3D11ShaderReflection();
	
	const D3D11ShaderReflection::ConstantBuffer & GetConstantBuffer(unsigned int index);
	unsigned int GetConstantBufferSize();
	//All the thing in constant buffer(cbuffer or tbuffer)
	const D3D11ShaderReflection::Uniform & GetUniform(const s2string &name);
	bool HasUniform(const s2string &name);
	
	const D3D11ShaderReflection::Parameter & GetInput(unsigned int index);
	unsigned int GetInputSize();
	const D3D11ShaderReflection::Parameter & GetOutput(unsigned int index);
	unsigned int GetOutputSize();
	
	const D3D11ShaderReflection::Sampler & GetSampler(const s2string &name);
	bool HasSampler(const s2string &name);
	const D3D11ShaderReflection::Resource & GetResource(const s2string &name);
	bool HasResource(const s2string &name);
	
	bool CheckCompatible(const s2string &shader_typename, const TypeInfo &cpp_type);
	
	const s2string & GetLastError() { return error; }
	
private:
	void PopulateTypeAndCompatibleMap();
	void PopulateCBAndUniforms(const D3D11_SHADER_DESC &desc);
	void PopulateInputs(const D3D11_SHADER_DESC &desc);
	void PopulateOutputs(const D3D11_SHADER_DESC &desc);
	void PopulateSamplers(const D3D11_SHADER_DESC &desc);
	void PopulateResources(const D3D11_SHADER_DESC &desc);
	void ParseShaderType(const ID3D11ShaderReflectionType &type);
	
private:
	s2string filepath;

	ID3D11ShaderReflection *reflect;
	
	CBVector cbs;
	UniformMap uniforms;
	
	TypeMap types;
	CompatibleMap compatibles;
	
	ParameterVector inputs;
	ParameterVector outputs;
	
	s2string error;
};

}



#endif		//D3D11_SHADER_REFLECTION_H_