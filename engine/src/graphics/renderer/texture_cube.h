#ifndef TEXTURE_CUBE_H_
#define TEXTURE_CUBE_H_

#include "resource.h"
#include "renderer_enum.h"
#include "texture_data.h"


namespace s2 {

class RenderTarget;
class DepthStencil;
class ShaderResource;
class GraphicPipeline;

/**
 * TextureCube can only be used as a ShaderResource.
 */
class TextureCube : public Resource {
public:
	struct Option {
		uint32_t 	width;
		uint32_t 	height;
		uint32_t 	mip_level;				//0 is full mipmap.
		uint32_t 	array_size;
		RendererEnum::Format 	format;
		RendererEnum::ResourceWrite resource_write;

		TextureCubeData data;
		
		Option() {
			width = 0;
			height = 0;
			mip_level = 1;
			array_size = 1;
			format = RendererEnum::R8G8B8A8_UNORM;
			resource_write = RendererEnum::CPU_WRITE_OCCASIONAL;
		}
	};

public:
	virtual ~TextureCube() {}
	virtual void Initialize(const Option &option) = 0;
	virtual const TextureCube::Option & GetOption() const = 0;		
	
	virtual void WriteMap(GraphicPipeline *pipeline, bool no_overwrite, uint32_t mip_index, uint32_t array_index, CubeFace face) = 0;
	virtual void Write(uint32_t row, uint32_t col,  const void *data, uint32_t size) = 0;
	virtual void WriteUnmap() = 0;
	
	virtual void ReadMap(GraphicPipeline *pipeline, uint32_t mip_index, uint32_t array_index, CubeFace face, bool wipe_cache=true) const = 0;
	virtual const void * Read(uint32_t row, uint32_t col) const = 0;
	virtual void ReadUnmap() const = 0;
	
	virtual void Update(
			GraphicPipeline *pipeline, 
			uint32_t mip_index, uint32_t array_index, CubeFace face,
			uint32_t left, uint32_t right,
			uint32_t top, uint32_t bottom,
			const void *data) = 0;

	virtual ShaderResource * AsShaderResource() const = 0;
};
	
}

#endif