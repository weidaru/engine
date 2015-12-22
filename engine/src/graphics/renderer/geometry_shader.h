#ifndef GEOMETRY_SHADER_H_
#define GEOMETRY_SHADER_H_

#include "resource.h"
#include "utils/s2string.h"
#include "utils/type_info.h"

namespace s2 {
class Sampler;
class ShaderResource;

struct StreamOutDescriptor {
	uint32_t index;
	uint32_t stream_index;
};

class GeometryShader : public Resource {
public:
	virtual ~GeometryShader() {}
	virtual bool Initialize(const ShaderBytecode &bytecode) = 0;
	virtual bool Initialize(const ShaderBytecode &bytecode, const std::vector<StreamOutDescriptor> &streamouts) = 0;

	virtual ShaderData *GetShaderData() = 0;

	s2string GetError() { return error; }

protected:
	s2string error;
};

}



#endif		//GEOMETRY_SHADER_H_