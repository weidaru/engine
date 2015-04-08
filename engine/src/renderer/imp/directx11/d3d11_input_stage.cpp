#pragma comment(lib, "d3d11.lib")


#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>
#include <algorithm>

#include "d3d11_input_stage.h"
#include "d3d11_buffer.h"
#include "d3d11_vertex_shader.h"
#include "d3d11_shader_reflection.h"
#include "d3d11_graphic_resource_manager.h"
#include "d3d11_enum_converter.h"

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

bool InputStateCompare::operator()(const InputState &lhs, const InputState &rhs) const {
	if (lhs.vs != rhs.vs) {
		return lhs.vs < rhs.vs;
	}
	if (lhs.vb_state.size() != rhs.vb_state.size()) {
		return lhs.vb_state.size() < rhs.vb_state.size();
	}
	for (unsigned int i = 0; i < lhs.vb_state.size(); i++) {
		const VBBinding &lhs_binding = lhs.vb_state[i];
		const VBBinding &rhs_binding = rhs.vb_state[i];
		if (lhs_binding.vb != rhs_binding.vb) {
			return lhs_binding.vb < rhs_binding.vb;
		}
		if (lhs_binding.index != rhs_binding.index) {
			return lhs_binding.index < rhs_binding.index;
		}
		if (lhs_binding.start_index != rhs_binding.start_index) {
			return lhs_binding.start_index < rhs_binding.start_index;
		}
	}
	return false;
}

D3D11InputLayoutManager::~D3D11InputLayoutManager() {
	for (auto it = input_layouts.begin(); it != input_layouts.end(); it++) {
		delete it->second;
	}
}

void D3D11InputLayoutManager::Put(const InputState &state, D3D11InputLayout *new_layout) {
	input_layouts.insert(std::make_pair(state, new_layout));
}

D3D11InputLayout * D3D11InputLayoutManager::Get(const InputState &state) {
	if (input_layouts.find(state) == input_layouts.end()) {
		return 0;
	} else {
		return input_layouts[state];
	}
}



