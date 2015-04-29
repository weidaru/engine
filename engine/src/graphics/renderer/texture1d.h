#ifndef TEXTURE1D_H_
#define	TEXTURE1D_H_

#include "resource.h"
#include "utils/s2string.h"
#include "texture_enum.h"
#include "renderer_enum.h"
#include "resource_view.h"

namespace s2 {

class Texture1D : public Resource {
public:
	struct Option {
		unsigned int width;
		unsigned int mip_level;			//0 is full mipmap.
		unsigned int array_size;
		RendererEnum::Format format;
		TextureEnum::OutputBind output_bind;
		TextureEnum::InputBind input_bind;
		RendererEnum::ResourceWrite resource_write;
		void *data;
		
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
	
	virtual void WriteMap(bool no_overwrite, unsigned int mip_index, unsigned array_index) = 0;
	virtual void Write(unsigned int index, const void *data, unsigned int size) = 0;
	virtual void WriteUnmap() = 0;
	
	virtual void Update(unsigned int index, const void *data, unsigned int size) = 0;
	
	virtual void ReadMap(unsigned int mip_index, unsigned array_index, bool wipe_cache=true) = 0;
	virtual void * Read(unsigned int index) = 0;
	virtual void ReadUnmap() = 0;

	virtual const RenderTarget & AsRenderTarget() const = 0;
	virtual const DepthStencil & AsDepthStencil() const = 0;
	virtual const ShaderResource & AsShaderResource() const = 0;
};

}

#endif		//TEXTURE1D_H_