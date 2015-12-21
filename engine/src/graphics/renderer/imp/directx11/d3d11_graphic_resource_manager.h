#ifndef D3D11_GRAPHIC_RESOURCE_MANAGER_H_
#define D3D11_GRAPHIC_RESOURCE_MANAGER_H_

#include "graphics/renderer/graphic_resource_manager.h"

#include "d3d11_texture2d.h"
#include "d3d11_texture_cube.h"
#include "d3d11_graphic_buffer.h"
#include "d3d11_vertex_shader.h"
#include "d3d11_pixel_shader.h"
#include "d3d11_sampler.h"
#include "d3d11_geometry_shader.h"
#include "d3d11_input_layout.h"


#include <map>

struct ID3D11Device;

namespace s2 {

/**
 * Right now, only single window is  used. So world is simple, single window, single swapchain, single backbuffer(We only use double buffering.)
 */
class D3D11GraphicResourceManager : public GraphicResourceManager {
public:
	D3D11GraphicResourceManager(ID3D11Device *_device);
	virtual ~D3D11GraphicResourceManager();
	
	//Buffer
	virtual D3D11GraphicBuffer * CreateGraphicBuffer() override;
	virtual D3D11GraphicBuffer * GetGraphicBuffer(uint32_t id) override;
	virtual void RemoveGraphicBuffer(uint32_t id) override;
	
	//Texture1D
	virtual Texture1D * CreateTexture1D() override;
	virtual Texture1D * GetTexture1D(uint32_t id) override;
	virtual void RemoveTexture1D(uint32_t id) override;
	
	//Texture2D
	virtual D3D11Texture2D * CreateTexture2D() override;
	virtual D3D11Texture2D * GetTexture2D(uint32_t id) override;
	virtual void RemoveTexture2D(uint32_t id) override;

	//TextureCube
	virtual D3D11TextureCube * CreateTextureCube() override;
	virtual D3D11TextureCube * GetTextureCube(uint32_t id) override;
	virtual void RemoveTextureCube(uint32_t id) override;
	
	//Texture3D
	virtual Texture3D * CreateTexture3D() override;
	virtual Texture3D * GetTexture3D(uint32_t id) override;
	virtual void RemoveTexture3D(uint32_t id) override;
	
	//Sampler
	virtual D3D11Sampler * CreateSampler() override;
	virtual D3D11Sampler * GetSampler(uint32_t id) override;
	virtual void RemoveSampler(uint32_t id) override;
 
	//VertexShader
	virtual D3D11VertexShader * CreateVertexShader() override;
	virtual D3D11VertexShader * GetVertexShader(uint32_t id) override;
	virtual void RemoveVertexShader(uint32_t id) override;
	
	//PixelShader
	virtual D3D11PixelShader * CreatePixelShader() override;
	virtual D3D11PixelShader * GetPixelShader(uint32_t id) override;
	virtual void RemovePixelShader(uint32_t id) override;

	//GeometryShader
	virtual D3D11GeometryShader * CreateGeometryShader() override;
	virtual D3D11GeometryShader * GetGeometryShader(uint32_t id) override;
	virtual void RemoveGeometryShader(uint32_t id) override;

	//InputLayout
	virtual D3D11InputLayout * CreateInputLayout() override;
	virtual D3D11InputLayout * GetInputLayout(uint32_t id) override;
	virtual void RemoveInputLayout(uint32_t) override;
	
	/****************D3D11 exclusive*****************/
public:
	ID3D11Device * GetDevice() { return device; }
	
private:	
	D3D11GraphicResourceManager(const D3D11GraphicResourceManager &);
	D3D11GraphicResourceManager & operator=(const D3D11GraphicResourceManager &);
	
	
private:
	ID3D11Device *device;
	
	std::map<uint32_t, D3D11GraphicBuffer *> buffer_map;
	std::map<uint32_t, Texture1D *> tex1d_map;
	std::map<uint32_t, D3D11Texture2D *> tex2d_map;
	std::map<uint32_t, D3D11TextureCube *> texcube_map;
	std::map<uint32_t, Texture3D *> tex3d_map;
	std::map<uint32_t, D3D11Sampler *> sampler_map;
	std::map<uint32_t, D3D11VertexShader *> vs_map;
	std::map<uint32_t, D3D11PixelShader *> ps_map;
	std::map<uint32_t, D3D11GeometryShader *> gs_map;
	std::map<uint32_t, D3D11InputLayout *> input_layout_map;
};


}



#endif 			//D3D11_GRAPHIC_RESOURCE_MANAGER_H_
