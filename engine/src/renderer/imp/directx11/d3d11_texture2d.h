#ifndef D3D11_TEXTURE2D_H_
#define D3D11_TEXTURE2D_H_

#include "renderer/texture2d.h"
#include "d3d11_resource_view.h"

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
	
	virtual void WriteMap(bool is_partial_map, unsigned int mip_index, unsigned array_index);
	virtual void Write(unsigned int row, unsigned int col,  const void *data, unsigned int size);
	virtual void WriteUnmap();
	
	virtual void ReadMap(unsigned int mip_index, unsigned array_index, bool wipe_cache) const;
	virtual const void * Read(unsigned int row, unsigned int col) const;
	virtual void ReadUnmap() const;
	
	virtual void Update(
			unsigned int left, unsigned int right,
			unsigned int top, unsigned int bottom,
			const void *data);

	virtual D3D11RenderTarget * AsRenderTarget() const;
	virtual D3D11DepthStencil * AsDepthStencil() const;
	virtual  D3D11ShaderResource * AsShaderResource() const;

	/**********************D3D11 exclusive***********************/
	void InitAsBackBuffer(ID3D11Texture2D *_tex, ID3D11RenderTargetView *_rt_view, const Option &_option);
	ID3D11Texture2D * GetInternal() { return tex; }
	
private:
	void Check() const;

private:
	Option option;
	D3D11GraphicResourceManager *manager;
	ID3D11Texture2D *tex;
	mutable D3D11MappedResource *mapped;

	D3D11RenderTarget *render_target;
	D3D11DepthStencil *depth_stencil;
	D3D11ShaderResource *shader_resource;

};

}

#endif		//D3D11_TEXTURE2D_H_