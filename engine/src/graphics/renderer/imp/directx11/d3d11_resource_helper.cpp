#pragma comment(lib, "d3d11.lib")

#include "d3d11_resource_helper.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

namespace s2 {

void D3D11ResourceHelper::SetBufferDesc(D3D11_BUFFER_DESC *desc, unsigned int  byte_width, RendererEnum::ResourceWrite resource_write) {
	desc->ByteWidth = byte_width;
	switch(resource_write) {
		case RendererEnum::IMMUTABLE:
			desc->Usage = D3D11_USAGE_IMMUTABLE;
			desc->CPUAccessFlags = 0;
			break;
		case RendererEnum::CPU_WRITE_OCCASIONAL:
			desc->Usage = D3D11_USAGE_DEFAULT;
			desc->CPUAccessFlags = 0;
			break;
		case RendererEnum::CPU_WRITE_FREQUENT:
			desc->Usage = D3D11_USAGE_DYNAMIC;
			desc->CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
		default:
			CHECK(false)<<"Unknown ResourceWrite flag";
			break;
	}

	desc->MiscFlags = 0;
	/*
	 * This is used by Compute Shader which is a more general way of using parallel GPU power.
	 * In our use case, we only care about the graphic part. So make this 0 anyway.
	 */
	desc->StructureByteStride = 0;			
}
	
	
}
















