#ifndef D3D11_GRAPHIC_RESOURCE_MANAGER_H_
#define D3D11_GRAPHIC_RESOURCE_MANAGER_H_

#include "graphics/renderer/graphic_resource_manager.h"

#include "d3d11_texture2d.h"
#include "d3d11_buffer.h"
#include "d3d11_vertex_shader.h"
#include "d3d11_pixel_shader.h"
#include "d3d11_sampler.h"
#include "d3d11_geometry_shader.h"

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
	virtual D3D11Buffer * CreateBuffer();
	virtual D3D11Buffer * GetBuffer(unsigned int id);
	virtual void RemoveBuffer(unsigned int id);
	
	//Texture1D
	virtual Texture1D * CreateTexture1D();
	virtual Texture1D * GetTexture1D(unsigned int id);
	virtual void RemoveTexture1D(unsigned int id);
	
	//Texture2D
	virtual D3D11Texture2D * CreateTexture2D();
	virtual D3D11Texture2D * GetTexture2D(unsigned int id);
	virtual void RemoveTexture2D(unsigned int id);
	virtual D3D11Texture2D * GetBackBuffer();
	
	//Texture3D
	virtual Texture3D * CreateTexture3D();
	virtual Texture3D * GetTexture3D(unsigned int id);
	virtual void RemoveTexture3D(unsigned int id);
	
	//Sampler
	virtual D3D11Sampler * CreateSampler();
	virtual D3D11Sampler * GetSampler(unsigned int id);
	virtual void RemoveSampler(unsigned int id);
 
	//VertexShader
	virtual D3D11VertexShader * CreateVertexShader();
	virtual D3D11VertexShader * GetVertexShader(unsigned int id);
	virtual void RemoveVertexShader(unsigned int id);
	
	//PixelShader
	virtual D3D11PixelShader * CreatePixelShader();
	virtual D3D11PixelShader * GetPixelShader(unsigned int id);
	virtual void RemovePixelShader(unsigned int id);

	//GeometryShader
	virtual D3D11GeometryShader * CreateGeometryShader();
	virtual D3D11GeometryShader * GetGeometryShader(unsigned int id);
	virtual void RemoveGeometryShader(unsigned int id);
	
	/****************D3D11 exclusive*****************/
public:
	ID3D11Device * GetDevice() { return device; }
	
private:	
	D3D11GraphicResourceManager(const D3D11GraphicResourceManager &);
	D3D11GraphicResourceManager & operator=(const D3D11GraphicResourceManager &);
	
	
private:
	ID3D11Device *device;
	
	std::map<unsigned int, D3D11Buffer *> buffer_map;
	std::map<unsigned int, Texture1D *> tex1d_map;
	std::map<unsigned int, D3D11Texture2D *> tex2d_map;
	std::map<unsigned int, Texture3D *> tex3d_map;
	std::map<unsigned int, D3D11Sampler *> sampler_map;
	std::map<unsigned int, D3D11VertexShader *> vs_map;
	std::map<unsigned int, D3D11PixelShader *> ps_map;
	std::map<unsigned int, D3D11GeometryShader *> gs_map;
};


}



#endif 			//D3D11_GRAPHIC_RESOURCE_MANAGER_H_
