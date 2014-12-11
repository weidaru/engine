#ifndef D3D11_SHADER_HELPER_h
#define D3D11_SHADER_HELPER_h

#include <vector>

class ID3D11DeviceContext;


namespace s2 {

class D3D11ConstantBuffer;
class D3D11ShaderReflection;

struct D3D11ShaderHelper {
	enum ShaderType {VERTEX, PIXEL};
};

class ConstantBufferContainer{
public:
	

private:
	typedef std::vector<D3D11ConstantBuffer *> CBVector;

public:
	ConstantBufferContainer(D3D11ShaderReflection *_reflect);
	
	bool SetUniform(const s2string &name, const void * value, unsigned int size);
	bool SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value);
	
	void Setup(ID3D11DeviceContext *context, D3D11ShaderHelper::ShaderType shader_type);
	
private:
	D3D11ShaderReflection *reflect;

	CBVector cbs;
};

class SamplerContainer {
public:
	
	
private:
	
};

class ResourceContainer {
public:

private:

};
	
}

#endif		//D3D11_SHADER_HELPER_h
