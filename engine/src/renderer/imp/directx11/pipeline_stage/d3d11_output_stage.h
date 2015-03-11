#ifndef D3D11_OUTPUT_STAGE_H_
#define D3D11_OUTPUT_STAGE_H_

#include <stdint.h>

namespace s2 {

class D3D11Texture2D;
class Resource;
class D3D11GraphicResourceManager;
class D3D11RenderTarget;
class D3D11DepthStencil;
class RenderTarget;
class DepthStencil;

class D3D11OutputStage {
public:
	typedef std::vector<std::pair<unsigned int, Resource * > > RTBindingVector;

private:
	struct  RTInfo {
		D3D11RenderTarget *render_target;
		bool is_new;
		
		RTInfo() {
			render_target = 0;
			is_new = false;
		}
	};
	
	struct  DSInfo {
		D3D11DepthStencil *depth_stencil;
		bool is_new;
		
		DSInfo() {
			depth_stencil = 0;
			is_new = false;
		}
	};

public:
	D3D11OutputStage(D3D11GraphicResourceManager *_manager);
	~D3D11OutputStage();
	
	void SetRenderTarget(unsigned int index, RenderTarget *target);
	D3D11RenderTarget * GetRenderTarget(unsigned int index);
	unsigned int GetRenderTargetCapacity() const;
	void SetDepthStencil(DepthStencil *buffer);
	D3D11DepthStencil * GetDepthStencil();

	void ClearRenderTarget(RenderTarget *rt, const float rgba[4]);
	void ClearDepthStencil(DepthStencil *ds, bool clear_depth, float depth, bool clear_stencil, int stencil);
	
	void Setup();
	void Refresh();
	
	/**
	 * New render target bindings since last draw call.
	 */
	RTBindingVector GetNewRenderTargetBindings() const;
	
private:
	void SetOutput();
	
private:
	D3D11GraphicResourceManager *manager;

	std::vector<RTInfo> rts;
	DSInfo ds;
};


}


#endif		//D3D11_OUTPUT_STAGE_H_
