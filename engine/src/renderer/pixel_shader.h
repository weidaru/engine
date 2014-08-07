#ifndef PIXEL_SHADER_H_
#define PIXEL_SHADER_H_

#include "resource.h"
#include "utils/s2string.h"

namespace s2 {
class ShaderReflection;
class ConstantBuffer;
class Sampler;
class DepthStencilBuffer;

class PixelShader : public Resource {
public:
	virtual 							~PixelShader() {}
	virtual void 						SetConstantBuffer(const s2string &name, const ConstantBuffer &cb) = 0;
	virtual ConstantBuffer * 			GetConstantBuffer(const s2string &name) = 0;
	virtual void 						SetSampler(const s2string &name, const Sampler &sampler) = 0;
	virtual Sampler * 					GetSampler(const s2string &name) = 0;
	virtual void 						SetResource(const s2string &name, const Resource &resource) = 0;
	virtual Resource * 					GetResource(const s2string &name) = 0;
	
	virtual void 						SetRenderTarget(const s2string &name, Texture2D *target) = 0;
	virtual Texture2D *					GetRenderTarget(const s2string &name) = 0;
	virtual void 						SetDepthStencilBuffer(DepthStencilBuffer *depth_stencil) = 0;
	virtual DepthStencilBuffer *		GetDepthStencilBuffer() = 0;
};

}

#endif		//PIXEL_SHADER_H_
