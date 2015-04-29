#ifndef TEXTURE_ENUM_H_
#define TEXTURE_ENUM_H_

namespace s2 {


struct TextureEnum {
	enum OutputBind {
		NOT_OUTPUT,
		RENDER_TARGET,
		DEPTH_STENCIL
	};
	
	enum InputBind {
		NOT_INPUT,
		SHADER_RESOURCE
	};
};
}


#endif		//TEXTURE_ENUM_H_