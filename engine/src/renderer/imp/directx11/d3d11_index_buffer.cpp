#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#include "d3d11_index_buffer.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>
#include <stdio.h>

#include "d3d11_graphic_resource_manager.h"
#include "d3d11_resource_helper.h"
#include "d3d11_mapped_resource.h"

namespace s2 {

D3D11IndexBuffer::D3D11IndexBuffer(D3D11GraphicResourceManager *_manager)
		: manager(_manager), ib(0), mapped(0){
	Clear();
}

D3D11IndexBuffer::~D3D11IndexBuffer() {
	Clear();
}

void D3D11IndexBuffer::Clear() {
	if(ib) {
		ib->Release();
		ib=0;
	}
	ele_count = 0;
	delete mapped;
	mapped = 0;
}

void D3D11IndexBuffer::Initialize(unsigned int element_count, const InputType *data, RendererEnum::ResourceWrite resource_write) {
	Clear();
	ele_count = element_count;
	D3D11_BUFFER_DESC desc;
	D3D11ResourceHelper::SetBufferDesc(&desc, sizeof(InputType)*element_count, resource_write);
	
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	
	HRESULT result = 1;
	if(data) {
		D3D11_SUBRESOURCE_DATA subresource;
		subresource.pSysMem = data;
		result = manager->GetDevice()->CreateBuffer(&desc, &subresource, &ib);
	} else {
		result = manager->GetDevice()->CreateBuffer(&desc, 0, &ib);
	}
	
	CHECK(!FAILED(result)) <<"Cannot create index buffer. Error code: " << ::GetLastError();
	
	mapped = new D3D11MappedResource(manager, ib, resource_write);
}

unsigned int D3D11IndexBuffer::GetElementCount() const {
	CHECK(ib)<<"Index buffer is not initialized.";
	return ele_count;
}

RendererEnum::ResourceWrite D3D11IndexBuffer::GetResourceWrite() const {
	CHECK(ib)<<"Index buffer is not initialized.";
	return mapped->GetResourceWrite();
}

void D3D11IndexBuffer::Map(bool is_partial_map) {
	CHECK(ib)<<"Index buffer is not initialized.";
	mapped->Map(is_partial_map, 0);
}

void D3D11IndexBuffer::Write(unsigned int index, const InputType *data, unsigned int array_size) {
	CHECK(ib)<<"Index buffer is not initialized.";
	mapped->Write(index*sizeof(InputType), (const void *)data, array_size*sizeof(InputType));
	
}

const IndexBuffer::InputType * D3D11IndexBuffer::Read(unsigned int index) const {
	CHECK(ib)<<"Index buffer is not initialized.";
	return (const InputType *)((const char *)mapped->Read()+index*sizeof(InputType));
}

void D3D11IndexBuffer::UnMap() {
	CHECK(ib)<<"Index buffer is not initialized.";
	mapped->UnMap();
}

void D3D11IndexBuffer::Update(unsigned int index, const InputType *data, unsigned int array_size) {
	CHECK(ib)<<"Index buffer is not initialized.";
	CHECK(mapped->GetResourceWrite() == RendererEnum::CPU_WRITE_OCCASIONAL)<<
				"Only CPU_WRITE_OCCASIONAL is allowed to update.";
	
	D3D11_BOX dest;
	dest.left = index*sizeof(InputType);
	dest.right = (index+array_size)*sizeof(InputType);
	dest.top = 0;
	dest.bottom = 1;
	dest.front = 0;
	dest.back = 1;
	
	manager->GetDeviceContext()->UpdateSubresource(
		ib, 0, &dest, (const void *)data, 0, 0 
	);
}

}







