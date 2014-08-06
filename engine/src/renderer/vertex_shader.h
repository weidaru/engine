#ifndef VERTEX_SHADER_H_
#define VERTEX_SHADER_H_

#include "resource.h"
#include "utils/s2string.h"

namespace s2 {
class ShaderReflection;
class ConstantBuffer;
class Sampler;
class VertexBuffer;
class IndexBuffer;

class VertexShader : public Resource {
public:
	enum VertexBufferUsage {
		PER_VERTEX,
		PER_INSTANCE
	};

public:
	virtual 							~VertexShader() {}
	virtual void 						SetConstantBuffer(const s2string &name, const ConstantBuffer &cb) = 0;
	virtual ConstantBuffer * 			GetConstantBuffer(const s2string &name) = 0;
	virtual void 						SetSampler(const s2string &name, const Sampler &sampler) = 0;
	virtual Sampler * 					GetSampler(const s2string &name) = 0;
	virtual void 						SetResource(const s2string &name, const Resource &resource) = 0;
	virtual Resource * 					GetResource(const s2string &name) = 0;
	
	//Input for vertex shader.
	virtual void 						SetVertexBuffer(const s2string &name, VertexBuffer *buf, VertexBufferUsage usage) = 0;
	virtual VertexBuffer *				GetVertexBuffer(const s2string &name, VertexBufferUsage *usage) = 0;
	virtual void 						SetIndexBuffer(IndexBuffer *buf) = 0;
	virtual IndexBuffer *				GetIndexBuffer() = 0;
};

}

#endif		//VERTEX_SHADER_H_