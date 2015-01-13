#ifndef D3D11_MAPPED_RESOURCE_H_
#define D3D11_MAPPED_RESOURCE_H_

#include "renderer/renderer_enum.h"

struct ID3D11Resource;
struct ID3D11DeviceContext;

namespace s2 {

class D3D11MappedResource {
public:
	D3D11MappedResource(ID3D11DeviceContext *_context, ID3D11Resource *resource, RendererEnum::ResourceWrite _resource_write);
	~D3D11MappedResource();
	
	void WriteMap(bool is_partial_map, unsigned int subresource_index);
	void Write(unsigned int offset, const void *data, unsigned int size);
	void WriteUnmap();
	
	D3D11MappedResource & SetStagingResource(ID3D11Resource *new_resource) {
		this->staging_resource = new_resource; 
		return *this; 
	}
	void GetStagingResource() const { return staging_resource; }
	
	void ReadMap(unsigned int subresource_index, bool wipe_cache);
	void * Read();
	void ReadUnmap();
	
	RendererEnum::ResourceWrite GetResourceWrite() const { return resource_write; }

	
private:
	ID3D11DeviceContext *context;
	ID3D11Resource *resource;
	RendererEnum::ResourceWrite resource_write;
	
	void *mapped_data;
	int write_index;
	
	ID3D11Resource *staging_resource;
	void *staging_data;
	int read_index;
};


}


#endif		//D3D11_MAPPED_RESOURCE_H_
