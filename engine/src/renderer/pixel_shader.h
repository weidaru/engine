#ifndef PIXEL_SHADER_H_
#define PIXEL_SHADER_H_

#include "resource.h"
#include "utils/s2string.h"
#include "utils/type_info.h"

namespace s2 {
class ConstantBuffer;
class Sampler;
class DepthStencilBuffer;
class Texture1D;
class Texture2D;
class Texture3D;

class PixelShader : public Resource {
public:
	virtual 								~PixelShader() {}
	virtual bool 						Initialize(const s2string &path, const s2string &entry_point) = 0;
	
	template <typename T>
	bool SetUniform(const s2string &name, const T &value) {
		SetUniform(name, TypeInfoManager::GetSingleton()->Get<T>().GetName(),&value);
	}
	virtual bool SetUniform(const s2string &name, const void * value, unsigned int size) = 0;
	
	virtual bool 						SetSampler(const s2string &name, Sampler *sampler) = 0;
	virtual Sampler * 				GetSampler(const s2string &name) = 0;
	virtual bool 						SetTexture2D(const s2string &name, Texture2D *resource) = 0;
	virtual Texture2D *			GetTexture2D(const s2string &name) = 0;

	virtual const s2string &		GetLastError() = 0;
	
protected:
	virtual bool 						SetUniform(const s2string &name, const TypeInfo &cpp_type, const void *value) = 0;
};

}

#endif		//PIXEL_SHADER_H_
