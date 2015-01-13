#ifndef D3D11_INDEX_BUFFER_H_
#define D3D11_INDEX_BUFFER_H_

#include "renderer/index_buffer.h"

#include <stdint.h>

struct ID3D11Buffer;

namespace s2 {

class D3D11MappedResource;
class D3D11GraphicResourceManager;

class D3D11IndexBuffer : public IndexBuffer {
public:
	D3D11IndexBuffer(D3D11GraphicResourceManager *_manager);
	virtual ~D3D11IndexBuffer();
	virtual void Initialize(unsigned int element_count, const InputType *data, RendererEnum::ResourceWrite resource_write);
	virtual unsigned int GetElementCount() const;
	
	virtual RendererEnum::ResourceWrite GetResourceWrite() const;
	virtual void WriteMap(bool is_partial_map);
	virtual void Write(unsigned int index, const InputType *data, unsigned int array_size);
	virtual void WriteUnmap();
	
	virtual void Update(unsigned int index, const InputType *data, unsigned int array_size);
	
	/*********************D3D11 exclusive***********************/
	ID3D11Buffer * GetInternal() { return ib; }
	
private:
	void Clear();

private:
	D3D11GraphicResourceManager *manager;
	ID3D11Buffer *ib;
	unsigned int ele_count;

	D3D11MappedResource *mapped;
};

}


#endif		//D3D11_INDEX_BUFFER_H_