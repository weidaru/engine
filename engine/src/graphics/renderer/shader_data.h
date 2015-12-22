#ifndef SHADER_DATA_H_
#define SHADER_DATA_H_

#include "utils/s2string.h"
#include "utils/type_info.h"

#include <stdint.h>

namespace s2 {

class Sampler;
class ShaderResource;

class ShaderData {
public:
	virtual ~ShaderData() {}

	template <typename T>
	bool SetUniform(const s2string &name, const T &value) {
		return SetUniform(name, TypeInfoManager::GetSingleton()->Get<T>(),&value);
	}
	
	virtual bool SetUniform(const s2string &name, const void * value, uint32_t size) = 0;
	
	virtual bool 					SetSampler(const s2string &name, Sampler *sampler) = 0;
	virtual Sampler * 			GetSampler(const s2string &name) = 0;
	virtual bool 					SetShaderResource(const s2string &name, ShaderResource *shader_resource) = 0;
	virtual ShaderResource *		GetShaderResource(const s2string &name) = 0;
	
	virtual const s2string &	GetLastError() = 0;
	
protected:
	virtual bool 					SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value) = 0;
};

}

#endif // !SHADER_DATA_H_
