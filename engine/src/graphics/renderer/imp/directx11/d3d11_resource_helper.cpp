#pragma comment(lib, "d3d11.lib")

#include "d3d11_resource_helper.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

namespace s2 {

void D3D11ResourceHelper::SetBufferDesc(D3D11_BUFFER_DESC *desc, uint32_t  byte_width, RendererResourceWrite resource_write) {
	desc->ByteWidth = byte_width;
	switch(resource_write) {
		case RendererResourceWrite::IMMUTABLE:
			desc->Usage = D3D11_USAGE_IMMUTABLE;
			desc->CPUAccessFlags = 0;
			break;
		case RendererResourceWrite::CPU_WRITE_OCCASIONAL:
			desc->Usage = D3D11_USAGE_DEFAULT;
			desc->CPUAccessFlags = 0;
			break;
		case RendererResourceWrite::CPU_WRITE_FREQUENT:
			desc->Usage = D3D11_USAGE_DYNAMIC;
			desc->CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
		default:
			CHECK(false)<<"Unknown RendererResourceWrite flag";
			break;
	}

	desc->MiscFlags = 0;
	desc->StructureByteStride = 0;			
}
	
	
}
















