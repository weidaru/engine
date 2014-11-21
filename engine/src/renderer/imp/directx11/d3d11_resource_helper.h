#ifndef D3D11_RESOURCE_HELPER_H_
#define D3D11_RESOURCE_HELPER_H_

#include "renderer/general_enum.h"
#include "utils/s2string.h"

struct D3D11_BUFFER_DESC;
struct ID3D11Resource;

namespace s2 {

class D3D11ResourceHelper {
public:
	static void SetBufferDesc(D3D11_BUFFER_DESC *desc, unsigned int  byte_width, GeneralEnum::MapBehavior map_behavior);
};

}

#endif		//D3D11_RESOURCE_HELPER_H_