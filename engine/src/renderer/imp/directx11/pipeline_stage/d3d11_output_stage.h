#ifndef D3D11_OUTPUT_STAGE_H_
#define D3D11_OUTPUT_STAGE_H_

#include <stdint.h>

struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView; 

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
	typedef std::vector<std::pair<unsigned int, Resource * > > RTBindingVector;

private:
	struct  RTInfo {
		Resource *resource;
		ID3D11RenderTargetView *view;
		bool is_new;
		
		RTInfo() {
			resource = 0;
			view = 0;
			is_new = false;
		}
	};
	
	struct  DSInfo {
		Resource *resource;
		ID3D11DepthStencilView *view;
		bool is_new;
		
		DSInfo() {
			resource = 0;
			view = 0;
			is_new = false;
		}
	};

public:
	D3D11OutputStage(D3D11GraphicResourceManager *_manager);
	~D3D11OutputStage();
	
	void SetRenderTarget(unsigned int index, Texture2D *target);
	Resource * GetRenderTarget(unsigned int index);
	unsigned int GetRenderTargetCapacity() const;
	void SetDepthStencilBuffer(Texture2D *buffer);
	Resource * GetDepthStencilBuffer();

	void ClearRenderTarget(Texture2D *texture, const float rgba[4]);
	void ClearDepthStencilBuffer(Texture2D *buffer, bool clear_depth, float depth, bool clear_stencil, int stencil);
	
	void Setup();
	
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
