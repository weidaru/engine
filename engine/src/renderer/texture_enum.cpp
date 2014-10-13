#include "texture_enum.h"

#include <glog/logging.h>

namespace s2 {

unsigned int TextureEnum::GetFormatSize(TextureFormat flag) {
	switch(flag) {
	case R8G8B8A8_UNORM:
	case D24_UNORM_S8_UINT:
		return 4;
	default:
		CHECK(false);
		return 0;
		break;
	}
}

}