#ifndef PIXEL_SHADER_H_
#define PIXEL_SHADER_H_

#include "resource.h"
#include <string>

namespace s2 {
class ShaderReflection;
class ConstantBuffer;
class Sampler;

class PixelShader {
public:
	virtual 							~PixelShader() {}
	virtual void 						SetConstantBuffer(unsigned int index, const ConstantBuffer &cb) = 0;
	virtual ConstantBuffer * 			GetConstantBuffer(unsigned int index) = 0;
	virtual void 						SetConstantBuffer(const std::string &name, const ConstantBuffer &cb) = 0;
	virtual ConstantBuffer * 			GetConstantBuffer(const std::string &name) = 0;
	virtual void 						SetSampler(unsigned int index, const Sampler &sampler) = 0;
	virtual Sampler * 					GetSampler(unsigned int index) = 0;
	virtual void 						SetSampler(const std::string &name, const Sampler &sampler) = 0;
	virtual Sampler * 					GetSampler(const std::string &name) = 0;
	virtual void 						SetResource(unsigned int index, const Resource &resource) = 0;
	virtual Resource * 					GetResource(unsigned int index) = 0;
	virtual void 						SetResource(const std::string &name, const Resource &resource) = 0;
	virtual Resource * 					GetResource(const std::string &name) = 0;
	
	virtual const ShaderReflection & 	GetReflection() = 0;
};

}

#endif		//PIXEL_SHADER_H_
