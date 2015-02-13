#pragma comment(lib, "d3d11.lib")

#include "d3d11_buffer.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_graphic_resource_manager.h"
#include "d3d11_resource_helper.h"
#include "d3d11_mapped_resource.h"
#include "d3d11_resource_view.h"

namespace s2 {

D3D11Buffer::D3D11Buffer(D3D11GraphicResourceManager *_manager)
	:	manager(_manager), buffer(0), mapped(0),
		ele_count(0), ele_member_count(0), ele_bytewidth(0),
		index_buffer(0), vertex_buffer(0), stream_out(0) {
}

D3D11Buffer::~D3D11Buffer() {
	delete stream_out;
	delete vertex_buffer;
	delete index_buffer;
	delete mapped;
	buffer->Release();
}

void D3D11Buffer::Check() const {
	CHECK(buffer) << "Vertex buffer is not initialized.";
}

void D3D11Buffer::Initialize(	unsigned int element_count, unsigned int element_member_count,
											unsigned int per_ele_size, const void *data,
											RendererEnum::ResourceWrite resource_write,
											Binding _binding) {
	CHECK(buffer == 0) << "Cannot initialize a buffer twice.";
	CHECK(element_count>0 && per_ele_size>0) << "element count and element bytewidth must not be 0";

	ele_count = element_count;
	ele_member_count = element_member_count;
	ele_bytewidth = per_ele_size;
	binding = _binding;

	D3D11_BUFFER_DESC desc;
	D3D11ResourceHelper::SetBufferDesc(&desc, per_ele_size*element_count, resource_write);
	desc.BindFlags = 0;
	if ((binding & 0xF) == VERTEX_BUFFER) {
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	} else if ((binding & 0xF) == INDEX_BUFFER) {
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	}
	if ((binding&0xF0) == STREAM_OUT) {
		desc.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
		CHECK(resource_write != RendererEnum::IMMUTABLE) << "STREAM_OUT binding cannot be used together with IMMUTABLE.";
	}

	HRESULT result = 1;
	if (data) {
		D3D11_SUBRESOURCE_DATA subresource;
		subresource.pSysMem = data;
		result = manager->GetDevice()->CreateBuffer(&desc, &subresource, &buffer);
	}
	else {
		result = manager->GetDevice()->CreateBuffer(&desc, 0, &buffer);
	}

	CHECK(!FAILED(result)) << "Cannot create vertex buffer. Error code: " << ::GetLastError();

	mapped = new D3D11MappedResource(manager->GetDeviceContext(), buffer, resource_write);

	//Create all the views.
	if ((binding & 0xF) == VERTEX_BUFFER) {
		vertex_buffer = new D3D11VertexBuffer(this);
	} else if ((binding & 0xF) == INDEX_BUFFER) {
		index_buffer = new D3D11IndexBuffer(this);
	} 
	if ((binding & 0xF0) == STREAM_OUT) {
		stream_out = new D3D11StreamOut(this);
	}
}

void D3D11Buffer::Initialize(unsigned int element_count, const TypeInfo &type_info, const void *data,
	RendererEnum::ResourceWrite resource_write,
	Binding _binding) {
	type_name = type_info.GetName();
	this->Initialize(element_count, type_info.GetMemberSize(), type_info.GetSize(), data, resource_write, _binding);
}

unsigned int D3D11Buffer::GetElementCount() const {
	Check();
	return ele_count;
}

unsigned int D3D11Buffer::GetElementBytewidth() const {
	Check();
	return ele_bytewidth;
}

RendererEnum::ResourceWrite D3D11Buffer::GetResourceWrite() const {
	Check();
	return mapped->GetResourceWrite();
}

Buffer::Binding D3D11Buffer::GetBinding() const {
	Check();
	return binding;
}

unsigned int D3D11Buffer::GetElementMemberCount() const {
	Check();
	return ele_member_count;
}

void D3D11Buffer::WriteMap(bool is_partial_map) {
	Check();
	mapped->WriteMap(is_partial_map, 0);
}

void D3D11Buffer::WriteUnmap() {
	Check();
	mapped->WriteUnmap();
}

void D3D11Buffer::Write(unsigned int index, const void *data, unsigned int array_size, unsigned int element_bytewidth) {
	Check();
	CHECK(element_bytewidth == GetElementBytewidth()) << "Element size mismatch.";

	mapped->Write(index*element_bytewidth, data, array_size*element_bytewidth);
}

const void * D3D11Buffer::Read(unsigned int index, unsigned int element_bytewidth) const {
	Check();
	CHECK(element_bytewidth == GetElementBytewidth()) << "Element size mismatch.";
	return (const char *)mapped->Read() + index*element_bytewidth;
}

void D3D11Buffer::ReadMap(bool wipe_cache) const {
	Check();
	//Create the staging resource if not present.
	if (mapped->GetStagingResource() == 0) {
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
		CHECK(!FAILED(result)) << "Cannot create staging resource. Error " << ::GetLastError();
		mapped->SetStagingResource(staging_resource);
	}
	mapped->ReadMap(0, wipe_cache);
}

void D3D11Buffer::ReadUnmap() const {
	Check();
	mapped->ReadUnmap();
}

void D3D11Buffer::Update(unsigned int index, const void *data, unsigned int array_size, unsigned int element_bytewidth) {
	Check();
	CHECK(element_bytewidth == GetElementBytewidth()) << "Element size mismatch.";
	CHECK(mapped->GetResourceWrite() == RendererEnum::CPU_WRITE_OCCASIONAL) <<
		"Only CPU_WRITE_OCCASIONAL is allowed to update.";

	D3D11_BOX dest;
	dest.left = index*element_bytewidth;
	dest.right = (index + array_size)*element_bytewidth;
	dest.top = 0;
	dest.bottom = 1;
	dest.front = 0;
	dest.back = 1;

	manager->GetDeviceContext()->UpdateSubresource(
		buffer, 0, &dest, data, 0, 0);
}

D3D11IndexBuffer * D3D11Buffer::AsIndexBuffer() const {
	CHECK(index_buffer != 0)<<"Buffer is not binded as index buffer";
	return index_buffer;
}

D3D11VertexBuffer * D3D11Buffer::AsVertexBuffer() const {
	CHECK(vertex_buffer != 0) << "Buffer is not binded as vertex buffer";
	return vertex_buffer;
}

D3D11StreamOut * D3D11Buffer::AsStreamOut() const {
	CHECK(stream_out != 0) << "Buffer is not binded as stream out";
	return stream_out;
}

}







