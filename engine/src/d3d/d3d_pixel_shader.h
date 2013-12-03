#ifndef D3D_PIXEL_SHADER_H_
#define D3D_PIXEL_SHADER_H_

#include <string>

struct ID3D11PixelShader;
struct D3D11_SHADER_INPUT_BIND_DESC;

class D3DPixelShader {
public:
	D3DPixelShader();
	
	bool LoadFromFile(const std::string &file_name, const std::string &entry_func_name, std::string *error_msg);
	
	D3D11_SHADER_INPUT_BIND_DESC* GetBindInfo(const std::string &name);

	ID3D11PixelShader * GetInternal() { return ps_; }
	
private:
	void CreateConstantBufferReflection();
	void CreateResourceReflection();
	void CreateSamplerReflection();

private:
	ID3D11PixelShader *ps_;
};

#endif 		//DED_PIXEL_SHADER_H_