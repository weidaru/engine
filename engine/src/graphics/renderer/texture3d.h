#ifndef TEXTURE3D_H_
#define TEXTURE3D_H_

#include "resource.h"
#include "texture_enum.h"
#include "utils/s2string.h"
#include "renderer_enum.h"
#include "texture_data.h"

namespace s2 {
class GraphicPipeline;

class Texture3D : public Resource {
public:
	struct Option {
		uint32_t 	width;
		uint32_t 	height;
		uint32_t 	depth;
		uint32_t 	mip_level;			//0 is full mipmap.
		uint32_t 	array_size;
		RendererEnum::Format 	format;
		uint32_t 	sample_size;
		RendererEnum::ResourceWrite resource_write;
		TextureEnum::OutputBind output_bind;
		TextureEnum::InputBind input_bind;
		TextureData data;
		
		Option() {
			width = 0;
			height = 0;
			depth = 0;
			mip_level = 1;
			array_size = 1;
			format = RendererEnum::R8G8B8A8_UNORM;
			output_bind = TextureEnum::NOT_OUTPUT;
			input_bind = TextureEnum::SHADER_RESOURCE;
			sample_size = 1;
			resource_write = RendererEnum::IMMUTABLE;
		}
	};

public:
	virtual ~Texture3D() {}
	virtual void Initialize(const Option &option) = 0;
	virtual const Texture3D::Option & GetOption() const = 0;		
	
	virtual void WriteMap(GraphicPipeline *pipeline, uint32_t mip_index, uint32_t array_index) = 0;
	virtual void Write(uint32_t row, uint32_t col, uint32_t depth, const void *data, uint32_t size) = 0;
	virtual void WriteUnmap() = 0;
	
	virtual void Update(
			GraphicPipeline *pipeline, 
			uint32_t mip_index, uint32_t array_index,
			uint32_t left, uint32_t right,
			uint32_t top, uint32_t bottom,
			uint32_t front, uint32_t back,
			const void *data) = 0;
			
	virtual void ReadMap(GraphicPipeline *pipeline, uint32_t mip_index, uint32_t array_index, bool wipe_cache=true) = 0;
	virtual void Read(uint32_t row, uint32_t col, uint32_t depth) = 0;
	virtual void ReadUnmap() = 0;

	virtual const RenderTarget & AsRenderTarget() const = 0;
	virtual const DepthStencil & AsDepthStencil() const = 0;
	virtual const ShaderResource & AsShaderResource() const = 0;
};

}

#endif 		//TEXTURE3D_H_