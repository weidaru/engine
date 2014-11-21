#ifndef D3D11_MAPPED_RESOURCE_H_
#define D3D11_MAPPED_RESOURCE_H_

#include "renderer/general_enum.h"

struct ID3D11Resource;

namespace s2 {

class D3D11GraphicResourceManager;

class D3D11MappedResource {
public:
	D3D11MappedResource(D3D11GraphicResourceManager *manager, ID3D11Resource *resource, GeneralEnum::MapBehavior mapped_behavior);
	
	void Map(bool is_partial_map);
	void Update(unsigned int offset, const void *data, unsigned int size);
	void UnMap();
	
	unsigned int GetRowOffset();
	unsigned int GetDepthOffset();
	
private:
	D3D11GraphicResourceManager *manager;
	ID3D11Resource *resource;
	GeneralEnum::MapBehavior map_behavior;
	void *mapped_data;
	unsigned int row_offset;
	unsigned int depth_offset;
};


}


#endif		//D3D11_MAPPED_RESOURCE_H_
