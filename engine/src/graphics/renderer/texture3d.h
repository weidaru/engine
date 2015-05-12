#ifndef TEXTURE3D_H_
#define TEXTURE3D_H_

#include "resource.h"
#include "texture_enum.h"
#include "utils/s2string.h"
#include "renderer_enum.h"

namespace s2 {
class GraphicPipeline;

class Texture3D : public Resource {
public:
	struct Option {
		unsigned int 	width;
		unsigned int 	height;
		unsigned int 	depth;
		unsigned int 	mip_level;			//0 is full mipmap.
		unsigned int 	array_size;
		RendererEnum::Format 	format;
		unsigned int 	sample_size;
		RendererEnum::ResourceWrite resource_write;
		TextureEnum::OutputBind output_bind;
		TextureEnum::InputBind input_bind;
		void *data;
		
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
	
	virtual void WriteMap(GraphicPipeline *pipeline, unsigned int mip_index, unsigned array_index) = 0;
	virtual void Write(unsigned int row, unsigned int col, unsigned int depth, const void *data, unsigned int size) = 0;
	virtual void WriteUnmap() = 0;
	
	virtual void Update(
			GraphicPipeline *pipeline, 
			unsigned int left, unsigned int right,
			unsigned int top, unsigned int bottom,
			unsigned int front, unsigned int back,
			const void *data) = 0;
			
	virtual void ReadMap(GraphicPipeline *pipeline, unsigned int mip_index, unsigned array_index, bool wipe_cache=true) = 0;
	virtual void Read(unsigned int row, unsigned int col, unsigned int depth) = 0;
	virtual void ReadUnmap() = 0;

	virtual const RenderTarget & AsRenderTarget() const = 0;
	virtual const DepthStencil & AsDepthStencil() const = 0;
	virtual const ShaderResource & AsShaderResource() const = 0;
};

}

#endif 		//TEXTURE3D_H_