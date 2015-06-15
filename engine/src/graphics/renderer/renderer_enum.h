#ifndef RENDERER_ENUM_H_
#define RENDERER_ENUM_H_

#include <stdint.h>

namespace s2 {

struct RendererEnum {
	enum ResourceWrite {
		IMMUTABLE,
		CPU_WRITE_OCCASIONAL,
		CPU_WRITE_FREQUENT
	};

	enum Format {
		R8G8B8A8_UNORM,
		D24_UNORM_S8_UINT,
		R32G32B32A32_FLOAT,
		R32_UNIT
		//Add more later.
	};

	static uint32_t GetFormatSize(Format flag) {
		switch (flag) {
		case R8G8B8A8_UNORM:
		case D24_UNORM_S8_UINT:
			return 4;
		case R32G32B32A32_FLOAT:
			return 16;
		default:
			return 0;
			break;
		}
		return 0;
	}
};

}

#endif		//RENDERER_ENUM_H_
