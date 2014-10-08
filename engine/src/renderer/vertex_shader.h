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
	enum PrimitiveTopology {
		POINT_LIST,
		LINE_LIST,
		LINE_STRIP,
		TRIANGLE_LIST,
		TRIANGLE_STRIP
	};

	enum VertexBufferUsage {
		PER_VERTEX,
		PER_INSTANCE
	};

public:
	virtual 							~VertexShader() {}
	virtual bool 					Initialize(const s2string &path, PrimitiveTopology topology) = 0;
	virtual bool 					SetPrimitiveTopology(PrimitiveTopology newvalue) = 0;
	virtual PrimitiveTopology	GetPrimitiveTopology() = 0;
	virtual bool 					SetConstantBuffer(const s2string &name, ConstantBuffer *cb) = 0;
	virtual ConstantBuffer * 	GetConstantBuffer(const s2string &name) = 0;
	virtual bool 					SetSampler(const s2string &name, Sampler *sampler) = 0;
	virtual Sampler * 			GetSampler(const s2string &name) = 0;
	virtual bool 					SetResource(const s2string &name, Texture1D *resource) = 0;
	virtual bool 					SetResource(const s2string &name, Texture2D *resource) = 0;
	virtual bool 					SetResource(const s2string &name, Texture3D *resource) = 0;
	virtual Resource * 			GetResource(const s2string &name) = 0;
	
	//Input for vertex shader.
	virtual bool 					SetVertexBuffer(unsigned int index, VertexBuffer *buf, VertexBufferUsage usage, const s2string &type_name) = 0;
	virtual VertexBuffer *		GetVertexBuffer(unsigned int index, VertexBufferUsage *usage, s2string *type_name) = 0;
	virtual bool 					SetIndexBuffer(IndexBuffer *buf) = 0;
	virtual IndexBuffer *		GetIndexBuffer() = 0;
	
	virtual void 					GetLastError(s2string *str) = 0;
};

}

#endif		//VERTEX_SHADER_H_
