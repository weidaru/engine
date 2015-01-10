#ifndef RENDERER_ENUM_H_
#define RENDERER_ENUM_H_

namespace s2 {

struct RendererEnum {
	enum MapBehavior {
		MAP_FORBIDDEN,
		MAP_WRITE_OCCASIONAL,
		MAP_WRITE_FREQUENT,
		MAP_READ,
		MAP_READ_WRITE
	};
};

}

#endif		//RENDERER_ENUM_H_
