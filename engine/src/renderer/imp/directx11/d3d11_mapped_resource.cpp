#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#include "d3d11_mapped_resource.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

D3D11MappedResource::D3D11MappedResource(
		D3D11GraphicResourceManager *_manager, ID3D11Resource *_resource, GeneralEnum::MapBehavior _map_behavior) 
			: 	manager(_manager), resource(_resource), map_behavior(_map_behavior), 
				mapped_data(0), row_offset(0), depth_offset(0) {
	CHECK_NOTNULL(_manager);
	CHECK_NOTNULL(_resource);
	CHECK_NOTNULL(_map_behavior);
}

void D3D11MappedResource::Map(bool is_partial_map, unsigned int subresource_index) {
	CHECK(map_behavior != GeneralEnum::MAP_FORBIDDEN) << "Not able to map as map_behavior is indicated as MAP_FORBIDDEN";
	if(map_behavior == GeneralEnum::MAP_WRITE_OCCASIONAL)
		mapped_data = 0;
	} else {
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
		result = manager->GetDeviceContext()->Map(ib, subresource_index, map_type, 0, &subresource);
		mapped_data = subresource.data;
		row_offset = subresource.RowPitch;
		depth_offset = subresource.DepthPitch;
		CHECK(!FAILED(result))<<"Fail to map resource. Error code "<<::GetLastError();
	}
}

void D3D11MappedResource::Update(unsigned int offset, void *data, unsigned int size) {
	
}

void D3D11MappedResource::UnMap() {

}