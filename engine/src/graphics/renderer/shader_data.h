#ifndef SHADER_DATA_H_
#define SHADER_DATA_H_

#include "utils/s2string.h"
#include "utils/type_info.h"

#include <stdint.h>

namespace s2 {

class Sampler;
class ShaderResource;
class ShaderBytecode;

class ShaderData {
public:
	virtual ~ShaderData() {}

	virtual bool Initialize(ShaderBytecode *bytecode) = 0;

	template <typename T>
	bool SetUniform(const s2string &name, const T &value) {
		return SetUniform(name, TypeInfoManager::GetSingleton()->Get<T>(),&value);
	}
	
	virtual bool SetUniform(const s2string &name, const void * value, uint32_t size) = 0;
	
	virtual bool 					SetSampler(const s2string &name, Sampler *sampler) = 0;
	virtual Sampler * 			GetSampler(const s2string &name) = 0;
	virtual bool 					SetShaderResource(const s2string &name, ShaderResource *shader_resource) = 0;
	virtual ShaderResource *		GetShaderResource(const s2string &name) = 0;
	
	s2string GetLastError() { return error; }
	
protected:
	virtual bool 					SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value) = 0;

protected:
	s2string error;
};

}

#endif // !SHADER_DATA_H_
