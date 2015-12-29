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
#include "d3d11_resource_view.h"

#ifdef NDEBUG
#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif


namespace s2 {


D3D11InputStage::D3D11InputStage(D3D11GraphicResourceManager *_manager, ID3D11DeviceContext *_context)
	: manager(_manager), context(_context), input_layout(0){
	SetPrimitiveTopology(GraphicPipeline::TRIANGLE_LIST);
	vbs.resize(D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
}

D3D11InputStage::~D3D11InputStage() {
}

void D3D11InputStage::SetPrimitiveTopology(GraphicPipeline::PrimitiveTopology newvalue) {
	topology = newvalue;
	if(context) {
		context->IASetPrimitiveTopology(D3D11EnumConverter::TopologyToD3D11Topology(topology));
	}
	
}

GraphicPipeline::PrimitiveTopology D3D11InputStage::GetPrimitiveTopology() {
	return topology;
}

void D3D11InputStage::SetInputLayout(InputLayout *_layout) {
	input_layout = static_cast<D3D11InputLayout *>(_layout);
	if(context) {
		if(input_layout) {
			context->IASetInputLayout(input_layout->GetInternal());
		} else {
			context->IASetInputLayout(0);
		}
	}
}

D3D11InputLayout *D3D11InputStage::GetInputLayout() {
	return input_layout;
}

void D3D11InputStage::SetVertexBuffer(uint32_t index, VertexBuffer *buf) {
	SetVertexBuffer(index, buf, 0, 0);
}

void D3D11InputStage::SetVertexBuffer(uint32_t start_index, const std::vector<VertexBuffer *> &_input){
	std::vector<VertexBufferBinding> input(_input.size());
	for(uint32_t i=0; i<_input.size(); i++) {
		input.push_back({_input[i], 0, 0});
	}
	SetVertexBuffer(start_index, input);
}

void D3D11InputStage::SetVertexBuffer(uint32_t index, VertexBuffer *_buf, uint32_t stride, uint32_t offset){
	D3D11VertexBuffer *buffer = NiceCast(D3D11VertexBuffer *, _buf);
	VBInfo &info = vbs[index];
	info.vb = buffer;
	if(stride == 0 && buffer != 0) {
		info.stride = buffer->GetResource()->GetElementBytewidth() ;
	} else {
		info.stride = stride;
	}
	info.offset = offset;

	if(context) {
		//Set vertex buffer.
		ID3D11Buffer *buffer_raw = buffer ? buffer->GetBuffer() : 0;
		context->IASetVertexBuffers(index, 1, &buffer_raw, &info.stride, &info.offset);
	}
}

void D3D11InputStage::SetVertexBuffer(uint32_t start_index, 
			const std::vector<VertexBufferBinding> &input){
	for(uint32_t i=0; i<input.size(); i++) {
		const VertexBufferBinding &item = input[i];
		VBInfo &info = vbs[start_index+i];
		info.vb = static_cast<D3D11VertexBuffer *>(item.buffer);
		if(item.stride == 0 && item.buffer != 0) {
			info.stride = info.vb->GetResource()->GetElementBytewidth();
		} else {
			info.stride = item.stride;
		}
		info.offset = item.offset;
	}
	
	if(context) {
		ID3D11Buffer **buffer_array  = new ID3D11Buffer*[input.size()];
		uint32_t *strides = new uint32_t[input.size()];
		uint32_t *offsets = new uint32_t[input.size()];

		for(uint32_t i=0; i<input.size(); i++) {
			VBInfo &info = vbs[start_index+i];

			buffer_array[i] = info.vb ?  info.vb->GetBuffer() : 0;
			offsets[i] = info.offset;
			strides[i] = info.stride;
		}
		context->IASetVertexBuffers(start_index, input.size(), buffer_array, strides, offsets);

		delete[] buffer_array;
		delete[] strides;
		delete[] offsets;
	}
}

D3D11VertexBuffer * D3D11InputStage::GetVertexBuffer(uint32_t index, uint32_t *stride, uint32_t *offset) {
	VBInfo &info = vbs[index];
	if(stride) {
		*stride = info.stride;
	}
	if(offset) {
		*offset = info.offset;
	}

	return info.vb;
}

void D3D11InputStage::SetIndexBuffer(IndexBuffer *_buf, uint32_t vertex_base) {
	D3D11IndexBuffer *buffer = NiceCast(D3D11IndexBuffer *, _buf);

	ib.vertex_base = vertex_base;
	ib.buffer = buffer;
	//Set index buffer
	if(context) {
		ID3D11Buffer *buffer_raw = buffer ? buffer->GetBuffer() : 0;
		context->IASetIndexBuffer(buffer_raw, DXGI_FORMAT_R32_UINT, 0);
	}
}

D3D11IndexBuffer * D3D11InputStage::GetIndexBuffer(uint32_t *vertex_base) {
	*vertex_base = ib.vertex_base;
	return ib.buffer;
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

	topology = state.topology;
	input_layout = state.input_layout;
	ib = state.ib;
	vbs = state.vbs;

	Sync(context);

	saved_states.pop();
}


void D3D11InputStage::Sync(ID3D11DeviceContext *context) {
	if(context == 0) {
		return;
	}

	//Sync PrimitiveTopology
	context->IASetPrimitiveTopology(D3D11EnumConverter::TopologyToD3D11Topology(topology));
	
	//Sync InputLayout
	if(input_layout) {
		context->IASetInputLayout(input_layout->GetInternal());
	} else {
		context->IASetInputLayout(0);
	}

	//Sync IndexBuffer
	context->IASetIndexBuffer(ib.buffer == 0 ? 0 : ib.buffer->GetBuffer(), DXGI_FORMAT_R32_UINT, 0);

	//Sync VertexBuffer
	ID3D11Buffer *buffer_raw[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
	uint32_t strides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
	uint32_t offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

	for(unsigned int i=0; i<vbs.size(); i++) {
		VBInfo &info = vbs[i];
		buffer_raw[i] = info.vb ? info.vb->GetBuffer() : 0;
		strides[i] = info.stride;
		offsets[i] = info.offset;
	}

	context->IASetVertexBuffers(0, vbs.size(), buffer_raw, strides, offsets);
}

void D3D11InputStage::ClearSavedState() {
	while(saved_states.empty() == false) {
		saved_states.pop();
	}
}

}