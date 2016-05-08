#ifndef D3D11_TEXTURE2D_H_
#define D3D11_TEXTURE2D_H_

#include "graphics/renderer/texture2d.h"
#include "d3d11_resource_view.h"

struct ID3D11Texture2D;
struct ID3D11RenderTargetView;

namespace s2 {

class D3D11MappedResource;

class D3D11GraphicResourceManager;

class D3D11Texture2D : public Texture2D {
public:
	D3D11Texture2D(D3D11GraphicResourceManager *_manager);

	virtual ~D3D11Texture2D();
	virtual bool Initialize(const Option &_option);
	virtual const Texture2D::Option & GetOption() const;
	
	virtual void WriteMap(GraphicPipeline *pipeline, bool no_overwrite, uint32_t mip_index, uint32_t array_index);
	virtual void Write(uint32_t row, uint32_t col,  const void *data, uint32_t size);
	virtual void WriteUnmap();
	
	virtual void ReadMap(GraphicPipeline *pipeline, uint32_t mip_index, uint32_t array_index, bool wipe_cache) const;
	virtual const void * Read(uint32_t row, uint32_t col) const;
	virtual void ReadUnmap() const;
	
	virtual void Update(
			GraphicPipeline *pipeline, 
			uint32_t mip_index, uint32_t array_index,
			uint32_t left, uint32_t right,
			uint32_t top, uint32_t bottom,
			const void *data);

	virtual D3D11RenderTarget * AsRenderTarget() const;
	virtual D3D11DepthStencil * AsDepthStencil() const;
	virtual D3D11ShaderResource * AsShaderResource() const;
	virtual D3D11UnorderedAccess * AsUnorderedAccess() const;

	/**********************D3D11 exclusive***********************/
	void InitAsBackBuffer(ID3D11Texture2D *_tex, ID3D11RenderTargetView *_rt_view, const Option &_option);
	ID3D11Texture2D * GetInternal() { return tex; }
	
private:
	void Check() const;
	void Clean();

private:
	Option option;
	D3D11GraphicResourceManager *manager;
	ID3D11Texture2D *tex;
	mutable D3D11MappedResource *mapped;

	D3D11RenderTarget *render_target;
	D3D11DepthStencil *depth_stencil;
	D3D11ShaderResource *shader_resource;
	D3D11UnorderedAccess *unordered_access;
};

}

#endif		//D3D11_TEXTURE2D_H_