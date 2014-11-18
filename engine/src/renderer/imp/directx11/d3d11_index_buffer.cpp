#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#include "d3d11_index_buffer.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>
#include <stdio.h>

#include "d3d11_graphic_resource_manager.h"
#include "d3d11_buffer_helper.h"

namespace s2 {

D3D11IndexBuffer::D3D11IndexBuffer(D3D11GraphicResourceManager *_manager)
		: manager(_manager), ib(0){
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
	cpu_access = GeneralEnum::CPU_NO_ACCESS;
}

void D3D11IndexBuffer::Initialize(unsigned int element_count, const InputType *data, GeneralEnum::CPUAccess _cpu_access) {
	Clear();
	ele_count = element_count;
	cpu_access = cpu_access;
	D3D11_BUFFER_DESC desc;
	D3D11BufferHelper::SetBufferDesc(&desc, sizeof(InputType)*element_count, cpu_access);
	
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
}

virtual GeneralEnum::CPUAccess GetCPUAccessFlag() const; {
	CHECK(ib)<<"Index buffer is not initialized.";
	return cpu_access;
}

unsigned int D3D11IndexBuffer::GetElementCount() const {
	CHECK(ib)<<"Index buffer is not initialized.";
	return ele_count;
}

void * D3D11IndexBuffer::Map() {
	CHECK(ib)<<"Index buffer is not initialized.";
	CHECK(cpu_access==GeneralEnum::CPU_WRITE || cpu_access==GeneralEnum::CPU_READ_WRITE)<<
				"Index buffer must be set to CPU_WRITE or CPU_READ_WRITE in order to map.";
	D3D11_MAPPED_SUBRESOURCE subresource;
	HRESULT result = 1;
	result = manager->GetDeviceContext()->Map(ib, 1, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	CHECK(!FAILED(result))<<"Fail to map index buffer. Error code "<<::GetLastError();
	
	return subresource.pData;
}


void D3D11IndexBuffer::UnMap() {
	CHECK(ib)<<"Index buffer is not initialized.";
	CHECK(cpu_access==GeneralEnum::CPU_WRITE || cpu_access==GeneralEnum::CPU_READ_WRITE)<<
				"Index buffer must be set to CPU_WRITE or CPU_READ_WRITE in order to map.";
	manager->GetDeviceContext()->Unmap(ib,0);
}

}







