#include "texture1d.h"

namespace s2 {

Texture1D::Option::Option(){
	width = 0;
	mip_level = 1;
	array_size = 1;
	format = TextureEnum::R8G8B8A8_UNORM;
	as_rendertarget = false;
	as_depthstencil = false;
	as_shaderresource = true;
	is_dynamic = false;
}



}