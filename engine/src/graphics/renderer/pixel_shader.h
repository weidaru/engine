#ifndef PIXEL_SHADER_H_
#define PIXEL_SHADER_H_

#include "resource.h"
#include "utils/s2string.h"
#include "utils/type_info.h"

namespace s2 {
class Sampler;
class ShaderResource;

class PixelShader : public Resource {
public:
	virtual ~PixelShader() {}
	virtual bool Initialize(const ShaderBytecode &bytecode) = 0;
	virtual ShaderData *GetShaderData() = 0;

	s2string GetError() { return error; }

protected:
	s2string error;
};

}

#endif		//PIXEL_SHADER_H_
