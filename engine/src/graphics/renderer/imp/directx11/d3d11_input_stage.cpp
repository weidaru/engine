#pragma comment(lib, "d3d11.lib")


#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>
#include <algorithm>

#include "d3d11_input_stage.h"

#include "d3d11_input_layout.h"
#include "d3d11_graphic_buffer.h"
#include "d3d11_vertex_shader.h"
#include "d3d11_shader_reflection.h"
#include "d3d11_graphic_resource_manager.h"
#include "d3d11_graphic_pipeline.h"
#include "d3d11_enum_converter.h"
#include "d3d11_drawing_state.h"

#ifdef NDEBUG
#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif


namespace s2 {


D3D11InputStage::D3D11InputStage(D3D11GraphicResourceManager *_manager, D3D11GraphicPipeline *_pipeline)
	: manager(_manager), pipeline(_pipeline), input_layout(0){
	SetPrimitiveTopology(GraphicPipeline::TRIANGLE_LIST);
	vbs.resize(D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
}

D3D11InputStage::~D3D11InputStage() {
}

void D3D11InputStage::SetPrimitiveTopology(GraphicPipeline::PrimitiveTopology newvalue) {
	topology = newvalue;
	ID3D11DeviceContext *context = pipeline->GetDeviceContext();
	context->IASetPrimitiveTopology(D3D11EnumConverter::TopologyToD3D11Topology(topology));
}

GraphicPipeline::PrimitiveTopology D3D11InputStage::GetPrimitiveTopology() {
	return topology;
}

void D3D11InputStage::SetInputLayout(InputLayout *_layout) {
	input_layout = static_cast<D3D11InputLayout *>(_layout);
	ID3D11DeviceContext *context = pipeline->GetDeviceContext();
	if(input_layout) {
		context->IASetInputLayout(input_layout->GetInternal());
	} else {
		context->IASetInputLayout(0);
	}
	
}

D3D11InputLayout *D3D11InputStage::GetInputLayout() {
	return input_layout;
}

void D3D11InputStage::SetVertexBuffer(uint32_t index, VertexBuffer *_buf) {
	D3D11VertexBuffer *buffer = NiceCast(D3D11VertexBuffer *, _buf);
	if (_buf) {
		CHECK(buffer) << "Cannot cast buf to D3D11VertexBuffer";
	}
	vbs[index].is_new = true;

	if(vbs[index].vb != buffer) {
		vbs[index].vb = buffer;

		ID3D11DeviceContext *context = pipeline->GetDeviceContext();
		//Set vertex buffer.
		ID3D11Buffer *buffer_raw = buffer ? buffer->GetBuffer() : 0;
		uint32_t strides = buffer ? buffer->GetResource()->GetElementBytewidth() : 0;
		uint32_t offsets = 0;
		context->IASetVertexBuffers(index, 1, &buffer_raw, &strides, &offsets);
	}

}

D3D11VertexBuffer * D3D11InputStage::GetVertexBuffer(uint32_t index) {
	return vbs[index].vb;
}

void D3D11InputStage::SetIndexBuffer(IndexBuffer *_buf, uint32_t vertex_base) {
	D3D11IndexBuffer *buffer = NiceCast(D3D11IndexBuffer *, _buf);
	if (_buf) {
		CHECK(buffer) << "Cannot cast buf to D3D11IndexBuffer";
	}

	ib.is_new = true;
	ib.vertex_base = vertex_base;

	if(ib.buffer != buffer) {
		ib.buffer = buffer;
		//Set index buffer
		ID3D11DeviceContext *context = pipeline->GetDeviceContext();
		ID3D11Buffer *buffer_raw = buffer ? buffer->GetBuffer() : 0;
		context->IASetIndexBuffer(buffer_raw, DXGI_FORMAT_R32_UINT, 0);
	}
}

D3D11IndexBuffer * D3D11InputStage::GetIndexBuffer(uint32_t *vertex_base) {
	*vertex_base = ib.vertex_base;
	return ib.buffer;
}

void D3D11InputStage::Setup(const D3D11VertexShader *shader) {
	//TODO: Remove after pipeline clean up.
}

void D3D11InputStage::Setup(const D3D11VertexShader *shader, D3D11DrawingState *draw_state) {
	//TODO: Remove after pipeline clean up.
}

void D3D11InputStage::Flush(uint32_t start_index, uint32_t vertex_count) {
	ID3D11DeviceContext *context = pipeline->GetDeviceContext();
	
	if (vertex_count == 0) {
		if (ib.is_new && ib.buffer) {
			vertex_count = ib.buffer->GetResource()->GetElementCount() - start_index;
		}
		else {
			for (uint32_t i = 0; i<vbs.size(); i++) {
				if (vbs[i].vb) {
					vertex_count = vbs[i].vb->GetResource()->GetElementCount() - start_index;
					break;
				}
			}
		}
	}

	
	if(ib.is_new && ib.buffer) {
		context->DrawIndexed(vertex_count, start_index, ib.vertex_base);
	} else {
		context->Draw(vertex_count, start_index);
	}
}

void D3D11InputStage::FlushWithInstancing(uint32_t vertex_start, uint32_t vertex_count, uint32_t instance_start, uint32_t instance_count) {
	if (vertex_count == 0) {
		if (ib.is_new && ib.buffer) {
			vertex_count = ib.buffer->GetResource()->GetElementCount() - vertex_start;
		}
		else {
			for (uint32_t i = 0; i<vbs.size(); i++) {
				if (vbs[i].vb) {
					vertex_count = vbs[i].vb->GetResource()->GetElementCount() - vertex_start;
					break;
				}
			}
		}
	}

	ID3D11DeviceContext *context = pipeline->GetDeviceContext();
	if(ib.is_new && ib.buffer) {
		context->DrawIndexedInstanced(vertex_count, instance_count, vertex_start, ib.vertex_base, instance_start);
	} else {
		context->DrawInstanced(vertex_count, instance_count, vertex_start, instance_start);
	}
}

bool D3D11InputStage::Validate(const D3D11VertexShader &shader, s2string *message) const {
	CHECK(false) << "Disabled";
	return true;
}

void D3D11InputStage::PushState() {
	saved_states.push(State());
	State &new_state = saved_states.top();

	new_state.ib = ib;
	new_state.vbs = vbs;
	new_state.topology = topology;
	new_state.input_layout = input_layout;
}

void D3D11InputStage::PopState() {
	State &state = saved_states.top();
	SetInputLayout(state.input_layout);
	ib = state.ib;
	vbs = state.vbs;
	SetPrimitiveTopology(state.topology);
	saved_states.pop();

	ID3D11DeviceContext *context = pipeline->GetDeviceContext();
	//sync index buffer
	{
		ID3D11Buffer *buffer_raw = ib.buffer ? ib.buffer->GetBuffer() : 0;
		context->IASetIndexBuffer(buffer_raw, DXGI_FORMAT_R32_UINT, 0);
	}

	//sync vertex buffer
	{
		ID3D11Buffer **buffer_raw = new ID3D11Buffer*[vbs.size()];
		uint32_t *strides = new uint32_t[vbs.size()];
		uint32_t *offsets = new uint32_t[vbs.size()];

		for(unsigned int i=0; i<vbs.size(); i++) {
			VBInfo &info = vbs[i];
			buffer_raw[i] = info.vb ? info.vb->GetBuffer() : 0;
			strides[i] = info.vb ? info.vb->GetResource()->GetElementBytewidth() : 0;
			offsets[i] = 0;
		}

		context->IASetVertexBuffers(0, 1, buffer_raw, strides, offsets);
	}
}

void D3D11InputStage::ClearSavedState() {
	while(saved_states.empty() == false) {
		saved_states.pop();
	}
}

void D3D11InputStage::Refresh() {
	//Set all the start_index to be -1 so that it will needs to be refreshed inorder to create new input layout.
	for (auto it = vbs.begin(); it != vbs.end(); it++) {
		it->is_new = false;
	}
	ib.is_new = false;
}
}