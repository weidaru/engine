#ifndef TEXTURE1D_H_
#define	TEXTURE1D_H_

#include "resource.h"
#include "utils/s2string.h"
#include "texture_enum.h"
#include "renderer_enum.h"
#include "resource_view.h"
#include "texture_data.h"

namespace s2 {
class GraphicPipeline;

class Texture1D : public Resource {
public:
	struct Option {
		uint32_t width;
		uint32_t mip_level;			//0 is full mipmap.
		uint32_t array_size;
		RendererEnum::Format format;
		TextureEnum::OutputBind output_bind;
		TextureEnum::InputBind input_bind;
		RendererEnum::ResourceWrite resource_write;
		TextureData data;
		
		Option() {
			width = 0;
			mip_level = 1;
			array_size = 1;
			format = RendererEnum::R8G8B8A8_UNORM;
			output_bind = TextureEnum::NOT_OUTPUT;
			input_bind = TextureEnum::SHADER_RESOURCE;
			resource_write = RendererEnum::IMMUTABLE;
		}
	};

public:
	virtual ~Texture1D() {}
	virtual void Initialize(const Option &option) = 0;
	virtual const Texture1D::Option & GetOption() const = 0;		
	
	virtual void WriteMap(GraphicPipeline *pipeline, bool no_overwrite, uint32_t mip_index, uint32_t array_index) = 0;
	virtual void Write(uint32_t index, const void *data, uint32_t size) = 0;
	virtual void WriteUnmap() = 0;
	
	virtual void Update(GraphicPipeline *pipeline, uint32_t mip_index, uint32_t array_index, const void *data, uint32_t size) = 0;
	
	virtual void ReadMap(GraphicPipeline *pipeline, uint32_t mip_index, uint32_t array_index, bool wipe_cache=true) = 0;
	virtual void * Read(uint32_t index) = 0;
	virtual void ReadUnmap() = 0;

	virtual const RenderTarget & AsRenderTarget() const = 0;
	virtual const DepthStencil & AsDepthStencil() const = 0;
	virtual const ShaderResource & AsShaderResource() const = 0;
};

}

#endif		//TEXTURE1D_H_