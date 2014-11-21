#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#include "d3d11_resoruce_helper.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

namespace s2 {

void D3D11ResourceHelper::SetBufferDesc(D3D11_BUFFER_DESC *desc, unsigned int  byte_width, GeneralEnum::MapBehavior map_behavior) {
	desc->ByteWidth = byte_width;
	switch(map_behavior) {
		case GeneralEnum::MAP_FORBIDDEN:
			desc->Usage = D3D11_USAGE_IMMUTABLE;
			desc->CPUAccessFlags = 0;
			break;
		case GeneralEnum::MAP_WRITE_OCCASIONAL:
			desc->Usage = D3D11_USAGE_DEFAULT;
			desc->CPUAccessFlags = 0;
			break;
		case GeneralEnum::MAP_WRITE_FREQUENT:
			desc->Usage = D3D11_USAGE_DYNAMIC;
			desc->CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
		case GeneralEnum::MAP_READ:
			desc->Usage = D3D11_USAGE_STAGING;
			desc->CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			break;
		
		case GeneralEnum::MAP_READ_WRITE:
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

void * D3D11ResourceHelper::Map(ID3D11DeviceContext *context, ID3D11Resource *resource, 
									GeneralEnum::MapBehavior map_behavior, bool is_partial_map, s2string *error) {
	
}

void D3D11ResourceHelper::UnMap(ID3D11DeviceContext *context, ID3D11Resource *resource) {

}
	
	
}
















