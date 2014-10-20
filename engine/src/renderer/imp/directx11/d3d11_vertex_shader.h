#ifndef D3D11_VERTEX_SHADER_H_
#define D3D11_VERTEX_SHADER_H_

#include "renderer/vertex_shader.h"

#include <vector>

struct ID3D11VertexShader;

namespace s2 {

class D3D11GraphicResourceManager;
class D3D11ConstantBuffer;
class D3D11VertexBuffer;
class D3D11IndexBuffer;
class D3D11ShaderReflection;
	
class D3D11VertexShader : public VertexShader {
public:
	D3D11VertexShader(D3D11GraphicResourceManager *_manager);
	virtual bool Initialize(const s2string &path, const s2string &entry_point);
	virtual ~D3D11VertexShader();

	virtual bool SetUniform(const s2string &name, const void * value, unsigned int size);
	virtual bool SetSampler(const s2string &name, Sampler *sampler);
	virtual Sampler * GetSampler(const s2string &name);
	virtual bool SetResource(const s2string &name, Texture1D *resource);
	virtual bool SetResource(const s2string &name, Texture2D *resource);
	virtual bool SetResource(const s2string &name, Texture3D *resource);
	virtual Resource * GetResource(const s2string &name);
	
	virtual const s2string & GetLastError() { return error; } 
	
	/*****************D3D11 exclusive.******************/
	void Flush();
	
protected:
	//That is everything in constant buffer, in the sense of Directx 11.
	virtual bool SetUniform(const s2string &name, const TypeInfo &type_info, const void *value);
	
private:
	void Check();
	void Clear();

private:
	D3D11GraphicResourceManager *manager;
	
	std::vector<D3D11ConstantBuffer *> cbs;
	ID3D11VertexShader *shader;
	D3D11ShaderReflection *reflect;
	s2string error;
};
	
}



#endif
