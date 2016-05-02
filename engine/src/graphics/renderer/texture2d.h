#ifndef TEXTURE2D_H_
#define TEXTURE2D_H_

#include "resource.h"
#include "renderer_enum.h"
#include "texture_data.h"

namespace s2 {

class UnorderedAccess;
class RenderTarget;
class DepthStencil;
class ShaderResource;
class GraphicPipeline;
class TextureData;

class Texture2D : public Resource {
public:
	struct Option {
		uint32_t 	width;
		uint32_t 	height;
		uint32_t 	mip_level;				//0 is full mipmap.
		uint32_t 	array_size;
		RendererFormat format;
		uint32_t 	sample_size;			//This is only useful for render target
		RendererResourceWrite resource_write;
		RendererOutputBind output_bind;
		RendererInputBind input_bind;
		TextureData data;
		
		Option() {
			width = 0;
			height = 0;
			mip_level = 1;
			array_size = 1;
			format = RendererFormat::R8G8B8A8_UNORM;
			output_bind = RendererOutputBind::NOT_OUTPUT;
			input_bind = RendererInputBind::SHADER_RESOURCE;
			sample_size = 1;
			resource_write = RendererResourceWrite::CPU_WRITE_OCCASIONAL;
		}
		
		static void SetAsDepthStencilBuffer(Option *option, uint32_t width, uint32_t height) {
			option->width = width;
			option->height = height;
			option->mip_level = 1;
			option->array_size = 1;
			option->format = RendererFormat::D24_UNORM_S8_UINT;
			option->output_bind = RendererOutputBind::DEPTH_STENCIL;
			option->input_bind = RendererInputBind::NOT_INPUT;
			option->sample_size = 1;
			option->resource_write = RendererResourceWrite::CPU_WRITE_OCCASIONAL;
			option->data.Reset(1, 1);
		}
	};

public:
	virtual ~Texture2D() {}
	virtual void Initialize(const Option &option) = 0;
	virtual const Texture2D::Option & GetOption() const = 0;		
	
	virtual void WriteMap(GraphicPipeline *pipeline, bool no_overwrite, uint32_t mip_index, uint32_t array_index) = 0;
	virtual void Write(uint32_t row, uint32_t col,  const void *data, uint32_t size) = 0;
	virtual void WriteUnmap() = 0;
	
	virtual void ReadMap(GraphicPipeline *pipeline, uint32_t mip_index, uint32_t array_index, bool wipe_cache=true) const = 0;
	virtual const void * Read(uint32_t row, uint32_t col) const = 0;
	virtual void ReadUnmap() const = 0;
	
	virtual void Update(
			GraphicPipeline *pipeline, 
			uint32_t mip_index, uint32_t array_index,
			uint32_t left, uint32_t right,
			uint32_t top, uint32_t bottom,
			const void *data) = 0;

	virtual RenderTarget * AsRenderTarget() const = 0;
	virtual DepthStencil * AsDepthStencil() const = 0;
	virtual ShaderResource * AsShaderResource() const = 0;
	virtual UnorderedAccess * AsUnorderedAccess() const = 0;
};

}

#endif 		//TEXTURE2D_H_