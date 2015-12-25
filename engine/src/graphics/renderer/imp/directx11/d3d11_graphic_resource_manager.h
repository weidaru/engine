#ifndef D3D11_GRAPHIC_RESOURCE_MANAGER_H_
#define D3D11_GRAPHIC_RESOURCE_MANAGER_H_

#include "graphics/renderer/graphic_resource_manager.h"

#include <map>

struct ID3D11Device;

namespace s2 {

class D3D11Texture2D;
class D3D11TextureCube;
class D3D11GraphicBuffer;
class D3D11ShaderBytecode;
class D3D11VertexShader;
class D3D11PixelShasder;
class D3D11GeometryShasder;
class D3D11Sampler;
class D3D11InputLayout;

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
	virtual Texture2D * CreateTexture2D() override;
	virtual Texture2D * GetTexture2D(uint32_t id) override;
	virtual void RemoveTexture2D(uint32_t id) override;

	//TextureCube
	virtual TextureCube * CreateTextureCube() override;
	virtual TextureCube * GetTextureCube(uint32_t id) override;
	virtual void RemoveTextureCube(uint32_t id) override;
	
	//Texture3D
	virtual Texture3D * CreateTexture3D() override;
	virtual Texture3D * GetTexture3D(uint32_t id) override;
	virtual void RemoveTexture3D(uint32_t id) override;
	
	//Sampler
	virtual Sampler * CreateSampler() override;
	virtual Sampler * GetSampler(uint32_t id) override;
	virtual void RemoveSampler(uint32_t id) override;
 
	//ShaderBytecode
	virtual ShaderBytecode * CreateShaderBytecode() override;
	virtual ShaderBytecode * GetShaderBytecode(uint32_t id) override;
	virtual void RemoveShaderBytecode(uint32_t id) override;

	//VertexShader
	virtual VertexShader * CreateVertexShader() override;
	virtual VertexShader * GetVertexShader(uint32_t id) override;
	virtual void RemoveVertexShader(uint32_t id) override;
	
	//PixelShader
	virtual PixelShader * CreatePixelShader() override;
	virtual PixelShader * GetPixelShader(uint32_t id) override;
	virtual void RemovePixelShader(uint32_t id) override;

	//GeometryShader
	virtual GeometryShader * CreateGeometryShader() override;
	virtual GeometryShader * GetGeometryShader(uint32_t id) override;
	virtual void RemoveGeometryShader(uint32_t id) override;

	//InputLayout
	virtual InputLayout * CreateInputLayout() override;
	virtual InputLayout * GetInputLayout(uint32_t id) override;
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
	std::map<uint32_t, D3D11ShaderBytecode *> bytecode_map;
	std::map<uint32_t, D3D11VertexShader *> vs_map;
	std::map<uint32_t, D3D11PixelShader *> ps_map;
	std::map<uint32_t, D3D11GeometryShader *> gs_map;
	std::map<uint32_t, D3D11InputLayout *> input_layout_map;
};


}



#endif 			//D3D11_GRAPHIC_RESOURCE_MANAGER_H_
