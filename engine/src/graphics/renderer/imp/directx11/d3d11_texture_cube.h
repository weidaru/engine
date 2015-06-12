#ifndef D3D11_TEXTURE_CUBE_H_
#define D3D11_TEXTURE_CUBE_H_

#include "graphics/renderer/texture_cube.h"
#include "d3d11_resource_view.h"

struct ID3D11Texture2D;

namespace s2 {

class D3D11MappedResource;
class D3D11GraphicResourceManager;

class D3D11TextureCube : public TextureCube {
public:
	D3D11TextureCube(D3D11GraphicResourceManager *_manager);

	virtual ~D3D11TextureCube();
	virtual void Initialize(const Option &option);
	virtual const TextureCube::Option & GetOption() const;
	
	virtual void WriteMap(GraphicPipeline *pipeline, bool no_overwrite, uint32_t mip_index, uint32_t array_index, CubeFace face);
	virtual void Write(uint32_t row, uint32_t col,  const void *data, uint32_t size);
	virtual void WriteUnmap();
	
	virtual void ReadMap(GraphicPipeline *pipeline, uint32_t mip_index, uint32_t array_index, CubeFace face, bool wipe_cache=true) const;
	virtual const void * Read(uint32_t row, uint32_t col) const;
	virtual void ReadUnmap() const;
	
	virtual void Update(
			GraphicPipeline *pipeline, 
			uint32_t mip_index, uint32_t array_index, CubeFace face,
			uint32_t left, uint32_t right,
			uint32_t top, uint32_t bottom,
			const void *data);

	virtual D3D11ShaderResource * AsShaderResource() const;

private:
	void Check() const;

private:
	Option option;
	D3D11GraphicResourceManager *manager;
	ID3D11Texture2D *tex;
	mutable D3D11MappedResource *mapped;

	D3D11ShaderResource *shader_resource;
};

}

#endif     //D3D11_TEXTURE_CUBE_H_