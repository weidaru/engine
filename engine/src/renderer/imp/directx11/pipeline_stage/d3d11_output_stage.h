#ifndef D3D11_OUTPUT_STAGE_H_
#define D3D11_OUTPUT_STAGE_H_

#include <stdint.h>

namespace s2 {

class D3D11Texture2D;
class Resource;
class Texture2D;
class D3D11GraphicResourceManager;


/**
 * TODO: Use D3D11 type directly instead of more general ones.
 */
class D3D11OutputStage {
public:
	D3D11OutputStage(D3D11GraphicResourceManager *_manager);
	~D3D11OutputStage();
	
	void SetRenderTarget(unsigned int index, D3D11Texture2D *target);
	Resource * GetRenderTarget(unsigned int index);
	void SetDepthStencilBuffer(D3D11Texture2D *buffer);
	Resource * GetDepthStencilBuffer();
	
	void Reset();
	void ResetRenderTargets();
	void ResetDepthStencilBuffer();
	
	void ClearRenderTarget(Texture2D *texture, const float rgba[4]);
	void ClearDepthStencilBuffer(Texture2D *buffer, bool clear_depth, float depth, bool clear_stencil, int stencil);
	
	void Setup();
	
private:
	void SetOutput();
	
private:
	D3D11GraphicResourceManager *manager;

	bool new_output;
	std::vector<D3D11Texture2D *> rts;
	D3D11Texture2D *ds;
	
};


}


#endif		//D3D11_OUTPUT_STAGE_H_
