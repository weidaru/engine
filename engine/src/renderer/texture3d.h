#ifndef TEXTURE3D_H_
#define TEXTURE3D_H_

#include "resource.h"
#include "texture_enum.h"
#include "utils/s2string.h"

namespace s2 {

class Texture3D : public Resource {
public:
	struct Option {
		unsigned int 	width;
		unsigned int 	height;
		unsigned int 	depth;
		unsigned int 	mip_level;			//0 is full mipmap.
		unsigned int 	array_size;
		TextureEnum::TextureFormat 	format;
		unsigned int 	sample_size;
		bool is_dynamic;
		TextureEnum::OutputBind output_bind;
		TextureEnum::InputBind input_bind;
		void *data;
		
		Option();
	};

public:
	virtual				~Texture3D() {}
	virtual void 		Initialize(const Option &option) = 0;
	virtual void * 		Map() = 0;
	virtual void 		UnMap() = 0;
	virtual void 		GetOption(Option *option) = 0;		
};

}

#endif 		//TEXTURE3D_H_