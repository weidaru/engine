#ifndef D3D11_PIXEL_SHADER_H_
#define D3D11_PIXEL_SHADER_H_

#include "renderer/pixel_shader.h"

#include <vector>

struct ID3D11PixelShader;

namespace s2 {

class D3D11GraphicResourceManager;
class D3D11ConstantBuffer;
class D3D11Texture2D;

class D3D11PixelShader : public PixelShader {
private:
	struct RenderTarget {
		D3D11Texture2D *tex;		//Only allow Texture2D for now.
		bool enable_clear;
		float rgba[4];
		
		RenderTarget() {
			tex=0;
			enable_clear=false;
		}
	};
	
	struct DepthStencil {
		D3D11Texture2D *tex;		//Only allow Texture2D for now.
		bool enable_clear;
		float depth;
		uint8_t stencil;
		
		DepthStencil(){
			tex=0;
			enable_clear=false;
		}
	};
	
public:
	D3D11PixelShader(D3D11GraphicResourceManager *_manager);
	virtual ~D3D11PixelShader();
	virtual bool Initialize(const s2string &path);
	virtual bool SetConstantBuffer(const s2string &name, ConstantBuffer *cb);
	virtual ConstantBuffer * GetConstantBuffer(const s2string &name);
	virtual bool SetSampler(const s2string &name, Sampler *sampler);
	virtual Sampler * GetSampler(const s2string &name);
	virtual bool SetResource(const s2string &name, Texture1D *resource);
	virtual bool SetResource(const s2string &name, Texture2D *resource);
	virtual bool SetResource(const s2string &name, Texture3D *resource);
	virtual Resource * GetResource(const s2string &name);
	
	virtual bool SetRenderTarget(unsigned int index, Texture2D *target);
	virtual Resource * GetRenderTarget(unsigned int index);
	virtual bool SetRenderTargetClearOption(unsigned int index, bool enable, const float rgba[4]);
	virtual bool GetRenderTargetClearOption(unsigned int index, bool *enable, float *rgba);
	virtual bool SetDepthStencilBuffer(Texture2D *buffer);
	virtual Resource * GetDepthStencilBuffer();
	virtual void SetDepthStencilBufferClearOption(bool enable,  float depth, uint8_t stencil);
	virtual void GetDepthStencilBufferClearOption(bool *enable,  float *depth, uint8_t *stencil);
	
	virtual void GetLastError(s2string *str);
	
	/*****************D3D11 exclusive.******************/
	void Flush();
	
private:
	void Check();
	void Clear();
	
private:
	D3D11GraphicResourceManager *manager;
	std::vector<D3D11ConstantBuffer *> cbs;
	std::vector<RenderTarget> rts;
	DepthStencil ds;
	
	ID3D11PixelShader *shader;
	s2string error;
	
};


}


#endif		//D3D11_PIXEL_SHADER_H_