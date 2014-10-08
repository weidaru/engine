#ifndef GRAPHIC_RESOURCE_MANAGER_H_
#define GRAPHIC_RESOURCE_MANAGER_H_

#include "utils/s2string.h"

#include "texture1d.h"
#include "texture2d.h"
#include "texture3d.h"
#include "sampler.h"
#include "vertex_shader.h"

#include <stdint.h>

namespace s2 {

class VertexBuffer;
class IndexBuffer;
class DepthStencilBuffer;
class VertexShader;
class PixelShader;

class GraphicResourceManager {
public:
	virtual ~GraphicResourceManager() {}
	
	//VertexBuffer
	virtual VertexBuffer * 		CreateVertexBuffer(unsigned int size, const void* data, bool is_dynamic) = 0;
	virtual VertexBuffer * 		GetVertexBuffer(unsigned int id) = 0;
	virtual void 					RemoveVertexBuffer(unsigned int id) = 0;
	
	//IndexBuffer
	virtual IndexBuffer *		CreateIndexBuffer(unsigned int size, const uint32_t* data, bool is_dynamic) = 0;
	virtual IndexBuffer * 		GetIndexBuffer(unsigned int id) = 0;
	virtual void 					RemoveIndexBuffer(unsigned int id) = 0;
	
	//Texture1D
	virtual Texture1D * 			CreateTexture1D(const Texture1D::Option &option) = 0;
	virtual Texture1D * 			GetTexture1D(unsigned int id) = 0;
	virtual void 					RemoveTexture1D(unsigned int id) = 0;
	
	//Texture2D
	virtual Texture2D * 			CreateTexture2D(const Texture2D::Option &option) = 0;
	virtual Texture2D * 			GetTexture2D(unsigned int id) = 0;
	virtual void 					RemoveTexture2D(unsigned int id) = 0;
	virtual Texture2D *			GetBackBuffer() = 0;
	
	//Texture3D
	virtual Texture3D * 			CreateTexture3D(const Texture1D::Option &option) = 0;
	virtual Texture3D * 			GetTexture3D(unsigned int id) = 0;
	virtual void 					RemoveTexture3D(unsigned int id) = 0;
	
	//Sampler
	virtual Sampler * 			CreateSampler(const Texture1D::Option &option) = 0;
	virtual Sampler * 			GetSampler(unsigned int id) = 0;
	virtual void 					RemoveSampler(unsigned int id) = 0;
	
	//VertexShader
	virtual VertexShader * 		CreateVertexShader(const s2string &path, VertexShader::PrimitiveTopology topology) = 0;
	virtual VertexShader *		GetVertexShader(unsigned int id) = 0;
	virtual void 					RemoveVertexShader(unsigned int id) = 0;
	
	//PixelShader
	virtual PixelShader * 		CreatePixelShader(const s2string &path) = 0;
	virtual PixelShader *		GetPixelShader(unsigned int id) = 0;
	virtual void 					RemovePixelShader(unsigned int id) = 0;
	
	virtual void 					GetLastError(s2string *str) = 0;
};

}

#endif		//GRAPHIC_RESOURCE_MANAGER_H_
