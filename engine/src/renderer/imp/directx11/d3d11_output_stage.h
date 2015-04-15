#ifndef D3D11_OUTPUT_STAGE_H_
#define D3D11_OUTPUT_STAGE_H_

#include <stdint.h>
#include "utils/s2string.h"

struct ID3D11GeometryShader;

namespace s2 {

class D3D11Texture2D;
class Resource;
class D3D11GraphicResourceManager;
class D3D11RenderTarget;
class D3D11DepthStencil;
class D3D11StreamOut;
class D3D11GeometryShader;
class RenderTarget;
class DepthStencil;
class StreamOut;
class D3D11DrawingState;

class D3D11OutputStage {
public:
	typedef std::vector<std::pair<unsigned int, Resource * > > RTBindingVector;

private:
	struct  RTInfo {
		D3D11RenderTarget *data;
		bool is_new;
		
		RTInfo() {
			data = 0;
			is_new = false;
		}
	};
	struct  DSInfo {
		D3D11DepthStencil *data;
		bool is_new;

		DSInfo() {
			data = 0;
			is_new = false;
		}
	};
	struct  SOInfo {
		D3D11StreamOut *data;
		int stream_index;

		SOInfo() {
			data = 0;
			stream_index = -1;
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

	void SetStreamOut(unsigned int index, unsigned int stream_index, StreamOut *stream_out);
	D3D11StreamOut * GetStreamOut(unsigned int index, unsigned int *stream_index);

	void SetRasterizedStream(int index);
	int GetRasterizedStream();

	void ClearRenderTarget(RenderTarget *rt, const float rgba[4]);
	void ClearDepthStencil(DepthStencil *ds, bool clear_depth, float depth, bool clear_stencil, int stencil);

	ID3D11GeometryShader * Setup(D3D11GeometryShader *gs);
	ID3D11GeometryShader * Setup(D3D11GeometryShader *gs, D3D11DrawingState *draw_state);
	void Refresh();
	
	/**
	 * New render target bindings since last draw call.
	 */
	RTBindingVector GetNewRenderTargetBindings() const;
	
private:
	void SetOutput();
	ID3D11GeometryShader * D3D11OutputStage::BuildStreamOutGeometryShader(D3D11GeometryShader *gs);
	s2string DumpStreamOutInfo(const std::vector<SOInfo> &infos);
	
private:
	D3D11GraphicResourceManager *manager;

	std::vector<RTInfo > rts;
	DSInfo ds;

	std::vector<SOInfo> stream_outs;
	int rasterized_stream;
};


}


#endif		//D3D11_OUTPUT_STAGE_H_
