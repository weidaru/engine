#ifndef TEXTURE1D_H_
#define	TEXTURE1D_H_

#include "resource.h"
#include "texture_enum.h"

namespace s2 {

class Texture1D : public Resource {
public:
	struct Option {
		unsigned int 	width;
		unsigned int 	mips_level;
		unsigned int 	array_size;
		TextureFormat 	format;
		TextureBindFlag bind_flag;
		bool 			is_dynamic;
		
		Option(unsigned int w){
			width = w;
			mips_level = 1;
			array_size = 1;
			format = TextureFormat::R8G8B8A8_UNORM;
			bind_flag = TextureBindFlag::Shader;
			is_dynamic = false;
		}
	};

public:
	virtual				~Texture1D() {}
	virtual void 		Initialize(const Option &option) = 0;
	virtual void * 		Map() = 0;
	virtual void 		UnMap() = 0;
	virtual void 		GetOption(Option *option) = 0;		
};

}

#endif		//TEXTURE1D_H_