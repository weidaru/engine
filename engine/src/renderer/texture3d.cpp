#include "texture3d.h"


namespace s2 {

Texture3D::Option::Option(){
	width = 0;
	height = 0;
	depth = 0;
	mip_level = 1;
	array_size = 1;
	format = TextureEnum::R8G8B8A8_UNORM;
	output_bind = TextureEnum::NOT_OUTPUT;
	input_bind = TextureEnum::SHADER_RESOURCE;
	sample_size = 1;
	map_behavior = GeneralEnum::MAP_FORBIDDEN;
}

}