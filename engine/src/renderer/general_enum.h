#ifndef GENERAL_ENUM_H_
#define GENERAL_ENUM_H_

namespace s2 {

struct GeneralEnum {
	enum MapBehavior {
		MAP_FORBIDDEN,
		MAP_WRITE_OCCASIONAL,
		MAP_WRITE_FREQUENT,
		MAP_READ,
		MAP_READ_WRITE
	};
};

}

#endif		//GENERAL_ENUM_H_
