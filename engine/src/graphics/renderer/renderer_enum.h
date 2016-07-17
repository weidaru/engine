#ifndef RENDERER_ENUM_H_
#define RENDERER_ENUM_H_

#include <stdint.h>

namespace s2 {

struct RendererBinding {
	enum {
		VERTEX_BUFFER     = 0x1,
		INDEX_BUFFER      = 0x2,
		//CONSTANT_BUFFER   = 0x4,
		SHADER_RESOURCE   = 0x8,
		STREAM_OUT     = 0x10,
		RENDER_TARGET     = 0x20,
		DEPTH_STENCIL     = 0x40,
		UNORDERED_ACCESS  = 0x80,
	};
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
	R32_UNIT,
	R32G32_FLOAT
	//Add more later.
};

static uint32_t GetFormatSize(RendererFormat flag) {
	switch (flag) {
	case RendererFormat::R8G8B8A8_UNORM:
	case RendererFormat::D24_UNORM_S8_UINT:
		return 4;
	case RendererFormat::R32G32_FLOAT:
		return 8;
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
