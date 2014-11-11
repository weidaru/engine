#ifndef VERTEX_SHADER_H_
#define VERTEX_SHADER_H_

#include "resource.h"
#include "utils/s2string.h"
#include "utils/type_info.h"

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
	virtual bool 					Initialize(const s2string &path, const s2string &entry_point) = 0;

	template <typename T>
	bool SetUniform(const s2string &name, const T *value) {
		return SetUniform(name, TypeInfoManager::GetSingleton()->Get<T>(),value);
	}
	
	//This will be version without compatibility check, more like a c style.
	virtual bool SetUniform(const s2string &name, const void * value, unsigned int size) = 0;
	
	virtual bool 					SetSampler(const s2string &name, Sampler *sampler) = 0;
	virtual Sampler * 			GetSampler(const s2string &name) = 0;
	virtual bool 					SetResource(const s2string &name, Texture1D *resource) = 0;
	virtual bool 					SetResource(const s2string &name, Texture2D *resource) = 0;
	virtual bool 					SetResource(const s2string &name, Texture3D *resource) = 0;
	virtual Resource * 			GetResource(const s2string &name) = 0;
	template<typename T>
	T * GetResource() {
		Resource *resource = GetResource();
	#ifdef NDEBUG
		return static_cast<T *>(resource);
	#else
		return dynamic_cast<T *>(resource);
	#endif
	}
	
	virtual const s2string &	GetLastError() = 0;
	
protected:
	virtual bool 					SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value) = 0;
};

}

#endif		//VERTEX_SHADER_H_
