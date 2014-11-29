#ifndef D3D11_OUTPUT_STAGE_H_
#define D3D11_OUTPUT_STAGE_H_

#include <stdint.h>

namespace s2 {

class D3D11Texture2D;
class Resource;
class Texture2D;
class D3D11GraphicResourceManager;

class D3D11OutputStage {
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
		bool enable_depth_clear;
		bool enable_stencil_clear;
		float depth;
		uint8_t stencil;
		
		DepthStencil(){
			tex=0;
			enable_depth_clear = false;
			enable_stencil_clear = false;
		}
	};

public:
	D3D11OutputStage(D3D11GraphicResourceManager *_manager);
	~D3D11OutputStage();
	
	void SetRenderTarget(unsigned int index, Texture2D *target);
	Resource * GetRenderTarget(unsigned int index);
	void SetRenderTargetClearOption(unsigned int index, bool enable, const float rgba[4]);
	void GetRenderTargetClearOption(unsigned int index, bool *enable, float *rgba) const;
	void SetDepthStencilBuffer(Texture2D *buffer);
	Resource * GetDepthStencilBuffer();
	void SetDepthStencilBufferClearOption(bool enable_depth_clear, bool enable_stencil_clear,  float depth, uint8_t stencil);
	void GetDepthStencilBufferClearOption(bool *enable_depth_clear, bool *enable_stencil_clear,  float *depth, uint8_t *stencil) const;
	
	void Clear();
	
	void ClearRenderTargets();
	void Setup();
	
private:
	void SetOutput();
	
private:
	D3D11GraphicResourceManager *manager;

	bool new_output;
	std::vector<RenderTarget> rts;
	DepthStencil ds;
	
};


}


#endif		//D3D11_OUTPUT_STAGE_H_
