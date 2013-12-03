#ifndef D3D_SHADER_BIND_INFO_H_
#define D3D_SHADER_BIND_INFO_H_

#include <string>

class D3DConstantBufferBindInfo {
public:
	D3DConstantBufferBindInfo();
	~D3DConstantBufferBindInfo();

	int GetBindSlot();
	unsigned int GetSize();

	template<typename T>
	bool assign(void *data, const std::string &name, const T &sub_data);

	template<typename T>
	bool assign(void *data, int index, const T &sub_data);
};

class D3DShaderResourceBindInfo {
public:
	D3DShaderResourceBindInfo();
	~D3DShaderResourceBindInfo();

	int GetBindSlot();
};

class D3DSamplerBindInfo {
public:
	D3DSamplerBindInfo();
	~D3DSamplerBindInfo();
	int GetBindSlot();
};

#endif			//D3D_SHADER_BIND_INFO_H_