#ifndef TEXTURE2D_H_
#define TEXTURE2D_H_

#include "resource.h"
#include "texture_enum.h"
#include "renderer_enum.h"

namespace s2 {

class Texture2D : public Resource {
public:
	struct Option {
		unsigned int 	width;
		unsigned int 	height;
		unsigned int 	mip_level;				//0 is full mipmap.
		unsigned int 	array_size;
		TextureEnum::TextureFormat 	format;
		unsigned int 	sample_size;			//This is only useful for render target
		RendererEnum::ResourceWrite resource_write;
		TextureEnum::OutputBind output_bind;
		TextureEnum::InputBind input_bind;
		void *data;
		
		Option();
		
		static void SetAsDepthStencilBuffer(Option *option, unsigned int width, unsigned int height);
	};

public:
	virtual ~Texture2D() {}
	virtual void Initialize(const Option &option) = 0;
	virtual const Texture2D::Option & GetOption() const = 0;		
	
	virtual void WriteMap(bool is_partial_map, unsigned int mip_index, unsigned array_index) = 0;
	virtual void Write(unsigned int row, unsigned int col,  const void *data, unsigned int size) = 0;
	virtual void WriteUnmap() = 0;
	
	virtual void ReadMap(unsigned int mip_index, unsigned array_index, bool wipe_cache=true) = 0;
	virtual void * Read(unsigned int row, unsigned int col) = 0;
	virtual void ReadUnmap() = 0;
	
	virtual void Update(
			unsigned int left, unsigned int right,
			unsigned int top, unsigned int bottom,
			const void *data) = 0;
};

}

#endif 		//TEXTURE2D_H_