#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#include "d3d11_vertex_buffer.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_graphic_resource_manager.h"
#include "d3d11_buffer_helper.h"

namespace s2 {

D3D11VertexBuffer::D3D11VertexBuffer(D3D11GraphicResourceManager *_manager)
		: manager(_manager),vb(0){
	Clear();
}

D3D11VertexBuffer::~D3D11VertexBuffer() {
	Clear();
}

void D3D11VertexBuffer::Clear() {
	if(vb) {
		vb->Release();
		vb=0;
	}
	ele_count = 0;
	ele_member_count = 0;
	ele_bytewidth = 0;
}

void D3D11VertexBuffer::Initialize(unsigned int element_count, unsigned int element_member_count,
													unsigned int per_ele_size, const void *data, GeneralEnum::CPUAccess _cpu_access) {
	Clear();
	CHECK(element_count>0 && per_ele_size>0)<<"element count and element bytewidth must not be 0";
	
	ele_count = element_count;
	ele_member_count = element_member_count;
	ele_bytewidth = per_ele_size;
	cpu_access = _cpu_access;
	
	D3D11_BUFFER_DESC desc;
	D3D11BufferHelper::SetBufferDesc(&desc, per_ele_size*element_count, cpu_access);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	
	HRESULT result = 1;
	if(data) {
		D3D11_SUBRESOURCE_DATA subresource;
		subresource.pSysMem = data;
		result = manager->GetDevice()->CreateBuffer(&desc, &subresource, &vb);
	} else {
		result = manager->GetDevice()->CreateBuffer(&desc, 0, &vb);
	}
	
	CHECK(!FAILED(result))<<"Cannot create vertex buffer. Error code: " <<::GetLastError();
}

void D3D11VertexBuffer::Initialize(unsigned int element_count, const TypeInfo &type_info, const void *data, GeneralEnum::CPUAccess _cpu_access) {
	type_name = type_info.GetName();
	this->Initialize(element_count, type_info.GetMemberSize(), type_info.GetSize(), data, _cpu_access);
}

GeneralEnum::CPUAccess GetCPUAccessFlag() const {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	return cpu_access;
}

unsigned int D3D11VertexBuffer::GetElementCount() const {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	return ele_count;
}

unsigned int D3D11VertexBuffer::GetElementBytewidth() const {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	return ele_bytewidth;
}

unsigned int D3D11VertexBuffer::GetElementMemberCount() const {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	return ele_member_count;
}

void * D3D11VertexBuffer::Map() {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	CHECK(IsDynamic())<<"Vertex buffer must be dynamic in order to map.";
	D3D11_MAPPED_SUBRESOURCE subresource;
	HRESULT result = 1;
	result = manager->GetDeviceContext()->Map(vb, 1, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	CHECK(!FAILED(result))<<"Fail to map vertex buffer. Error code "<<::GetLastError();
	
	return subresource.pData;
}


void D3D11VertexBuffer::UnMap() {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	CHECK(IsDynamic())<<"Vertex buffer mut be dynamic in order to unmap.";
	manager->GetDeviceContext()->Unmap(vb, 0);
}

}







