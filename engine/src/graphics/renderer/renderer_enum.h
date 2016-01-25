#ifndef RENDERER_ENUM_H_
#define RENDERER_ENUM_H_

#include <stdint.h>

namespace s2 {

enum struct RendererOutputBind {
	NOT_OUTPUT,
	RENDER_TARGET,
	DEPTH_STENCIL
};

enum struct RendererInputBind {
	NOT_INPUT,
	SHADER_RESOURCE,
	UNORDERED_ACCESS
};

enum struct RendererResourceWrite {
	IMMUTABLE,
	CPU_WRITE_OCCASIONAL,
	CPU_WRITE_FREQUENT
};

enum struct RendererFormat {
	R8G8B8A8_UNORM,
	D24_UNORM_S8_UINT,
	R32G32B32A32_FLOAT,
	R32_UNIT
	//Add more later.
};

static uint32_t GetFormatSize(RendererFormat flag) {
	switch (flag) {
	case RendererFormat::R8G8B8A8_UNORM:
	case RendererFormat::D24_UNORM_S8_UINT:
		return 4;
	case RendererFormat::R32G32B32A32_FLOAT:
		return 16;
	default:
		return 0;
		break;
	}
	return 0;
}

}

#endif		//RENDERER_ENUM_H_
