#ifndef TEXTURE1D_H_
#define	TEXTURE1D_H_

#include "resource.h"
#include "utils/s2string.h"
#include "texture_enum.h"
#include "mappable.h"

namespace s2 {

class Texture1D : public Resource, public Mappable {
public:
	struct Option {
		unsigned int width;
		unsigned int mip_level;			//0 is full mipmap.
		unsigned int array_size;
		TextureEnum::TextureFormat format;
		TextureEnum::OutputBind output_bind;
		TextureEnum::InputBind input_bind;
		GeneralEnum::MapBehavior map_behavior;
		void *data;
		
		Option();
	};

public:
	virtual				~Texture1D() {}
	virtual void 		Initialize(const Option &option) = 0;
	virtual void 		GetOption(Option *option) = 0;		
};

}

#endif		//TEXTURE1D_H_