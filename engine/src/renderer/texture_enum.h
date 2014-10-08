#ifndef TEXTURE_ENUM_H_
#define TEXTURE_ENUM_H_

#include <assert.h>

namespace s2 {


struct TextureEnum {
	enum TextureFormat {
		R8G8B8A8_UNORM
		//Add more later.
	};
	
	static unsigned int GetFormatSize(TextureFormat flag) {
		switch(flag) {
		case R8G8B8A8_UNORM:
			return 4;
		default:
			assert(false);
			break;
		}
	}
};
}


#endif		//TEXTURE_ENUM_H_