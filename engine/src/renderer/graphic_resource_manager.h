#ifndef GRAPHIC_RESOURCE_MANAGER_H_
#define GRAPHIC_RESOURCE_MANAGER_H_

namespace s2 {

class Texture1D;
class Texture2D;
class Texture3D;
class Sampler;
class VertexBuffer;
class StreamOut;
class IndexBuffer;
class VertexShader;
class PixelShader;
class GeometryShader;

class GraphicResourceManager {
public:
	virtual ~GraphicResourceManager() {}
	
	//VertexBuffer
	virtual VertexBuffer * 		CreateVertexBuffer() = 0;
	virtual VertexBuffer * 		GetVertexBuffer(unsigned int id) = 0;
	virtual void 					RemoveVertexBuffer(unsigned int id) = 0;
	
	//StreamOut
	virtual StreamOut *		CreateStreamOut() = 0;
	virtual StreamOut *		GetStreamOut(unsigned int id) = 0;
	virtual void					RemoveStreamOut(unsigned int id) = 0;

	//IndexBuffer
	virtual IndexBuffer *		CreateIndexBuffer() = 0;
	virtual IndexBuffer * 		GetIndexBuffer(unsigned int id) = 0;
	virtual void 					RemoveIndexBuffer(unsigned int id) = 0;
	
	//Texture1D
	virtual Texture1D * 			CreateTexture1D() = 0;
	virtual Texture1D * 			GetTexture1D(unsigned int id) = 0;
	virtual void 					RemoveTexture1D(unsigned int id) = 0;
	
	//Texture2D
	virtual Texture2D * 			CreateTexture2D() = 0;
	virtual Texture2D * 			GetTexture2D(unsigned int id) = 0;
	virtual void 					RemoveTexture2D(unsigned int id) = 0;
	virtual Texture2D *			GetBackBuffer() = 0;
	
	//Texture3D
	virtual Texture3D * 			CreateTexture3D() = 0;
	virtual Texture3D * 			GetTexture3D(unsigned int id) = 0;
	virtual void 					RemoveTexture3D(unsigned int id) = 0;
	
	//Sampler
	virtual Sampler * 			CreateSampler() = 0;
	virtual Sampler * 			GetSampler(unsigned int id) = 0;
	virtual void 					RemoveSampler(unsigned int id) = 0;
	
	//VertexShader
	virtual VertexShader * 		CreateVertexShader() = 0;
	virtual VertexShader *		GetVertexShader(unsigned int id) = 0;
	virtual void 					RemoveVertexShader(unsigned int id) = 0;
	
	//PixelShader
	virtual PixelShader * 		CreatePixelShader() = 0;
	virtual PixelShader *		GetPixelShader(unsigned int id) = 0;
	virtual void 					RemovePixelShader(unsigned int id) = 0;

	//GeometryShader
	virtual GeometryShader * CreateGeometryShader() = 0;
	virtual GeometryShader * GetGeometryShader(unsigned int id) = 0;
	virtual void RemoveGeometryShader(unsigned int id) = 0;
	
};

}

#endif		//GRAPHIC_RESOURCE_MANAGER_H_
