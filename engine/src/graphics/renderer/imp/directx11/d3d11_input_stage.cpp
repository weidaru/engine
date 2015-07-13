#pragma comment(lib, "d3d11.lib")


#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>
#include <algorithm>

#include "d3d11_input_stage.h"
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

D3D11InputLayout::~D3D11InputLayout() {
	if (layout) {
		layout->Release();
	}
}


D3D11InputStage::D3D11InputStage(D3D11GraphicResourceManager *_manager, D3D11GraphicPipeline *_pipeline)
	: manager(_manager), pipeline(_pipeline), owned_layout(0){
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

void D3D11InputStage::SetVertexBuffer(uint32_t index, uint32_t start_input_index, VertexBuffer *_buf) {
	D3D11VertexBuffer *buffer = NiceCast(D3D11VertexBuffer *, _buf);
	if (_buf) {
		CHECK(buffer) << "Cannot cast buf to D3D11VertexBuffer";
	}
	vbs[index].is_new = true;
	vbs[index].start_index = start_input_index;

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

D3D11VertexBuffer * D3D11InputStage::GetVertexBuffer(uint32_t index, uint32_t *start_input_index) {
	if (start_input_index) {
		*start_input_index = vbs[index].start_index;
	}
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
	ID3D11DeviceContext *context = pipeline->GetDeviceContext();

	if (shader != 0) {
		D3D11InputLayout *input_layout = 0;
		input_layout = CreateInputLayout(shader);

		context->IASetInputLayout(input_layout->layout);
		current_first_instance_count = input_layout->first_instance_count;
		if (owned_layout) {
			owned_layout->Release();
		}
		owned_layout = input_layout->layout;
	}
	else {
		context->IASetInputLayout(0);
		current_first_instance_count = 0;
	}
}

void D3D11InputStage::Setup(const D3D11VertexShader *shader, D3D11DrawingState *draw_state) {
	CHECK_NOTNULL(draw_state);
	ID3D11DeviceContext *context = pipeline->GetDeviceContext();

	if (shader != 0) {
		D3D11InputLayout *input_layout = 0;
		if (draw_state->GetInputLayout()) {
			input_layout = draw_state->GetInputLayout();
		}
		else {
			input_layout = CreateInputLayout(shader);
		}

		context->IASetInputLayout(input_layout->layout);
		current_first_instance_count = input_layout->first_instance_count;

		draw_state->SetInputLayout(input_layout);
	}
	else {
		context->IASetInputLayout(0);
		current_first_instance_count = 0;
	}
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

	if(instance_count == 0) {
		instance_count = current_first_instance_count;
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

namespace {

	//Exception for POSITION semantic 
	DXGI_FORMAT GetFormat(const D3D11ShaderReflection::Parameter &p) {
		static DXGI_FORMAT float_list[4] = { DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT,
			DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT };
		static DXGI_FORMAT int_list[4] = { DXGI_FORMAT_R32_SINT, DXGI_FORMAT_R32G32_SINT,
			DXGI_FORMAT_R32G32B32_SINT, DXGI_FORMAT_R32G32B32A32_SINT };
		static DXGI_FORMAT uint_list[4] = { DXGI_FORMAT_R32_UINT, DXGI_FORMAT_R32G32_UINT,
			DXGI_FORMAT_R32G32B32_UINT, DXGI_FORMAT_R32G32B32A32_UINT };
		if (p.type_name == "float")
			return float_list[p.size / 4 - 1];
		if (p.type_name == "int")
			return int_list[p.size / 4 - 1];
		if (p.type_name == "uint")
			return uint_list[p.size / 4 - 1];
		CHECK(false) << "Unexpected parameter typename " << p.type_name;
		return DXGI_FORMAT_R32_FLOAT;
	}

}

bool D3D11InputStage::VBCompare(const std::vector<VBInfo>::iterator lhs, const std::vector<VBInfo>::iterator rhs) {
	return lhs->start_index < rhs->start_index;
}

s2string D3D11InputStage::DumpVertexBufferInfo(const std::vector<VBInfo> &infos) {
	char buffer[1024 * 8];
	char *head = buffer;
	for (uint32_t i = 0; i<infos.size(); i++) {
		if (infos[i].vb) {
			uint32_t start = infos[i].start_index;
			uint32_t end = start + infos[i].vb->GetResource()->GetElementMemberCount() - 1;

			head += sprintf_s(head, 1024 * 8 - (head - buffer), "VertexBuffer %d, start at input %d, ends at input %d.\n",
				i, start, end);
		}

	}
	return s2string(buffer);
}


/*
typedef struct D3D11_INPUT_ELEMENT_DESC {
LPCSTR                   SemanticName;						//From vertex shader reflection
UINT                       SemanticIndex;						//From vertex shader reflection
DXGI_FORMAT         Format;									//From vertex shader reflection
UINT                       InputSlot;								//From VBInfo
UINT                       AlignedByteOffset;					//From VBInfo and vertex shader reflection
D3D11_INPUT_CLASSIFICATION InputSlotClass;		//From vertex shader reflection
UINT                       InstanceDataStepRate;			//From VBInfo
} D3D11_INPUT_ELEMENT_DESC;
*/
D3D11InputLayout * D3D11InputStage::CreateInputLayout(const D3D11VertexShader *shader) {
	D3D11InputLayout *input_layout = new D3D11InputLayout;

	const D3D11ShaderReflection &reflect = shader->GetReflection();

	uint32_t size = reflect.GetInputSize();
	D3D11_INPUT_ELEMENT_DESC *descs = new D3D11_INPUT_ELEMENT_DESC[size];

	for (uint32_t i = 0; i<size; i++) {
		const D3D11ShaderReflection::Parameter &p = reflect.GetInput(i);
		descs[i].SemanticName = p.semantic.c_str();
		descs[i].SemanticIndex = p.semantic_index;
		descs[i].Format = GetFormat(p);
		//All the instance semantics starts with Instance_
		if (p.semantic.substr(0, 9) == "INSTANCE_") {
			descs[i].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			descs[i].InstanceDataStepRate = 1; 			//Make this 1 which means 1 data for 1 instance.
		}
		else {
			descs[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			descs[i].InstanceDataStepRate = 0;
		}
	}

	std::vector<std::vector<VBInfo>::iterator> pool;
	for (std::vector<VBInfo>::iterator it = vbs.begin(); it != vbs.end(); it++) {
		if (it->vb && it->is_new)
			pool.push_back(it);
	}

	std::sort(pool.begin(), pool.end(), VBCompare);

	//Number of register it covers.
	uint32_t head = 0;
	for (std::vector<std::vector<VBInfo>::iterator>::iterator it = pool.begin(); it != pool.end(); it++) {
		const VBInfo &vbinfo = **it;
		if (head < (uint32_t)vbinfo.start_index) {
			LOG(FATAL) << "Shader input " << head << " is not covered by vertex buffer. Dumping:\n" << DumpVertexBufferInfo(vbs);
		}
		else if (head >(uint32_t)vbinfo.start_index) {
			LOG(FATAL) << "Shader input " << head << " is covered by multiple vertex buffer. Dumping:\n" << DumpVertexBufferInfo(vbs);
		}
		else {
			D3D11GraphicBuffer *buffer = vbinfo.vb->GetResource() ;
			uint32_t byte_width = buffer->GetElementBytewidth();
			uint32_t offset = 0;
			while(offset < byte_width) {
				const D3D11ShaderReflection::Parameter &p = reflect.GetInput(head);
				
				descs[head].InputSlot = *it - vbs.begin();
				descs[head].AlignedByteOffset = offset;
				offset += p.size;
				head++;
				if (head > size) {
					LOG(FATAL) << "Vertex buffer overflows input. Dumping:\n" << DumpVertexBufferInfo(vbs);
				}
			}
		}
	}


	//Find the first input parameter bound as D3D11_INPUT_PER_INSTANCE_DATA and 
	//remember its corresponding vertex buffer element count
	for (uint32_t i = 0; i<size; i++) {
		if (descs[i].InputSlotClass == D3D11_INPUT_PER_INSTANCE_DATA) {
			input_layout->first_instance_count = vbs[descs[i].InputSlot].vb->GetResource()->GetElementCount();

			break;
		}
	}

	ID3D11Device *device = manager->GetDevice();
	HRESULT result = 1;
	result = device->CreateInputLayout(descs, size, shader->GetBlob()->GetBufferPointer(), shader->GetBlob()->GetBufferSize(), &(input_layout->layout));
	CHECK(!FAILED(result)) << "Fail to create input layout error " << ::GetLastError();
	delete descs;

	return input_layout;
}

void D3D11InputStage::PushState() {
	saved_states.push(State());
	State &new_state = saved_states.top();

	new_state.ib = ib;
	new_state.vbs = vbs;
	new_state.topology = topology;
}

void D3D11InputStage::PopState() {
	State &state = saved_states.top();
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