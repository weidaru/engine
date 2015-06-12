#ifndef D3D11_MAPPED_RESOURCE_H_
#define D3D11_MAPPED_RESOURCE_H_

#include "graphics/renderer/renderer_enum.h"
#include <stdint.h>

struct ID3D11Resource;
struct ID3D11DeviceContext;

namespace s2 {
class D3D11GraphicPipeline;

class D3D11MappedResource {
public:
	D3D11MappedResource(ID3D11Resource *resource, RendererEnum::ResourceWrite _resource_write);
	~D3D11MappedResource();
	
	void WriteMap(D3D11GraphicPipeline *pipeline, bool is_partial_map, uint32_t subresource_index);
	void Write(uint32_t offset, const void *data, uint32_t size);
	void WriteUnmap();
	uint32_t GetWriteRowPitch() { return write_row_pitch; }
	uint32_t GetWriteDepthPitch() { return write_depth_pitch; }
	
	D3D11MappedResource & SetStagingResource(ID3D11Resource *new_resource) {
		this->staging_resource = new_resource; 
		return *this; 
	}
	ID3D11Resource * GetStagingResource() const { return staging_resource; }
	
	void ReadMap(D3D11GraphicPipeline *pipeline, uint32_t subresource_index, bool wipe_cache);
	const void * Read() const;
	void ReadUnmap();
	uint32_t GetReadRowPitch() { return read_row_pitch; }
	uint32_t GetReadDepthPitch() { return read_depth_pitch; }
	
	RendererEnum::ResourceWrite GetResourceWrite() const { return resource_write; }

	
private:
	ID3D11DeviceContext *context;
	ID3D11Resource *resource;
	RendererEnum::ResourceWrite resource_write;
	
	void *mapped_data;
	int write_index;
	uint32_t write_row_pitch, write_depth_pitch;
	
	ID3D11Resource *staging_resource;
	void *staging_data;
	int read_index;
	uint32_t read_row_pitch, read_depth_pitch;
};


}


#endif		//D3D11_MAPPED_RESOURCE_H_
