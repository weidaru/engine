#ifndef SHADER_BYTECODE_H_
#define SHADER_BYTECODE_H_

#include "resource.h"

namespace s2 {

class ShaderBytecode : public Resource {
public:
	virtual ~ShaderBytecode() {}

	virtual bool Initialize(const s2string &path, const s2string &entry_point)  = 0;

	s2string GetLastError() { return error; }

protected:
	s2string error;
};

}

#endif // !SHADER_H_
