#ifndef TEXTURE3D_H_
#define TEXTURE3D_H_

#include "resource.h"
#include "texture_enum.h"

namespace s2 {

class Texture3D : public Resource {
public:
	struct Option {
		unsigned int 	width;
		unsigned int 	height;
		unsigned int 	deepth;
		unsigned int 	mips_level;
		unsigned int 	array_size;
		TextureFormat 	format;
		TextureBindFlag bind_flag;
		unsigned int 	sample_size;
		bool 			is_dynamic;
		
		Option(unsigned int w, unsigned int h, unsigned int d){
			width = w;
			height = h;
			depth = d;
			mips_level = 1;
			array_size = 1;
			format = TextureFormat::R8G8B8A8_UNORM;
			bind_flag = TextureBindFlag::Shader;
			sample_size = 1;
			is_dynamic = false;
		}
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