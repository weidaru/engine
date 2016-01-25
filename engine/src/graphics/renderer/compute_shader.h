#ifndef COMPUTE_SHADER_H_
#define	COMPUTE_SHADER_H_

#include "resource.h"

namespace s2 {

class ShaderBytecode;

class ComputeShader : public Resource {
public:
	virtual ~ComputeShader() {}
	virtual bool Initialize(ShaderBytecode *bytecode) = 0;
	virtual bool Initialize(const s2string &path, const s2string &entry_point)  = 0;

	virtual const ShaderBytecode * GetShaderBytecode() const = 0;
	virtual ShaderBytecode * GetShaderBytecode() = 0;
};


}



#endif // !COMPUTE_SHADER_H_



