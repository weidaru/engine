#pragma comment(lib, "d3d11.lib")


#include "d3d11_constant_buffer.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>
#include <stdio.h>

#include "d3d11_graphic_resource_manager.h"
#include "d3d11_graphic_pipeline.h"


namespace s2 {

D3D11ConstantBuffer::D3D11ConstantBuffer(D3D11GraphicResourceManager * _manager)
		: manager(_manager), cb(0), size(0), data_buffer(0){

}

D3D11ConstantBuffer::~D3D11ConstantBuffer() {
	Clear();
}

void D3D11ConstantBuffer::Clear() {
	if(cb) {
		cb->Release();
		cb = 0;
	}
	size = 0;
	delete[] data_buffer;
}

bool D3D11ConstantBuffer::SetData(uint32_t offset, const void *data, uint32_t _size) {
	if(offset+_size > size) {
		error = "Data overflow.";
		return false;
	}
	memcpy((void*)(data_buffer+offset), data, _size);
	return true;
}

void D3D11ConstantBuffer::Initialize(uint32_t _size, const void *data) {
	Clear();
	size = _size;
	data_buffer = new char[size];
	if(data) {
		memcpy((void *)data_buffer, data, size);
	} else {
		memset((void *)data_buffer, 0, size);
	}

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	
	HRESULT result = 1;
	if(data) {
		D3D11_SUBRESOURCE_DATA subresource;
		subresource.pSysMem = data;
		result = manager->GetDevice()->CreateBuffer(&desc, &subresource, &cb);
	}
	else
		result = manager->GetDevice()->CreateBuffer(&desc, 0, &cb);
	
	CHECK(!FAILED(result))<<"Cannot create constant buffer. Error code: " << ::GetLastError();
}

void D3D11ConstantBuffer::Flush(D3D11GraphicPipeline *pipeline) {
	CHECK(cb)<<"Constant buffer is not initialized.";
	D3D11_MAPPED_SUBRESOURCE subresource;
	HRESULT result=1;
	result = pipeline->GetDeviceContext()->Map(cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	CHECK(!FAILED(result))<<"Fail to map the constant buffer. Error code  "<<::GetLastError();
	memcpy(subresource.pData, (const void *)data_buffer, size);
	
	pipeline->GetDeviceContext()->Unmap(cb, 0);
}

}

