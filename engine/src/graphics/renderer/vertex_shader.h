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
	virtual bool Initialize(const ShaderBytecode &bytecode) = 0;
	virtual ShaderData *GetShaderData() = 0;

	s2string GetError() { return error; }

protected:
	s2string error;
};

}

#endif		//VERTEX_SHADER_H_
