#ifndef PIXEL_SHADER_H_
#define PIXEL_SHADER_H_

#include "resource.h"
#include "utils/s2string.h"
#include "utils/type_info.h"

namespace s2 {
class ShaderBytecode;

class PixelShader : public Resource {
public:
	virtual ~PixelShader() {}
	virtual bool Initialize(ShaderBytecode *bytecode) = 0;
	virtual bool Initialize(const s2string &path, const s2string &entry_point)  = 0;

	virtual ShaderBytecode *GetShaderData() = 0;

	s2string GetError() { return error; }

protected:
	s2string error;
};

}

#endif		//PIXEL_SHADER_H_
