#ifndef GRAPHIC_RESOURCE_MANAGER_H_
#define GRAPHIC_RESOURCE_MANAGER_H_

#include <stdint.h>

namespace s2 {

class Texture1D;
class Texture2D;
class TextureCube;
class Texture3D;
class Sampler;
class GraphicBuffer;
class VertexShader;
class PixelShader;
class GeometryShader;

class GraphicResourceManager {
public:
	virtual ~GraphicResourceManager() {}
	
	//VertexBuffer
	virtual GraphicBuffer * CreateGraphicBuffer() = 0;
	virtual GraphicBuffer * GetGraphicBuffer(uint32_t id) = 0;
	virtual void RemoveGraphicBuffer(uint32_t id) = 0;
	
	//Texture1D
	virtual Texture1D * CreateTexture1D() = 0;
	virtual Texture1D * GetTexture1D(uint32_t id) = 0;
	virtual void RemoveTexture1D(uint32_t id) = 0;
	
	//Texture2D
	virtual Texture2D * CreateTexture2D() = 0;
	virtual Texture2D * GetTexture2D(uint32_t id) = 0;
	virtual void RemoveTexture2D(uint32_t id) = 0;

	//TextureCube
	virtual TextureCube * CreateTextureCube() = 0;
	virtual TextureCube * GetTextureCube(uint32_t id) = 0;
	virtual void RemoveTextureCube(uint32_t id) = 0;
	
	//Texture3D
	virtual Texture3D * CreateTexture3D() = 0;
	virtual Texture3D * GetTexture3D(uint32_t id) = 0;
	virtual void RemoveTexture3D(uint32_t id) = 0;
	
	//Sampler
	virtual Sampler * CreateSampler() = 0;
	virtual Sampler * GetSampler(uint32_t id) = 0;
	virtual void RemoveSampler(uint32_t id) = 0;
	
	//VertexShader
	virtual VertexShader * CreateVertexShader() = 0;
	virtual VertexShader * GetVertexShader(uint32_t id) = 0;
	virtual void RemoveVertexShader(uint32_t id) = 0;
	
	//PixelShader
	virtual PixelShader * CreatePixelShader() = 0;
	virtual PixelShader * GetPixelShader(uint32_t id) = 0;
	virtual void RemovePixelShader(uint32_t id) = 0;

	//GeometryShader
	virtual GeometryShader * CreateGeometryShader() = 0;
	virtual GeometryShader * GetGeometryShader(uint32_t id) = 0;
	virtual void RemoveGeometryShader(uint32_t id) = 0;
	
};

}

#endif		//GRAPHIC_RESOURCE_MANAGER_H_
