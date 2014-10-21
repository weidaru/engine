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

struct ShaderTypeInfo {
	struct Member {
		s2string name;
		s2string type_name;
		unsigned int offset;
	};

	s2string name;
	unsigned int size;
	std::vector<Member> members;
};

class ShaderTypeInfoStore {
private:
	typedef std::map<s2string, ShaderTypeInfo> TypeMap;
	typedef std::map<s2string, std::vector<s2string> > CompatibleMap;

public:
	//This should only be used after main starts.
	bool CheckCompatible(const s2string &shader_typename, const s2string &cpp_type) const;
	void MakeCompatible(const s2string &shader_typename, const s2string &cpp_type);
	const ShaderTypeInfo & GetTypeInfo(const s2string &shader_typename) const;
	bool HasTypeInfo(const s2string &shader_typename) const;
	ShaderTypeInfo * CreateTypeInfo(const s2string &shader_typename);
	
private:
	TypeMap types;
	CompatibleMap compatibles;
};

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
		unsigned int size;		
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
	typedef std::vector<Parameter> ParameterVector;
	typedef std::vector<ConstantBuffer> CBVector;
	typedef std::map<s2string, Uniform> UniformMap;
	
	
public:
	D3D11ShaderReflection(const s2string &_filepath, ID3DBlob *shader_blob);
	~D3D11ShaderReflection();
	
	const D3D11ShaderReflection::ConstantBuffer & GetConstantBuffer(unsigned int index) const;
	unsigned int GetConstantBufferSize() const;
	//All the thing in constant buffer(cbuffer or tbuffer)
	const D3D11ShaderReflection::Uniform & GetUniform(const s2string &name) const;
	bool HasUniform(const s2string &name) const;
	
	const D3D11ShaderReflection::Parameter & GetInput(unsigned int index) const;
	unsigned int GetInputSize() const;
	const D3D11ShaderReflection::Parameter & GetOutput(unsigned int index) const;
	unsigned int GetOutputSize() const;
	
	const D3D11ShaderReflection::Sampler & GetSampler(const s2string &name) const;
	bool HasSampler(const s2string &name) const;
	const D3D11ShaderReflection::Resource & GetResource(const s2string &name) const;
	bool HasResource(const s2string &name) const;
	
	bool CheckCompatible(const s2string &shader_typename, const TypeInfo &cpp_type) const;
	const ShaderTypeInfo & GetTypeInfo(const s2string &shader_typename) const;
	bool HasTypeInfo(const s2string &shader_typename) const;
	
	const s2string & GetLastError() const { return error; }
	
	static ShaderTypeInfoStore * GetPrimitiveTypeInfoStore();
	
private:
	void PopulateCBAndUniforms(const D3D11_SHADER_DESC &desc);
	void PopulateInputs(const D3D11_SHADER_DESC &desc);
	void PopulateOutputs(const D3D11_SHADER_DESC &desc);
	void PopulateSamplers(const D3D11_SHADER_DESC &desc);
	void PopulateResources(const D3D11_SHADER_DESC &desc);
	//Only uniforms are parsed for now.
	void ParseShaderType(ID3D11ShaderReflectionType &type);
	void _ParseShaderType(ID3D11ShaderReflectionType &type, const D3D11_SHADER_TYPE_DESC &desc, ShaderTypeInfo *_info);

	//No copy
	D3D11ShaderReflection(const D3D11ShaderReflection&);
	D3D11ShaderReflection & operator=(const D3D11ShaderReflection&);
	
private:
	s2string filepath;

	ID3D11ShaderReflection *reflect;
	
	CBVector cbs;
	UniformMap uniforms;
	
	mutable ShaderTypeInfoStore type_store;
	
	ParameterVector inputs;
	ParameterVector outputs;
	
	s2string error;
};

}



#endif		//D3D11_SHADER_REFLECTION_H_