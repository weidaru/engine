#include "texture1d.h"

namespace s2 {

Texture1D::Option::Option(){
	width = 0;
	mip_level = 1;
	array_size = 1;
	format = TextureEnum::R8G8B8A8_UNORM;
	output_bind = TextureEnum::NOT_OUTPUT;
	input_bind = TextureEnum::SHADER_RESOURCE;
	map_behavior = RendererEnum::MAP_FORBIDDEN;
}



}