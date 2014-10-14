#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#include "d3d11_constant_buffer.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>
#include <stdio.h>

#include "d3d11_graphic_resource_manager.h"


namespace s2 {

D3D11ConstantBuffer::D3D11ConstantBuffer(D3D11GraphicResourceManager * _manager)
		: manager(_manager), cb(0){

}

D3D11ConstantBuffer::~D3D11ConstantBuffer() {
	Clear();
}

void D3D11ConstantBuffer::Clear() {
	if(cb) {
		cb->Release();
		cb = 0;
	}
}

void D3D11ConstantBuffer::Initialize(unsigned int size, const void *data) {
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
	D3D11_SUBRESOURCE_DATA subresource;
	subresource.pSysMem = data;
	
	HRESULT result = 1;
	result = manager->GetDevice()->CreateBuffer(&desc, &subresource, &cb);
	CHECK(!FAILED(result))<<"Cannot create constant buffer. Error code: " << ::GetLastError();
}

unsigned int D3D11ConstantBuffer::GetSize() {
	CHECK(cb)<<"Constant buffer is not initialized.";
	D3D11_BUFFER_DESC desc;
	cb->GetDesc(&desc);
	
	return desc.ByteWidth;
}

void * D3D11ConstantBuffer::Map() {
	CHECK(cb)<<"Constant buffer is not initialized.";
	D3D11_MAPPED_SUBRESOURCE subresource;
	HRESULT result=1;
	result = manager->GetDeviceContext()->Map(cb, 1, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	CHECK(!FAILED(result))<<"Fail to map the constant buffer. Error code  "<<::GetLastError();

	return subresource.pData;
}

void D3D11ConstantBuffer::UnMap() {
	CHECK(cb)<<"Constant buffer is not initialized.";
	HRESULT result = 1;
	manager->GetDeviceContext()->Unmap(cb, 0);
}

}

