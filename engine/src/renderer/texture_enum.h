#ifndef TEXTURE_ENUM_H_
#define TEXTURE_ENUM_H_

namespace s2 {
	enum TextureFormat {
		R8G8B8A8_UNORM
		//Add more later.
	};

	enum TextureBindFlag {
		SHADER 			= 0x01,
		RENDER_TARGET 	= 0x02,
		DEPTH_STENCIL 	= 0x04
	};
}


#endif		//TEXTURE_ENUM_H_