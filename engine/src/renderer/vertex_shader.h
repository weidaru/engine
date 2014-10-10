#ifndef VERTEX_SHADER_H_
#define VERTEX_SHADER_H_

#include "resource.h"
#include "utils/s2string.h"

namespace s2 {
class ConstantBuffer;
class Sampler;
class VertexBuffer;
class IndexBuffer;
class Texture1D;
class Texture2D;
class Texture3D;

class VertexShader : public Resource {
public:
	virtual 							~VertexShader() {}
	virtual bool 					Initialize(const s2string &path) = 0;

	virtual bool 					SetConstantBuffer(const s2string &name, ConstantBuffer *cb) = 0;
	virtual ConstantBuffer * 	GetConstantBuffer(const s2string &name) = 0;
	virtual bool 					SetSampler(const s2string &name, Sampler *sampler) = 0;
	virtual Sampler * 			GetSampler(const s2string &name) = 0;
	virtual bool 					SetResource(const s2string &name, Texture1D *resource) = 0;
	virtual bool 					SetResource(const s2string &name, Texture2D *resource) = 0;
	virtual bool 					SetResource(const s2string &name, Texture3D *resource) = 0;
	virtual Resource * 			GetResource(const s2string &name) = 0;
	
	virtual void 					GetLastError(s2string *str) = 0;
};

}

#endif		//VERTEX_SHADER_H_
