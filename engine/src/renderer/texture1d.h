#ifndef TEXTURE1D_H_
#define	TEXTURE1D_H_

#include "resource.h"
#include "utils/s2string.h"
#include "texture_enum.h"

namespace s2 {

class Texture1D : public Resource {
public:
	struct Option {
		unsigned int width;
		unsigned int mip_level;			//0 is full mipmap.
		unsigned int array_size;
		TextureEnum::TextureFormat format;
		bool as_rendertarget;
		bool as_depthstencil;
		bool as_shaderresource;
		bool is_dynamic;
		void *data;
		
		Option();
	};

public:
	virtual				~Texture1D() {}
	virtual bool 		Initialize(const Option &option) = 0;
	virtual void * 		Map() = 0;
	virtual void 		UnMap() = 0;
	virtual void 		GetOption(Option *option) = 0;		
	
	virtual void			GetLastError(s2string *str) = 0;
};

}

#endif		//TEXTURE1D_H_