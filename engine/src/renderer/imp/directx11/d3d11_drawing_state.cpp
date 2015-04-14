#pragma comment(lib, "d3d11.lib")

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include "d3d11_drawing_state.h"
#include "d3d11_input_stage.h"

namespace s2 {

D3D11DrawingState::D3D11DrawingState() : layout(0), stream_out_gs(0) {

}

D3D11DrawingState::~D3D11DrawingState() {
	delete layout;
	if (stream_out_gs) {
		stream_out_gs->Release();
	}
	
}

const D3D11DrawingState & D3D11DrawingState::SetInputLayout(D3D11InputLayout *input_layout) {
	if (input_layout != this->layout) {
		delete this->layout;
	}
	this->layout = input_layout;
	return *this;
}

D3D11InputLayout * D3D11DrawingState::GetInputLayout() {
	return layout;
}

const D3D11DrawingState & D3D11DrawingState::SetStreamOutGeometryShader(ID3D11GeometryShader *gs) {
	if (stream_out_gs && gs != stream_out_gs) {
		stream_out_gs->Release();
	}
	stream_out_gs = gs;
	return *this;
}

ID3D11GeometryShader * D3D11DrawingState::GetStreamOutGeometryShader() {
	return stream_out_gs;
}
}