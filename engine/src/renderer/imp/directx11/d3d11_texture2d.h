#ifndef D3D11_TEXTURE2D_H_
#define D3D11_TEXTURE2D_H_

#include "renderer/texture2d.h"

struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11ShaderResourceView;

namespace s2 {

class D3D11MappedResource;

class D3D11GraphicResourceManager;

class D3D11Texture2D : public Texture2D {
	friend class D3D11GraphicResourceManager;

public:
	D3D11Texture2D(D3D11GraphicResourceManager *_manager);

	virtual ~D3D11Texture2D();
	virtual void Initialize(const Option &_option);
	virtual const Texture2D::Option & GetOption() const;
	
	virtual void Map(bool is_partial_map, unsigned int mip_index, unsigned array_index);
	virtual void Write(unsigned int row, unsigned int col,  const void *data, unsigned int size);
	virtual const void * Read(unsigned int row, unsigned int col) const;
	virtual void UnMap();
	
	virtual void Update(
			unsigned int left, unsigned int right,
			unsigned int top, unsigned int bottom,
			const void *data);

	/**********************D3D11 exclusive***********************/
	void InitAsBackBuffer(ID3D11Texture2D *_tex, ID3D11RenderTargetView *_rt_view, const Option &_option);
	ID3D11Texture2D * GetInternal() { return tex; }
	ID3D11RenderTargetView * GetRenderTargetView() { return rt_view; }
	ID3D11DepthStencilView * GetDepthStencilView() { return ds_view; }
	ID3D11ShaderResourceView * GetShaderResourceView() { return sr_view; }
	
private:
	void Clear();
	
private:
	D3D11GraphicResourceManager *manager;

	ID3D11Texture2D *tex;
	ID3D11DepthStencilView *ds_view;
	ID3D11RenderTargetView *rt_view;
	ID3D11ShaderResourceView  *sr_view;
	Option option;
	D3D11MappedResource *mapped;
};

}

#endif		//D3D11_TEXTURE2D_H_