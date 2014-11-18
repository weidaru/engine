#ifndef D3D11_BUFFER_HELPER_H_
#define D3D11_BUFFER_HELPER_H_

#include "renderer/general_enum.h"
#include "utils/s2string.h"

struct D3D11_BUFFER_DESC;

namespace s2 {

class D3D11BufferHelper {
public:
	static void SetBufferDesc(D3D11_BUFFER_DESC *desc, unsigned int  byte_width, GeneralEnum::CPUAccess cpu_access);
	static void * Map(ID3D11DeviceContext *context, ID3D11Buffer *buffer, GeneralEnum::CPUAccess cpu_access, s2string *error);
	static void UnMap(ID3D11DeviceContext *context, ID3D11Buffer *buffer);
};

}

#endif		//D3D11_BUFFER_HELPER_H_