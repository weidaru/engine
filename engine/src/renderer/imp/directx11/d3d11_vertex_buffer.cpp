#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#include "d3d11_vertex_buffer.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_graphic_resource_manager.h"
#include "d3d11_resource_helper.h"
#include "d3d11_mapped_resource.h"

namespace s2 {

D3D11VertexBuffer::D3D11VertexBuffer(D3D11GraphicResourceManager *_manager)
		: manager(_manager),vb(0),mapped(0){
	Clear();
}

D3D11VertexBuffer::~D3D11VertexBuffer() {
	Clear();
}

void D3D11VertexBuffer::Clear() {
	delete mapped;
	mapped = 0;

	ele_count = 0;
	ele_member_count = 0;
	ele_bytewidth = 0;
	
	if(vb) {
		vb->Release();
		vb=0;
	}
}

void D3D11VertexBuffer::Initialize(unsigned int element_count, unsigned int element_member_count,
													unsigned int per_ele_size, const void *data, GeneralEnum::MapBehavior map_behavior) {
	Clear();
	CHECK(element_count>0 && per_ele_size>0)<<"element count and element bytewidth must not be 0";
	
	ele_count = element_count;
	ele_member_count = element_member_count;
	ele_bytewidth = per_ele_size;
	map_behavior = map_behavior;
	
	D3D11_BUFFER_DESC desc;
	D3D11ResourceHelper::SetBufferDesc(&desc, per_ele_size*element_count, map_behavior);
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
	
	mapped = new D3D11MappedResource(manager, vb, map_behavior);
}

void D3D11VertexBuffer::Initialize(unsigned int element_count, const TypeInfo &type_info, const void *data, GeneralEnum::MapBehavior _map_behavior) {
	type_name = type_info.GetName();
	this->Initialize(element_count, type_info.GetMemberSize(), type_info.GetSize(), data, _map_behavior);
}

unsigned int D3D11VertexBuffer::GetElementCount() const {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	return ele_count;
}

unsigned int D3D11VertexBuffer::GetElementBytewidth() const {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	return ele_bytewidth;
}

GeneralEnum::MapBehavior D3D11VertexBuffer::GetMapBehavior() const {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	return mapped->GetMapBehavior();
}

unsigned int D3D11VertexBuffer::GetElementMemberCount() const {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	return ele_member_count;
}

void D3D11VertexBuffer::Map(bool is_partial_map) {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	mapped->Map(is_partial_map, 0);
}

void D3D11VertexBuffer::UnMap() {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	mapped->UnMap();
}

void D3D11VertexBuffer::Write(unsigned int index, const void *data, unsigned int array_size, unsigned int element_bytewidth) {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	CHECK(element_bytewidth == GetElementBytewidth())<<"Element size mismatch.";
	
	mapped->Write(index*element_bytewidth, data, array_size*element_bytewidth);
}

const void * D3D11VertexBuffer::Read(unsigned int index, unsigned int element_bytewidth) const {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	CHECK(element_bytewidth == GetElementBytewidth())<<"Element size mismatch.";
	return (const char *)mapped->Read() + index*element_bytewidth;
}

void D3D11VertexBuffer::Update(unsigned int index, const void *data, unsigned int array_size, unsigned int element_bytewidth) {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	CHECK(element_bytewidth == GetElementBytewidth())<<"Element size mismatch.";
	CHECK(mapped->GetMapBehavior() == GeneralEnum::MAP_WRITE_OCCASIONAL)<<
				"Only MAP_WRITE_OCCASIONAL is allowed to update.";
	
	D3D11_BOX dest;
	dest.left = index*element_bytewidth;
	dest.right = (index+array_size)*element_bytewidth;
	dest.top = 0;
	dest.bottom = 1;
	dest.front = 0;
	dest.back = 1;
	
	manager->GetDeviceContext()->UpdateSubresource(
		vb, 0, &dest, data, 0, 0
	);
}

}







