#ifndef VERTEX_SHADER_H_
#define VERTEX_SHADER_H_

#include "resource.h"
#include "utils/s2string.h"
#include "utils/type_info.h"

namespace s2 {

class ShaderBytecode;
class ShaderData;

class VertexShader : public Resource {
public:
	virtual ~VertexShader() {}
	virtual bool Initialize(ShaderBytecode *bytecode) = 0;
	virtual bool Initialize(const s2string &path, const s2string &entry_point)  = 0;

	virtual ShaderBytecode * GetShaderBytecode() = 0;

	s2string GetError() { return error; }

protected:
	s2string error;
};

}

#endif		//VERTEX_SHADER_H_
