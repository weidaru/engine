#ifndef D3D11_DRAW_STATE_H_
#define D3D11_DRAW_STATE_H_

#include "graphics/renderer/drawing_state.h"

struct ID3D11GeometryShader;

namespace s2 {

struct D3D11InputLayout;

class D3D11DrawingState : public DrawingState {
public:
	D3D11DrawingState();

	virtual ~D3D11DrawingState();

	const D3D11DrawingState & SetInputLayout(D3D11InputLayout *input_layout);
	D3D11InputLayout * GetInputLayout();

	const D3D11DrawingState & SetStreamOutGeometryShader(ID3D11GeometryShader *gs);
	ID3D11GeometryShader * GetStreamOutGeometryShader();
	

private:
	virtual void Dummy() {}

private:
	D3D11InputLayout *layout;
	ID3D11GeometryShader *stream_out_gs;
};

}


#endif				//D3D11_DRAW_STATE_H_