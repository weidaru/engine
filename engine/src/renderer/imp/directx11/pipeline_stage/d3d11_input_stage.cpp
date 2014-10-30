#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_input_stage.h"
#include "renderer/imp/directx11/d3d11_vertex_buffer.h"
#include "renderer/imp/directx11/d3d11_index_buffer.h"


D3D11InputStage::D3D11InputStage(D3D11GraphicResourceManager *_manager)
			: 	manager(_manager), {
	Clear();
}

D3D11InputStage::~D3D11InputStage() {
	Clear();
}

D3D11InputStage::Clear() {
	new_input = true;
	ib = 0;
	vbs.clear();
	vbs.resize(D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
	topology = GraphicPipeline::TRIANGLE_LIST;
}

void D3D11InputStage::SetPrimitiveTopology(GraphicPipeline::PrimitiveTopology newvalue) {
	new_input = true;
	topology = newvalue;
}

GraphicPipeline::PrimitiveTopology D3D11InputStage::GetPrimitiveTopology() {
	return topology;
}

void D3D11InputStage::SetVertexBuffer(unsigned int index, unsigned int start_input_index, VertexBuffer *buf, VertexBufferUsage usage) {
	new_input = true;
	D3D11VertexBuffer *buf = NiceCast(D3D11VertexBuffer *, _buf);
	int i = 0;
	vbs[i].start_index = start_input_index;
	vbs[i].vb = buf;
	vbs[i].usage = usage;
}

D3D11VertexBuffer * D3D11InputStage::GetVertexBuffer(unsigned int index, unsigned int *start_input_index, VertexBufferUsage *usage) {
	CHECK(false)<<"Disable for now";
	return 0;
}

void D3D11InputStage::SetIndexBuffer(IndexBuffer *_buf) {
	new_input = true;
	D3D11IndexBuffer *buf = NiceCast(D3D11IndexBuffer *, _buf);
	ib = buf;
}

D3D11IndexBuffer * D3D11InputStage::GetIndexBuffer() {
	return ib;
}

void D3D11InputStage::SetInput() {
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	//Set primitive topology
	context->IASetPrimitiveTopology(D3D11EnumConverter::TopologyToD3D11Topology(topology));
	
	//Populate input layout.
	SetInputLayout();
	
	//Set vertex buffer.
	{
		int last_index = -1;
		for(int i=vbs.size()-1; i>=0; i--) {
			if(vbs[i].vb) {
				last_index = i;
				break;
			}
		}
		if(last_index != -1) {
			ID3D11Buffer **buf_array = new ID3D11Buffer *[last_index+1];
			unsigned int *stride_array = new unsigned int[last_index+1];
			unsigned int *offset_array = new unsigned int[last_index+1];
			for(int i=0; i<=last_index; i++) {
				buf_array[i] = vbs[i].vb->GetInternal();
				stride_array[i] = 4;			//TODO: This information needs to retrive from type_name, use 4 for now.
				offset_array[i] = 0;
			}
			context->IASetVertexBuffers(0, last_index+1, buf_array, stride_array, offset_array);
			delete[] stride_array;
			delete[] offset_array;
		}
	}
	
	//Set index buffer
	if(ib) {
		context->IASetIndexBuffer(ib->GetInternal(), DXGI_FORMAT_R32_UINT, 0);
	}
}

D3D11InputStage::Flush() {
	if(new_input) {
		SetInputLayout();
		SetInput();
	}
}


/*
typedef struct D3D11_INPUT_ELEMENT_DESC {
	LPCSTR                   SemanticName;						//From vertex shader reflection
	UINT                       SemanticIndex;						//From vertex shader reflection
	DXGI_FORMAT         Format;									//From vertex shader reflection
	UINT                       InputSlot;								//From VBInfo
	UINT                       AlignedByteOffset;					//From vertex shader reflection
	D3D11_INPUT_CLASSIFICATION InputSlotClass;		//From VBInfo
	UINT                       InstanceDataStepRate;			//From VBInfo
} D3D11_INPUT_ELEMENT_DESC;
*/
void D3D11InputStage::SetInputLayout() {
	
	
}


