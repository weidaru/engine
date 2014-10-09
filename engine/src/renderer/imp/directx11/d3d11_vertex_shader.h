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
	
class D3D11VertexShader : public VertexShader {
public:
	D3D11VertexShader(D3D11GraphicResourceManager *_manager);
	virtual bool Initialize(const s2string &path, PrimitiveTopology _topology);
	virtual ~D3D11VertexShader();

	virtual bool SetConstantBuffer(const s2string &name, ConstantBuffer *cb);
	virtual ConstantBuffer * GetConstantBuffer(const s2string &name);
	virtual bool SetSampler(const s2string &name, Sampler *sampler);
	virtual Sampler * GetSampler(const s2string &name);
	virtual bool SetResource(const s2string &name, Texture1D *resource);
	virtual bool SetResource(const s2string &name, Texture2D *resource);
	virtual bool SetResource(const s2string &name, Texture3D *resource);
	virtual Resource * GetResource(const s2string &name);
	
	virtual void GetLastError(s2string *str);
	
	/*****************D3D11 exclusive.******************/
	void Flush();
	
private:
	void Check();
	void Clear();

private:
	D3D11GraphicResourceManager *manager;
	
	std::vector<D3D11ConstantBuffer *> cbs;
	ID3D11VertexShader *shader;

	s2string error;
};
	
}



#endif
