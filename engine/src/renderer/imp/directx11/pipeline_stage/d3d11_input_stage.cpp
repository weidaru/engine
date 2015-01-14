#pragma comment(lib, "d3d11.lib")


#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>
#include <algorithm>

#include "d3d11_input_stage.h"
#include "renderer/imp/directx11/d3d11_vertex_buffer.h"
#include "renderer/imp/directx11/d3d11_index_buffer.h"
#include "renderer/imp/directx11/d3d11_vertex_shader.h"
#include "renderer/imp/directx11/d3d11_shader_reflection.h"
#include "renderer/imp/directx11/d3d11_graphic_resource_manager.h"
#include "renderer/imp/directx11/d3d11_enum_converter.h"

#ifdef NDEBUG
	#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
	#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif


namespace s2 {

D3D11InputStage::D3D11InputStage(D3D11GraphicResourceManager *_manager)
			: 	manager(_manager){
	SetPrimitiveTopology(GraphicPipeline::TRIANGLE_LIST);
	new_input = true;
	old_shader = 0;
	ib = 0;
	vbs.resize(D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
	input_layout = 0;
	first_instance_count  = 0;
}

D3D11InputStage::~D3D11InputStage() {
	if(input_layout) {
		input_layout->Release();
	}
	input_layout = 0;
}

void D3D11InputStage::SetPrimitiveTopology(GraphicPipeline::PrimitiveTopology newvalue) {
	new_input = true;
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
	new_input = true;
	vbs[index].start_index = start_input_index;
	vbs[index].vb = buffer;
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

	new_input = true;
	ib = buffer;
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
			strides[i] = vbs[i].vb->GetElementBytewidth();
			offsets[i] = 0;
			buffers[i] = vb->GetInternal();
		} else {
			strides[i] = 0;
			offsets[i] = 0;
			buffers[i] = 0;
		}
	}
	context->IASetVertexBuffers(0, vbs.size(), buffers, strides, offsets);
	
	//Set index buffer
	if(ib) {
		context->IASetIndexBuffer(ib->GetInternal(), DXGI_FORMAT_R32_UINT, 0);
	} else {
		context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);
	}
}

void D3D11InputStage::Setup(const D3D11VertexShader *shader) {
	if(new_input)  {
		SetInput();
	}
	if(shader != old_shader || new_input) {
		SetInputLayout(shader);
	}
	old_shader  = shader;
	new_input = false;
}

void D3D11InputStage::Flush(unsigned int vertex_count, unsigned int instance_count) {
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	if(vertex_count == 0) {
		if(ib!=0) {
			vertex_count = ib->GetElementCount();
		} else {
			for(unsigned int i=0; i<vbs.size(); i++) {
				if(vbs[i].vb) {
					vertex_count=vbs[i].vb->GetElementCount();
					break;
				}
			}
		}

	}
		
	if(instance_count == 0) {
		instance_count = first_instance_count;
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
			unsigned int end = start+infos[i].vb->GetElementMemberCount()-1;

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
void D3D11InputStage::SetInputLayout(const D3D11VertexShader *shader) {
	if(input_layout) {
		input_layout->Release();
		input_layout = 0;
		first_instance_count = 0;
	}
	
	if(shader == 0) {
		ID3D11DeviceContext *context = manager->GetDeviceContext();
		context->IASetInputLayout(0);
		return;
	}
	
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
		if(it->vb)
			pool.push_back(it);
	}
	
	std::sort(pool.begin(), pool.end(), VBCompare);
	
	unsigned int head = 0;
	for(std::vector<std::vector<VBInfo>::iterator>::iterator it=pool.begin(); it != pool.end(); it++) {
		const VBInfo &vbinfo = **it;
		if(head < vbinfo.start_index) {
			CHECK(false)<<"Shader input "<<head<<" is not covered by vertex buffer. Dumping:\n"<<DumpVertexBufferInfo(vbs);
		} else if(head > vbinfo.start_index) {
			CHECK(false)<<"Shader input "<<head<<" is covered by multiple vertex buffer. Dumping:\n"<<DumpVertexBufferInfo(vbs);
		} else {
			head = vbinfo.start_index + vbinfo.vb->GetElementMemberCount();
		}
	}
	if(head < size) {
		CHECK(false)<<"Some shader tail inputs are not covered by vertex buffer. Dumping:\n"<<DumpVertexBufferInfo(vbs)<<"Last input index is "<<size-1;
	} else if(head > size) {
		CHECK(false)<<"Vertex buffer overflows input. Dumping:\n"<<DumpVertexBufferInfo(vbs)<<"Last input index is "<<size-1;
	}
	
	std::vector<std::vector<VBInfo>::iterator>::iterator it=pool.begin();
	unsigned int offset = 0;
	for(unsigned int i=0; i<size; i++) {
		if((**it).start_index + (**it).vb->GetElementMemberCount()-1 < i) {
			it++;
			offset = 0;
		}
		const D3D11ShaderReflection::Parameter &p = reflect.GetInput(i);
		const VBInfo &vbinfo = **it;
		CHECK(i<=(vbinfo.start_index+vbinfo.vb->GetElementMemberCount()));
		descs[i].InputSlot = *it - vbs.begin();
		descs[i].AlignedByteOffset = offset;
		offset += p.size;
	}
	
	//Find the first input parameter bound as D3D11_INPUT_PER_INSTANCE_DATA and 
	//remember its corresponding vertex buffer element count
	for(unsigned int i=0; i<size; i++) {
		if(descs[i].InputSlotClass == D3D11_INPUT_PER_INSTANCE_DATA) {
			first_instance_count = vbs[descs[i].InputSlot].vb->GetElementCount();
			
			break;
		}
	}
	
	ID3D11Device *device = manager->GetDevice();
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	HRESULT result = 1;
	result = device->CreateInputLayout(descs, size, shader->GetBlob()->GetBufferPointer(), shader->GetBlob()->GetBufferSize(), &input_layout);
	CHECK(!FAILED(result))<<"Fail to create input layout error "<<::GetLastError();
	context->IASetInputLayout(input_layout);
	delete descs;
}


}