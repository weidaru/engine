#ifndef TEXTURE2D_H_
#define TEXTURE2D_H_

#include "resource.h"
#include "texture_enum.h"
#include "general_enum.h"

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
		GeneralEnum::MapBehavior map_behavior;
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
	
	virtual void Map(bool is_partial_map, unsigned int mip_index, unsigned array_index) = 0;
	virtual void Write(unsigned int row, unsigned int col,  const void *data, unsigned int size) = 0;
	virtual const void * Read(unsigned int row, unsigned int col) const = 0;
	virtual void UnMap() = 0;
	
	virtual void Update(
			unsigned int left, unsigned int right,
			unsigned int top, unsigned int bottom,
			const void *data) = 0;
};

}

#endif 		//TEXTURE2D_H_