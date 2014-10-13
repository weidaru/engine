#ifndef TEXTURE_ENUM_H_
#define TEXTURE_ENUM_H_

namespace s2 {


struct TextureEnum {
	enum TextureFormat {
		R8G8B8A8_UNORM,
		D24_UNORM_S8_UINT
		//Add more later.
	};
	
	static unsigned int GetFormatSize(TextureFormat flag);
};
}


#endif		//TEXTURE_ENUM_H_