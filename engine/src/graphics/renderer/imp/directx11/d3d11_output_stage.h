#ifndef D3D11_OUTPUT_STAGE_H_
#define D3D11_OUTPUT_STAGE_H_

#include <stdint.h>
#include "utils/s2string.h"

#include <stack>

struct ID3D11DeviceContext;

namespace s2 {

class D3D11Texture2D;
class D3D11GraphicResourceManager;
class D3D11GraphicPipeline;
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
	typedef std::vector<std::pair<uint32_t, Resource * > > RTBindingVector;

private:
	struct  RTInfo {
		D3D11RenderTarget *data;
		
		RTInfo() {
			data = 0;
		}
	};
	struct  DSInfo {
		D3D11DepthStencil *data;

		DSInfo() {
			data = 0;
		}
	};
	struct  SOInfo {
		D3D11StreamOut *data;
		uint32_t stream_index;

		SOInfo() {
			data = 0;
			stream_index = 0;
		}
	};

private:
	struct State {
		std::vector<RTInfo > rts;
		DSInfo ds;
		std::vector<SOInfo> stream_outs;
		int rasterized_stream;
	};

public:
	D3D11OutputStage(D3D11GraphicResourceManager *_manager, ID3D11DeviceContext *_context);
	~D3D11OutputStage();
	
	void SetRenderTarget(uint32_t index, RenderTarget *target);
	void SetRenderTarget(uint32_t start_index, const std::vector<RenderTarget *> &rts);
	D3D11RenderTarget * GetRenderTarget(uint32_t index);
	uint32_t GetRenderTargetCapacity() const;

	void SetDepthStencil(DepthStencil *buffer);
	D3D11DepthStencil * GetDepthStencil();

	void SetStreamOut(uint32_t index, uint32_t stream_index, StreamOut *stream_out);
	void SetStreamOut(uint32_t start_index, const std::vector<std::tuple<uint32_t, StreamOut *>> &stream_outs);
	D3D11StreamOut * GetStreamOut(uint32_t index, uint32_t *stream_index);

	void SetRasterizedStream(int index);
	int GetRasterizedStream();

	void ClearRenderTarget(RenderTarget *rt, const float rgba[4]);
	void ClearDepthStencil(DepthStencil *ds, bool clear_depth, float depth, bool clear_stencil, int stencil);
	
	void PushState();
	void PopState();
	void ClearSavedState();
	
	void Sync(ID3D11DeviceContext *context);

private:
	void SyncRenderTargetsAndDepthStencil(ID3D11DeviceContext *context);
	void SyncStreamOuts(ID3D11DeviceContext *context);
	
private:
	D3D11GraphicResourceManager *manager;
	ID3D11DeviceContext *context;

	std::vector<RTInfo > rts;
	DSInfo ds;

	std::vector<SOInfo> stream_outs;
	int rasterized_stream;

	std::stack<State> saved_states;
};


}


#endif		//D3D11_OUTPUT_STAGE_H_
