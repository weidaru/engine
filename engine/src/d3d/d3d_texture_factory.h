#ifndef D3D_TEXTURE_FACTORY_H_
#define D3D_TEXTURE_FACTORY_H_

#include <string>
#include <map>

struct ID3D11Device;

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct D3DTexture2D {
	ID3D11Texture2D *texture;
	ID3D11ShaderResourceView *sr_view;
	ID3D11RenderTargetView *rt_view;

	D3DTexture2D();
	~D3DTexture2D();
};

struct D3DX11_IMAGE_LOAD_INFO;

/*!
 Produce texture useful with the shader. 
 It can be bound both as shader resource and render target, which depends on the actual value of view pointer.
*/
class D3DTextureFactory {
private:
	typedef std::map<std::string, D3DTexture2D *> Texture2DMap;

public:
	D3DTextureFactory(ID3D11Device *device);
	~D3DTextureFactory();

	//D3DX11_IMAGE_LOAD_INFO deprecated in windows 8 ???
	D3DTexture2D * CreateTexture2DFromFile(const std::string &name, const std::string &file_path, const D3DX11_IMAGE_LOAD_INFO* info = NULL);
	D3DTexture2D * GetTexture2D(const std::string &name);
	D3DTexture2D * BindShaderResource(D3DTexture2D *texture);
	D3DTexture2D * BindRenderTarget(D3DTexture2D *texture);

	void RemoveTexture2D(const std::string &name);

private:
	Texture2DMap data_2d_;
	ID3D11Device *device_;
};

#endif			//D3D_TEXTURE_FACTORY_H_