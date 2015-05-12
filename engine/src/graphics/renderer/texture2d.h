#ifndef TEXTURE2D_H_
#define TEXTURE2D_H_

#include "resource.h"
#include "texture_enum.h"
#include "renderer_enum.h"

namespace s2 {

class RenderTarget;
class DepthStencil;
class ShaderResource;
class GraphicPipeline;

class Texture2D : public Resource {
public:
	struct Option {
		unsigned int 	width;
		unsigned int 	height;
		unsigned int 	mip_level;				//0 is full mipmap.
		unsigned int 	array_size;
		RendererEnum::Format 	format;
		unsigned int 	sample_size;			//This is only useful for render target
		RendererEnum::ResourceWrite resource_write;
		TextureEnum::OutputBind output_bind;
		TextureEnum::InputBind input_bind;
		void *data;
		
		Option() {
			width = 0;
			height = 0;
			mip_level = 1;
			array_size = 1;
			format = RendererEnum::R8G8B8A8_UNORM;
			output_bind = TextureEnum::NOT_OUTPUT;
			input_bind = TextureEnum::SHADER_RESOURCE;
			sample_size = 1;
			resource_write = RendererEnum::CPU_WRITE_OCCASIONAL;
			data = 0;
		}
		
		static void SetAsDepthStencilBuffer(Option *option, unsigned int width, unsigned int height) {
			option->width = width;
			option->height = height;
			option->mip_level = 1;
			option->array_size = 1;
			option->format = RendererEnum::D24_UNORM_S8_UINT;
			option->output_bind = TextureEnum::DEPTH_STENCIL;
			option->input_bind = TextureEnum::NOT_INPUT;
			option->sample_size = 1;
			option->resource_write = RendererEnum::CPU_WRITE_OCCASIONAL;
			option->data = 0;
		}
	};

public:
	virtual ~Texture2D() {}
	virtual void Initialize(const Option &option) = 0;
	virtual const Texture2D::Option & GetOption() const = 0;		
	
	virtual void WriteMap(GraphicPipeline *pipeline, bool no_overwrite, unsigned int mip_index, unsigned array_index) = 0;
	virtual void Write(unsigned int row, unsigned int col,  const void *data, unsigned int size) = 0;
	virtual void WriteUnmap() = 0;
	
	virtual void ReadMap(GraphicPipeline *pipeline, unsigned int mip_index, unsigned array_index, bool wipe_cache=true) const = 0;
	virtual const void * Read(unsigned int row, unsigned int col) const = 0;
	virtual void ReadUnmap() const = 0;
	
	virtual void Update(
			GraphicPipeline *pipeline, 
			unsigned int left, unsigned int right,
			unsigned int top, unsigned int bottom,
			const void *data) = 0;

	virtual RenderTarget * AsRenderTarget() const = 0;
	virtual DepthStencil * AsDepthStencil() const = 0;
	virtual ShaderResource * AsShaderResource() const = 0;
};

}

#endif 		//TEXTURE2D_H_