#ifndef RENDERER_ENUM_H_
#define RENDERER_ENUM_H_

namespace s2 {

struct RendererEnum {
	enum ResourceWrite {
		IMMUTABLE,
		CPU_WRITE_OCCASIONAL,
		CPU_WRITE_FREQUENT
	};
};

}

#endif		//RENDERER_ENUM_H_