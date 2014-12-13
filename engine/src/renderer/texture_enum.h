#ifndef TEXTURE_ENUM_H_
#define TEXTURE_ENUM_H_

namespace s2 {


struct TextureEnum {
	enum TextureFormat {
		R8G8B8A8_UNORM,
		D24_UNORM_S8_UINT,
		R32G32B32A32_FLOAT
		//Add more later.
	};
	
	enum OutputBind {
		NOT_OUTPUT,
		RENDER_TARGET,
		DEPTH_STENCIL
	};
	
	enum InputBind {
		NOT_INPUT,
		SHADER_RESOURCE
	};
	
	static unsigned int GetFormatSize(TextureFormat flag);
};
}


#endif		//TEXTURE_ENUM_H_