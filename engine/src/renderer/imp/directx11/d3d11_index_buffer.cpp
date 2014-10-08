#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#include "d3d11_index_buffer.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>
#include <stdio.h>

#include "d3d11_graphic_resource_manager.h"

namespace s2 {

D3D11IndexBuffer::D3D11IndexBuffer(D3D11GraphicResourceManager *_manager)
		: manager(_manager){

}

D3D11IndexBuffer::~D3D11IndexBuffer() {
	Clear();
}

void D3D11IndexBuffer::Clear() {
	if(ib) {
		ib->Release();
		ib=0;
	}
}

bool D3D11IndexBuffer::Initialize(unsigned int size, const uint32_t *data, bool is_dynamic) {
	Clear();
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(uint32_t)*size;
	if(is_dynamic) {
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	} else {
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
	}
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	
	D3D11_SUBRESOURCE_DATA subresource;
	subresource.pSysMem = data;
	
	HRESULT result = 1;
	result = manager->GetDevice()->CreateBuffer(&desc, &subresource, &ib);
	CHECK(!FAILED(result)) <<"Cannot create index buffer. Error code: " << ::GetLastError();
	return true;
}

bool D3D11IndexBuffer::IsDynamic() {
	CHECK(ib)<<"Index buffer is not initialized.";
	D3D11_BUFFER_DESC desc;
	ib->GetDesc(&desc);
	return desc.Usage == D3D11_USAGE_DYNAMIC;
}

unsigned int D3D11IndexBuffer::GetSize() {
	CHECK(ib)<<"Index buffer is not initialized.";
	D3D11_BUFFER_DESC desc;
	ib->GetDesc(&desc);
	return desc.ByteWidth;
}

void * D3D11IndexBuffer::Map() {
	CHECK(ib)<<"Index buffer is not initialized.";
	CHECK(IsDynamic())<<"Index buffer must be dynamic in order to map.";
	D3D11_MAPPED_SUBRESOURCE subresource;
	HRESULT result = 1;
	result = manager->GetDeviceContext()->Map(ib, 1, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	CHECK(!FAILED(result))<<"Fail to map index buffer. Error code "<<::GetLastError();
	
	return subresource.pData;
}


void D3D11IndexBuffer::UnMap() {
	CHECK(ib)<<"Index buffer is not initialized.";
	CHECK(IsDynamic())<<"Index buffer mut be dynamic in order to unmap.";
	HRESULT result = 1;
	manager->GetDeviceContext()->Unmap(ib,0);
}

void D3D11IndexBuffer::GetLastError(s2string *str) {
	*str = error;
}

}







