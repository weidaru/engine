#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

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
			: 	manager(_manager), input_layout(0) {
	Clear();
}

D3D11InputStage::~D3D11InputStage() {
	Clear();
}

void D3D11InputStage::Clear() {
	new_input = true;
	ib = 0;
	vbs.clear();
	vbs.resize(D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
	topology = GraphicPipeline::TRIANGLE_LIST;
	if(input_layout)
		input_layout->Release();
}

void D3D11InputStage::SetPrimitiveTopology(GraphicPipeline::PrimitiveTopology newvalue) {
	new_input = true;
	topology = newvalue;
}

GraphicPipeline::PrimitiveTopology D3D11InputStage::GetPrimitiveTopology() {
	return topology;
}

void D3D11InputStage::SetVertexBuffer(unsigned int index, unsigned int start_input_index, VertexBuffer *_buf) {
	new_input = true;
	D3D11VertexBuffer *buf = NiceCast(D3D11VertexBuffer *, _buf);
	vbs[index].start_index = start_input_index;
	vbs[index].vb = buf;
}

D3D11VertexBuffer * D3D11InputStage::GetVertexBuffer(unsigned int index, unsigned int *start_input_index) {
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
				stride_array[i] = vbs[i].vb->GetElementBytewidth();
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

void D3D11InputStage::Setup(const D3D11VertexShader *shader) {
	if(new_input) 
		SetInput();
	new_input = false;
	if(shader)
		SetInputLayout(*shader);
}

void D3D11InputStage::Flush() {
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	if(ib) {
		context->DrawIndexed(ib->GetElementCount(), 0, 0);
	} else {
		unsigned int index = -1;
		for(unsigned int i=0; i<vbs.size(); i++) {
			if(vbs[i].vb) {
				index = i;
				break;
			}
		}
		if(index != -1)
			context->Draw(vbs[index].vb->GetElementCount(), 0);	
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

s2string D3D11InputStage::DumpVertexBufferInfo(const std::vector<VBInfo> infos) {
	char buffer[1024*8];
	char *head = buffer;
	for(unsigned int i=0; i<infos.size(); i++) {
		if(infos[i].vb)
			head += sprintf_s(head, 1024*8, "VertexBuffer %d, start at input %d, ends at input %d.\n", 
											i, 
											infos[i].start_index, 
											infos[i].start_index+infos[i].vb->GetElementCount()-1);
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
void D3D11InputStage::SetInputLayout(const D3D11VertexShader &shader) {
	const D3D11ShaderReflection &reflect = shader.GetReflection();
	if(input_layout) {
		input_layout->Release();
		input_layout = 0;
	}
		
	unsigned int size = reflect.GetInputSize();
	D3D11_INPUT_ELEMENT_DESC *descs = new D3D11_INPUT_ELEMENT_DESC[size];
	
	for(unsigned int i=0; i<size; i++) {
		const D3D11ShaderReflection::Parameter &p = reflect.GetInput(i);
		descs[i].SemanticName = p.semantic.c_str();
		descs[i].SemanticIndex = p.semantic_index;
		descs[i].Format = GetFormat(p);
		//All the instance semantics starts with Instance_
		if(p.semantic.substr(0, 9) == "Instance_") {
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
		CHECK(false)<<"Some inputs are not covered by vertex buffer. Dumping:\n"<<DumpVertexBufferInfo(vbs);
	} else if(head > size) {
		CHECK(false)<<"Vertex buffer overflows input. Dumping:\n"<<DumpVertexBufferInfo(vbs);
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
	
	ID3D11Device *device = manager->GetDevice();
	ID3D11DeviceContext *context = manager->GetDeviceContext();
	HRESULT result = 1;
	result = device->CreateInputLayout(descs, size, shader.GetBlob()->GetBufferPointer(), shader.GetBlob()->GetBufferSize(), &input_layout);
	CHECK(!FAILED(result))<<"Fail to create input layout error "<<::GetLastError();
	context->IASetInputLayout(input_layout);
	delete descs;
}


}