#pragma comment(lib, "d3d11.lib")


#include "d3d11_mapped_resource.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>
#include <stdio.h>

#include "d3d11_graphic_resource_manager.h"

namespace s2 {

D3D11MappedResource::D3D11MappedResource(
		ID3D11DeviceContext *_context, ID3D11Resource *_resource, RendererEnum::ResourceWrite _resource_write) 
			: 	context(_context), resource(_resource), resource_write(_resource_write) {
	CHECK_NOTNULL(_context);
	CHECK_NOTNULL(_resource);
	mapped_data = 0;
	write_index = -1;
	write_row_pitch = 0;
	write_depth_pitch = 0;
	
	staging_resource = 0;
	staging_data = 0;
	read_index = -1;
	read_row_pitch = 0;
	read_depth_pitch = 0;
}

D3D11MappedResource::~D3D11MappedResource() {
	if(staging_resource) {
		staging_resource->Release();
	}
	staging_resource = 0;
}


void D3D11MappedResource::WriteMap(bool is_partial_map, unsigned int subresource_index) {
	write_index = subresource_index;
	CHECK(mapped_data == 0)<<"Cannot map a resource twice in a row. Call ReadUnmap first";
	CHECK(resource_write != RendererEnum::IMMUTABLE) << 
					"Not able to map as resource_write is indicated as IMMUTABLE ";
	CHECK(resource_write != RendererEnum::CPU_WRITE_OCCASIONAL) << "Map not allowed. Use Update directly for MAP_WRITE_OCASSIONAL";
	
	D3D11_MAP map_type = D3D11_MAP_WRITE_DISCARD;
	if(resource_write == RendererEnum::CPU_WRITE_FREQUENT) {
		map_type = is_partial_map ? D3D11_MAP_WRITE_NO_OVERWRITE  : D3D11_MAP_WRITE_DISCARD;
	}
	D3D11_MAPPED_SUBRESOURCE subresource;
	HRESULT result = 1;
	result = context->Map(resource, write_index, map_type, 0, &subresource);
	mapped_data = subresource.pData;
	write_row_pitch = subresource.RowPitch;
	write_depth_pitch = subresource.DepthPitch;
	CHECK(!FAILED(result))<<"Fail to map resource. Error code "<<::GetLastError();
}

void D3D11MappedResource::Write(unsigned int offset, const void *data, unsigned int size) {
	CHECK(mapped_data) << "Must call WriteMap before Write.";
	CHECK(resource_write != RendererEnum::CPU_WRITE_OCCASIONAL) << "Map not allowed. Use Update directly for CPU_WRITE_OCASSIONAL";
	memcpy(mapped_data, data, size);
}

void D3D11MappedResource::WriteUnmap() {
	CHECK(mapped_data) << "Must call WriteMap before WriteUnmap.";
	context->Unmap(resource , write_index);
	mapped_data = 0;
	write_index = -1;
	write_row_pitch = 0;
	write_depth_pitch = 0;
}

void D3D11MappedResource::ReadMap(unsigned int subresource_index, bool wipe_cache) {
	//Copy resource to staging resource.
	read_index = subresource_index;
	CHECK(staging_resource)<<"staging_resource must be set before mapping";
	CHECK(staging_data==0)<<"Cannot map a resource twice in a row. Call ReadUnmap first";
	if(wipe_cache) {
		context->CopyResource(staging_resource, resource);
	}
	HRESULT result = 1;
	D3D11_MAPPED_SUBRESOURCE subresource;
	result = context->Map(staging_resource, read_index, D3D11_MAP_READ, 0, &subresource);
	staging_data = subresource.pData;
	read_row_pitch = subresource.RowPitch;
	read_depth_pitch = subresource.DepthPitch;
	CHECK(!FAILED(result))<<"Fail to map resource. Error code "<<::GetLastError();
}

void D3D11MappedResource::ReadUnmap() {
	CHECK(staging_data) << "Must call ReadMap before ReadUnmap.";
	context->Unmap(staging_resource, read_index);
	staging_data = 0;
	read_index = -1;
	read_row_pitch = 0;
	read_depth_pitch = 0;
}

const void * D3D11MappedResource::Read() const {
	CHECK(staging_data) << "Must call WriteMap before WriteUnmap.";
	return staging_data;
}


}






