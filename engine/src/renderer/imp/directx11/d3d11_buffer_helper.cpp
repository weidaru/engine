#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#include "d3d11_buffer_helper.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

namespace s2 {

void D3D11BufferHelper::SetBufferDesc(D3D11_BUFFER_DESC *desc, unsigned int  byte_width, GeneralEnum::CPUAccess cpu_access) {
	desc->ByteWidth = byte_width;
	switch(cpu_access) {
		case GeneralEnum::CPU_NO_ACCESS:
			desc->Usage = D3D11_USAGE_DEFAULT;
			desc->CPUAccessFlags = 0;
			break;
		case GeneralEnum::CPU_READ:
			desc->Usage = D3D11_USAGE_STAGING;
			desc->CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			break;
		case GeneralEnum::CPU_WRITE:
			desc->Usage = D3D11_USAGE_DYNAMIC;
			desc->CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
		case GeneralEnum::CPU_READ_WRITE:
			desc->Usage = D3D11_USAGE_STAGING;
			desc->CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
			break;
	}

	desc->MiscFlags = 0;
	/*
	 * This is used by Compute Shader which is a more general way of using parallel GPU power.
	 * In our use case, we only care about the graphic part. So make this 0 anyway.
	 */
	desc->StructureByteStride = 0;			
}

void * D3D11BufferHelper::Map(ID3D11DeviceContext *context, ID3D11Buffer *buffer, GeneralEnum::CPUAccess cpu_access, s2string *error) {
	CHECK(buffer)<<"Buffer is not initialized.";
	CHECK(cpu_access==GeneralEnum::CPU_WRITE || cpu_access==GeneralEnum::CPU_READ_WRITE)<<
				"Buffer must be initialized to CPU_WRITE or CPU_READ_WRITE in order to map.";
	D3D11_MAPPED_SUBRESOURCE subresource;
	HRESULT result = 1;
	result = context->Map(ib, 1, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	CHECK(!FAILED(result))<<"Fail to map index buffer. Error code "<<::GetLastError();
	
	return subresource.pData;
}

void D3D11BufferHelper::UnMap() {

}
	
	
}
















