#ifndef GEOMETRY_SHADER_H_
#define GEOMETRY_SHADER_H_

#include "resource.h"
#include "utils/s2string.h"
#include "utils/type_info.h"

namespace s2 {
class ShaderBytecode;

struct StreamOutDescriptor {
	uint32_t index;
	uint32_t stream_index;
};

class GeometryShader : public Resource {
public:
	virtual ~GeometryShader() {}
	virtual bool Initialize(ShaderBytecode *bytecode) = 0;
	virtual bool Initialize(const s2string &path, const s2string &entry_point)  = 0;
	virtual bool Initialize(ShaderBytecode *bytecode, int rasterized_stream, const std::vector<StreamOutDescriptor> &streamouts) = 0;

	virtual ShaderBytecode *GetShaderBytecode() = 0;

	s2string GetError() { return error; }

protected:
	s2string error;
};

}



#endif		//GEOMETRY_SHADER_H_