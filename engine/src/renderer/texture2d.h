#ifndef TEXTURE2D_H_
#define TEXTURE2D_H_

#include "resource.h"
#include "texture_enum.h"
#include "general_enum.h"
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
		unsigned int 	sample_size;			//This is only useful for render target
		GeneralEnum::CPUAccess cpu_access;
		TextureEnum::OutputBind output_bind;
		TextureEnum::InputBind input_bind;
		void *data;
		
		Option();
		
		static void SetAsDepthStencilBuffer(Option *option, unsigned int width, unsigned int height);
	};

public:
	virtual				~Texture2D() {}
	virtual void 		Initialize(const Option &option) = 0;
	virtual void * 		Map() = 0;
	virtual void 		UnMap() = 0;
	virtual void 		GetOption(Option *option) = 0;		
};

}

#endif 		//TEXTURE2D_H_