#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#include "d3d11_mapped_resource.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>
#include <stdio.h>

#include "d3d11_graphic_resource_manager.h"

namespace s2 {

D3D11MappedResource::D3D11MappedResource(
		D3D11GraphicResourceManager *_manager, ID3D11Resource *_resource, GeneralEnum::MapBehavior _map_behavior) 
			: 	manager(_manager), resource(_resource), map_behavior(_map_behavior){
	CHECK_NOTNULL(_manager);
	CHECK_NOTNULL(_resource);
	Clear();
}

void D3D11MappedResource::Clear() {
	mapped_data=0;
	row_offset=0;
	depth_offset=0;
	subresource_index=-1; 
}

void D3D11MappedResource::Map(bool is_partial_map, unsigned int _subresource_index) {
	subresource_index = _subresource_index;
	CHECK(map_behavior != GeneralEnum::MAP_FORBIDDEN) << 
					"Not able to map as map_behavior is indicated as MAP_FORBIDDEN ";
	CHECK(map_behavior != GeneralEnum::MAP_WRITE_OCCASIONAL) << "Map not allowed. Use Update directly for MAP_WRITE_OCASSIONAL";
	
	D3D11_MAP map_type = D3D11_MAP_WRITE_DISCARD;
	if(map_behavior == GeneralEnum::MAP_WRITE_FREQUENT) {
		map_type = is_partial_map ? D3D11_MAP_WRITE_NO_OVERWRITE  : D3D11_MAP_WRITE_DISCARD;
	} else if(map_behavior == GeneralEnum::MAP_READ) {
		map_type = D3D11_MAP_READ;
	} else if(map_behavior == GeneralEnum::MAP_READ_WRITE) {
		map_type = D3D11_MAP_READ_WRITE;
	}
	D3D11_MAPPED_SUBRESOURCE subresource;
	HRESULT result = 1;
	result = manager->GetDeviceContext()->Map(resource, subresource_index, map_type, 0, &subresource);
	mapped_data = subresource.pData;
	row_offset = subresource.RowPitch;
	depth_offset = subresource.DepthPitch;
	CHECK(!FAILED(result))<<"Fail to map resource. Error code "<<::GetLastError();
}

void D3D11MappedResource::Write(unsigned int offset, const void *data, unsigned int size) {
	CHECK(subresource_index != -1) << "Must call Map before Write.";
	CHECK(map_behavior != GeneralEnum::MAP_WRITE_OCCASIONAL) << "Map not allowed. Use Update directly for MAP_WRITE_OCASSIONAL";
	memcpy(mapped_data, data, size);
}

const void * D3D11MappedResource::Read() {
	CHECK(subresource_index != -1) << "Must call Map before Read.";
	CHECK(map_behavior==GeneralEnum::MAP_READ||map_behavior==GeneralEnum::MAP_READ_WRITE)<<
				"map behavior must be MAP_READ or MAP_READ_WRITE inorder to read.";
	return mapped_data;
}

void D3D11MappedResource::UnMap() {
	CHECK(subresource_index != -1) << "Must call Map before UnMap.";
	manager->GetDeviceContext()->Unmap(resource , subresource_index);
	Clear();
}

}






