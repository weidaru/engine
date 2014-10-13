#include "texture3d.h"


namespace s2 {

Texture3D::Option::Option(){
	width = 0;
	height = 0;
	depth = 0;
	mip_level = 1;
	array_size = 1;
	format = TextureEnum::R8G8B8A8_UNORM;
	as_rendertarget = false;
	as_depthstencil = false;
	as_shaderresource = true;
	sample_size = 1;
	is_dynamic = false;
}

}