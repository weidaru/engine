#ifndef GENERAL_ENUM_H_
#define GENERAL_ENUM_H_

namespace s2 {

struct GeneralEnum {
	enum CPUAccess {
		CPU_NO_ACCESS,
		CPU_READ,
		CPU_WRITE,
		CPU_READ_WRITE
	};
}

}


#endif		//GENERAL_ENUM_H_
