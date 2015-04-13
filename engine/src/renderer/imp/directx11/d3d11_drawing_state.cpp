#include "d3d11_drawing_state.h"
#include "d3d11_input_stage.h"

namespace s2 {

D3D11DrawingState::D3D11DrawingState() : layout(0) {

}

D3D11DrawingState::~D3D11DrawingState() {
	delete layout;
}

const D3D11DrawingState & D3D11DrawingState::SetInputLayout(D3D11InputLayout *input_layout) {
	this->layout = input_layout;
	return *this;
}

D3D11InputLayout * D3D11DrawingState::GetInputLayout() {
	return layout;
}

}