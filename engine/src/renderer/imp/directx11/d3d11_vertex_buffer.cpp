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
													unsigned int per_ele_size, const void *data, 
													RendererEnum::ResourceWrite resource_write,
													Binding _binding) {
	Clear();
	CHECK(element_count>0 && per_ele_size>0)<<"element count and element bytewidth must not be 0";
	
	ele_count = element_count;
	ele_member_count = element_member_count;
	ele_bytewidth = per_ele_size;
	binding = _binding;
	
	D3D11_BUFFER_DESC desc;
	D3D11ResourceHelper::SetBufferDesc(&desc, per_ele_size*element_count, resource_write);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	if((binding&VERTEX_BUFFER) == 0) {
		LOG(ERROR)<<"Vertex Buffer is not bind as normal usage. Bind it automatically.";
	}
	if((binding&SHADER_RESOURCE) != 0) { 
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	}
	if((binding&STREAM_OUT) != 0) {
		desc.BindFlags = D3D11_BIND_STREAM_OUTPUT;
		CHECK(resource_write != RendererEnum::IMMUTABLE) << "STREAM_OUT binding cannot be used together with IMMUTABLE.";
	}
	
	HRESULT result = 1;
	if(data) {
		D3D11_SUBRESOURCE_DATA subresource;
		subresource.pSysMem = data;
		result = manager->GetDevice()->CreateBuffer(&desc, &subresource, &vb);
	} else {
		result = manager->GetDevice()->CreateBuffer(&desc, 0, &vb);
	}
	
	CHECK(!FAILED(result))<<"Cannot create vertex buffer. Error code: " <<::GetLastError();
	
	mapped = new D3D11MappedResource(manager->GetDeviceContext(), vb, resource_write);
}

void D3D11VertexBuffer::Initialize(unsigned int element_count, const TypeInfo &type_info, const void *data, 
												RendererEnum::ResourceWrite resource_write,
												Binding _binding) {
	type_name = type_info.GetName();
	this->Initialize(element_count, type_info.GetMemberSize(), type_info.GetSize(), data, resource_write, _binding);
}

unsigned int D3D11VertexBuffer::GetElementCount() const {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	return ele_count;
}

unsigned int D3D11VertexBuffer::GetElementBytewidth() const {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	return ele_bytewidth;
}

RendererEnum::ResourceWrite D3D11VertexBuffer::GetResourceWrite() const {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	return mapped->GetResourceWrite();
}

VertexBuffer::Binding D3D11VertexBuffer::GetBinding() const {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	return binding;
}

unsigned int D3D11VertexBuffer::GetElementMemberCount() const {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	return ele_member_count;
}

void D3D11VertexBuffer::WriteMap(bool is_partial_map) {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	mapped->WriteMap(is_partial_map, 0);
}

void D3D11VertexBuffer::WriteUnmap() {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	mapped->WriteUnmap();
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

void D3D11VertexBuffer::ReadMap(bool wipe_cache) {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	//Create the staging resource if not present.
	if(mapped->GetStagingResource() == 0) {
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = ele_bytewidth*ele_count;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;		
		
		HRESULT result = 1;
		ID3D11Buffer *staging_resource;
		result = manager->GetDevice()->CreateBuffer(&desc, 0, &staging_resource);
		CHECK(!FAILED(result))<<"Cannot create staging resource. Error "<<::GetLastError();
		mapped->SetStagingResource(staging_resource);
	}
	mapped->ReadMap(0, wipe_cache);
}

void D3D11VertexBuffer::ReadUnmap() {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	mapped->ReadUnmap();
}

void D3D11VertexBuffer::Update(unsigned int index, const void *data, unsigned int array_size, unsigned int element_bytewidth) {
	CHECK(vb)<<"Vertex buffer is not initialized.";
	CHECK(element_bytewidth == GetElementBytewidth())<<"Element size mismatch.";
	CHECK(mapped->GetResourceWrite() == RendererEnum::CPU_WRITE_OCCASIONAL)<<
				"Only CPU_WRITE_OCCASIONAL is allowed to update.";
	
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







