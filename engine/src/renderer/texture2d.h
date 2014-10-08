#ifndef TEXTURE2D_H_
#define TEXTURE2D_H_

#include "resource.h"
#include "texture_enum.h"
#include "utils/s2string.h"

namespace s2 {

class Texture2D : public Resource {
public:
	struct Option {
		unsigned int 	width;
		unsigned int 	height;
		unsigned int 	mip_level;				//0 is full mipmap.
		unsigned int 	array_size;
		TextureEnum::TextureFormat 	format;
		unsigned int 	sample_size;
		bool is_dynamic;
		bool as_rendertarget;
		bool as_depthstencil;
		bool as_shaderresource;
		void *data;
		
		Option(){
			width = 0;
			height = 0;
			mip_level = 1;
			array_size = 1;
			format = TextureEnum::R8G8B8A8_UNORM;
			as_rendertarget = false;
			as_depthstencil = false;
			as_shaderresource = true;
			sample_size = 1;
			is_dynamic = false;
			data = 0;
		}
	};

public:
	virtual				~Texture2D() {}
	virtual bool 		Initialize(const Option &option) = 0;
	virtual void * 		Map() = 0;
	virtual void 		UnMap() = 0;
	virtual void 		GetOption(Option *option) = 0;		
	
	virtual void 		GetLastError(s2string *str) = 0;
};

}

#endif 		//TEXTURE2D_H_