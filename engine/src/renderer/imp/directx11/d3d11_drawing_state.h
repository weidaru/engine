#ifndef D3D11_DRAW_STATE_H_
#define D3D11_DRAW_STATE_H_

#include "renderer/drawing_state.h"


namespace s2 {

struct D3D11InputLayout;

class D3D11DrawingState : public DrawingState {
public:
	D3D11DrawingState();

	virtual ~D3D11DrawingState();

	const D3D11DrawingState & SetInputLayout(D3D11InputLayout *input_layout);
	D3D11InputLayout * GetInputLayout();


private:
	virtual void Dummy() {}

private:
	D3D11InputLayout *layout;
};

}


#endif				//D3D11_DRAW_STATE_H_