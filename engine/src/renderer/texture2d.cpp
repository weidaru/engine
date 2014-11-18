#include "texture2d.h"


namespace s2 {

Texture2D::Option::Option(){
	width = 0;
	height = 0;
	mip_level = 1;
	array_size = 1;
	format = TextureEnum::R8G8B8A8_UNORM;
	output_bind = TextureEnum::NOT_OUTPUT;
	input_bind = TextureEnum::SHADER_RESOURCE;
	sample_size = 1;
	cpu_access = GeneralEnum::CPU_NO_ACCESS;
	data = 0;
}

void Texture2D::Option::SetAsDepthStencilBuffer(Option *option, unsigned int width, unsigned int height) {
	option->width = width;
	option->height = height;
	option->mip_level = 1;
	option->array_size= 1;
	option->format = TextureEnum::D24_UNORM_S8_UINT;
	option->output_bind = TextureEnum::DEPTH_STENCIL;
	option->input_bind= TextureEnum::NOT_INPUT;
	option->sample_size = 1;
	option->is_dynamic = false;
	option->data = 0;
}

}