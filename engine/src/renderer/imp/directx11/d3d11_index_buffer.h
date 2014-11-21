#ifndef D3D11_INDEX_BUFFER_H_
#define D3D11_INDEX_BUFFER_H_

#include "renderer/index_buffer.h"

#include <stdint.h>

struct ID3D11Buffer;

namespace s2 {

class D3D11GraphicResourceManager;

class D3D11IndexBuffer : public IndexBuffer {
public:
	D3D11IndexBuffer(D3D11GraphicResourceManager *_manager);
	virtual ~D3D11IndexBuffer();
	virtual void Initialize(unsigned int element_count, const InputType *data, GeneralEnum::MapBehavior map_behavior);
	virtual GeneralEnum::CPUAccess GetCPUAccessFlag() const;
	virtual unsigned int GetElementCount() const;
	virtual void Map(bool is_partial_map);
	virtual void Update(unsigned int offset, const void *data, unsigned int size);
	virtual void UnMap();
	
	/*********************D3D11 exclusive***********************/
	ID3D11Buffer * GetInternal() { return ib; }
	
private:
	void Clear();

private:
	D3D11GraphicResourceManager *manager;
	ID3D11Buffer *ib;
	unsigned int ele_count;

	
};

}


#endif		//D3D11_INDEX_BUFFER_H_