D3D11InputStage::D3D11InputStage(D3D11GraphicResourceManager *_manager)
			: 	manager(_manager){
	SetPrimitiveTopology(GraphicPipeline::TRIANGLE_LIST);
	ib = 0;
	vbs.resize(D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
}

D3D11InputStage::~D3D11InputStage() {
}

void D3D11InputStage::SetPrimitiveTopology(GraphicPipeline::PrimitiveTopology newvalue) {
	topology = newvalue;
}

GraphicPipeline::PrimitiveTopology D3D11InputStage::GetPrimitiveTopology() {
	return topology;
}

void D3D11InputStage::SetVertexBuffer(unsigned int index, unsigned int start_input_index, VertexBuffer *_buf) {
	D3D11VertexBuffer *buffer = NiceCast(D3D11VertexBuffer *, _buf);
	if(_buf) {
		CHECK(buffer)<<"Cannot cast buf to D3D11VertexBuffer";
	}
	
	if (vbs[index].start_index != start_input_index || vbs[index].vb != buffer) {
		vbs[index].start_index = start_input_index;
		vbs[index].vb = buffer;
	}
}

D3D11VertexBuffer * D3D11InputStage::GetVertexBuffer(unsigned int index, unsigned int *start_input_index) {
	*start_input_index = vbs[index].start_index;
	return vbs[index].vb;
}

void D3D11InputStage::SetIndexBuffer(IndexBuffer *_buf) {
	D3D11IndexBuffer *buffer = NiceCast(D3D11IndexBuffer *, _buf);
	if(_buf) {
		CHECK(buffer)<<"Cannot cast buf to D3D11IndexBuffer";
	}

	if (ib != buffer) {
		ib = buffer;
	}
}

D3D11IndexBuffer * D3D11InputStage::GetIndexBuffer() {
	return ib;
}

void D3D11InputStage::SetInput() {
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	//Set primitive topology
	context->IASetPrimitiveTopology(D3D11EnumConverter::TopologyToD3D11Topology(topology));
	
	//Set vertex buffer.
	ID3D11Buffer **buffers = new ID3D11Buffer *[vbs.size()];
	unsigned int *strides = new unsigned int[vbs.size()];
	unsigned int *offsets = new unsigned int[vbs.size()];
	for(unsigned i=0; i<vbs.size(); i++) {
		D3D11VertexBuffer *vb = vbs[i].vb;
		if(vb) {
			strides[i] = vbs[i].vb->GetResource()->GetElementBytewidth();
			offsets[i] = 0;
			buffers[i] = vb->GetBuffer();
		} else {
			strides[i] = 0;
			offsets[i] = 0;
			buffers[i] = 0;
		}
	}
	context->IASetVertexBuffers(0, vbs.size(), buffers, strides, offsets);
	
	//Set index buffer
	if(ib) {
		context->IASetIndexBuffer(ib->GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
	} else {
		context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);
	}
}

void D3D11InputStage::Setup(const D3D11VertexShader *shader) {
	ID3D11DeviceContext *context = manager->GetDeviceContext();

	SetInput();

	if (shader != 0) {
		InputState state;
		BuildState(shader, &state);
		D3D11InputLayout *input_layout = input_layout_manager.Get(state);
		if (input_layout == 0) {
			int first_instance_count = -1;
			input_layout = CreateInputLayout(shader);
			input_layout_manager.Put(state, input_layout);
		}
		
		context->IASetInputLayout(input_layout->layout);
		current_first_instance_count = input_layout->first_instance_count;
	}
	else {
		context->IASetInputLayout(0);
		current_first_instance_count = 0;
	}
}

void D3D11InputStage::BuildState(const D3D11VertexShader *shader, InputState *state) const {
	state->vs = shader;
	for (auto it = vbs.begin(); it != vbs.end(); it++) {
		if (it->vb != 0 && it->start_index >= 0) {
			VBBinding binding;
			binding.index = it-vbs.begin();
			binding.start_index = it->start_index;
			binding.vb = it->vb;
			state->vb_state.push_back(binding);
		}
	}
}

void D3D11InputStage::Flush(unsigned int vertex_count, unsigned int instance_count) {
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	if(vertex_count == 0) {
		if(ib!=0) {
			vertex_count = ib->GetResource()->GetElementCount();
		} else {
			for(unsigned int i=0; i<vbs.size(); i++) {
				if(vbs[i].vb) {
					vertex_count=vbs[i].vb->GetResource()->GetElementCount();
					break;
				}
			}
		}
	}
		
	if(instance_count == 0) {
		instance_count = current_first_instance_count;
	}

	if(vertex_count !=0 ) {
		if(ib!=0) {
			if(instance_count == 0) {
				context->DrawIndexed(vertex_count, 0, 0);
			} else {
				context->DrawIndexedInstanced(vertex_count, instance_count,0,0,0);
			}
		} else{
			if(instance_count == 0) {
				context->Draw(vertex_count, 0);
			} else {
				context->DrawInstanced(vertex_count, instance_count, 0, 0);
			}
		}
	}
}

bool D3D11InputStage::Validate(const D3D11VertexShader &shader, s2string *message) const {
	CHECK(false)<<"Disabled";
	return true;
}

namespace {

//Exception for POSITION semantic 
DXGI_FORMAT GetFormat(const D3D11ShaderReflection::Parameter &p) {
	static DXGI_FORMAT float_list[4] = {	DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, 
															DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT};
	static DXGI_FORMAT int_list[4] = {	DXGI_FORMAT_R32_SINT, DXGI_FORMAT_R32G32_SINT, 
															DXGI_FORMAT_R32G32B32_SINT, DXGI_FORMAT_R32G32B32A32_SINT};
	static DXGI_FORMAT uint_list[4] = {	DXGI_FORMAT_R32_UINT, DXGI_FORMAT_R32G32_UINT, 
															DXGI_FORMAT_R32G32B32_UINT, DXGI_FORMAT_R32G32B32A32_UINT};
	if(p.type_name == "float")
		return float_list[p.size/4-1];
	if(p.type_name == "int")
		return int_list[p.size/4-1];
	if(p.type_name == "uint")
		return uint_list[p.size/4-1];
	CHECK(false)<<"Unexpected parameter typename "<<p.type_name;
	return DXGI_FORMAT_R32_FLOAT;
}

}

bool D3D11InputStage::VBCompare(const std::vector<VBInfo>::iterator lhs, const std::vector<VBInfo>::iterator rhs) {
	return lhs->start_index < rhs->start_index; 
}

s2string D3D11InputStage::DumpVertexBufferInfo(const std::vector<VBInfo> &infos) {
	char buffer[1024*8];
	char *head = buffer;
	for(unsigned int i=0; i<infos.size(); i++) {
		if(infos[i].vb) {
			unsigned int start = infos[i].start_index;
			unsigned int end = start+infos[i].vb->GetResource()->GetElementMemberCount()-1;

			head += sprintf_s(head, 1024*8-(head-buffer), "VertexBuffer %d, start at input %d, ends at input %d.\n", 
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
		
	unsigned int size = reflect.GetInputSize();
	D3D11_INPUT_ELEMENT_DESC *descs = new D3D11_INPUT_ELEMENT_DESC[size];
	
	for(unsigned int i=0; i<size; i++) {
		const D3D11ShaderReflection::Parameter &p = reflect.GetInput(i);
		descs[i].SemanticName = p.semantic.c_str();
		descs[i].SemanticIndex = p.semantic_index;
		descs[i].Format = GetFormat(p);
		//All the instance semantics starts with Instance_
		if(p.semantic.substr(0, 9) == "INSTANCE_") {
			descs[i].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			descs[i].InstanceDataStepRate = 1; 			//Make this 1 which means 1 data for 1 instance.
		} else {
			descs[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			descs[i].InstanceDataStepRate = 0; 
		}
	}
	
	std::vector<std::vector<VBInfo>::iterator> pool;
	for(std::vector<VBInfo>::iterator it=vbs.begin(); it!=vbs.end(); it++) {
		if(it->vb && it->start_index>=0)
			pool.push_back(it);
	}
	
	std::sort(pool.begin(), pool.end(), VBCompare);
	
	unsigned int head = 0;
	for(std::vector<std::vector<VBInfo>::iterator>::iterator it=pool.begin(); it != pool.end(); it++) {
		const VBInfo &vbinfo = **it;
		if(head < (unsigned int)vbinfo.start_index) {
			LOG(FATAL)<<"Shader input "<<head<<" is not covered by vertex buffer. Dumping:\n"<<DumpVertexBufferInfo(vbs);
		} else if(head > (unsigned int)vbinfo.start_index) {
			LOG(FATAL) << "Shader input " << head << " is covered by multiple vertex buffer. Dumping:\n" << DumpVertexBufferInfo(vbs);
		} else {
			head = vbinfo.start_index + vbinfo.vb->GetResource()->GetElementMemberCount();
		}
	}
	if(head < size) {
		LOG(FATAL) << "Some shader tail inputs are not covered by vertex buffer. Dumping:\n" << DumpVertexBufferInfo(vbs);
	} else if(head > size) {
		LOG(FATAL) << "Vertex buffer overflows input. Dumping:\n" << DumpVertexBufferInfo(vbs);
	}
	
	std::vector<std::vector<VBInfo>::iterator>::iterator it=pool.begin();
	unsigned int offset = 0;
	for(unsigned int i=0; i<size; i++) {
		if((**it).start_index + (**it).vb->GetResource()->GetElementMemberCount()-1 < i) {
			it++;
			offset = 0;
		}
		const D3D11ShaderReflection::Parameter &p = reflect.GetInput(i);
		const VBInfo &vbinfo = **it;
		CHECK(i<=(vbinfo.start_index+vbinfo.vb->GetResource()->GetElementMemberCount()));
		descs[i].InputSlot = *it - vbs.begin();
		descs[i].AlignedByteOffset = offset;
		offset += p.size;
	}
	
	//Find the first input parameter bound as D3D11_INPUT_PER_INSTANCE_DATA and 
	//remember its corresponding vertex buffer element count
	for(unsigned int i=0; i<size; i++) {
		if(descs[i].InputSlotClass == D3D11_INPUT_PER_INSTANCE_DATA) {
			input_layout->first_instance_count = vbs[descs[i].InputSlot].vb->GetResource()->GetElementCount();
			
			break;
		}
	}
	
	ID3D11Device *device = manager->GetDevice();
	HRESULT result = 1;
	result = device->CreateInputLayout(descs, size, shader->GetBlob()->GetBufferPointer(), shader->GetBlob()->GetBufferSize(), &(input_layout->layout));
	CHECK(!FAILED(result))<<"Fail to create input layout error "<<::GetLastError();
	delete descs;

	return input_layout;
}

void D3D11InputStage::Refresh() {
	//Set all the start_index to be -1 so that it will needs to be refreshed inorder to create new input layout.
	for (auto it = vbs.begin(); it != vbs.end(); it++) {
		if (it->vb && it->start_index >= 0) {
			it->start_index = -1;
		}
	}
}
}