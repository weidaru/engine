#ifndef GRAPHIC_RESOURCE_MANAGER_H_
#define GRAPHIC_RESOURCE_MANAGER_H_

#include "utils/s2string.h"

#include "texture1d.h"
#include "texture2d.h"
#include "texture3d.h"
#include "sampler.h"

namespace s2 {

class VertexBuffer;
class IndexBuffer;
class DepthStencilBuffer;

class GraphicResourceManager {
public:
	virtual ~GraphicResourceManager() {}
	
	//VertexBuffer
	virtual VertexBuffer * 			CreateVertexBuffer(unsigned int size, const void* data, bool is_dynamic) = 0;
	virtual VertexBuffer * 			GetVertexBuffer(unsigned int id) = 0;
	virtual VertexBuffer * 			GetVertexBUffer(const s2string &name) = 0;
	virtual void 					RemoveVertexBuffer(unsigned int id) = 0;
	
	//IndexBuffer
	virtual IndexBuffer *			CreateVertexBuffer(unsigned int size, const void* data, bool is_dynamic) = 0;
	virtual IndexBuffer * 			GetVertexBuffer(unsigned int id) = 0;
	virtual IndexBuffer * 			GetVertexBUffer(const s2string &name) = 0;
	virtual void 					RemoveIndexBuffer(unsigned int id) = 0;
	
	//Texture1D
	virtual Texture1D * 			CreateTexture1D(const Texture1D::Option &option) = 0;
	virtual Texture1D * 			GetTexture1D(unsigned int id) = 0;
	virtual Texture1D * 			GetTexture1D(const s2string &name) = 0;
	virtual void 					RemoveTexture1D(unsigned int id) = 0;
	
	//Texture2D
	virtual Texture2D * 			CreateTexture2D(const Texture2D::Option &option) = 0;
	virtual Texture2D * 			GetTexture2D(unsigned int id) = 0;
	virtual Texture2D * 			GetTexture2D(const s2string &name) = 0;
	virtual void 					RemoveTexture2D(unsigned int id) = 0;
	virtual Texture2D *				GetBackBuffer() = 0;
	
	//Texture3D
	virtual Texture3D * 			CreateTexture3D(const Texture1D::Option &option) = 0;
	virtual Texture3D * 			GetTexture3D(unsigned int id) = 0;
	virtual Texture3D * 			GetTexture3D(const s2string &name) = 0;
	virtual void 					RemoveTexture3D(unsigned int id) = 0;
	
	//Sampler
	virtual Sampler * 				CreateSampler(const Texture1D::Option &option) = 0;
	virtual Sampler * 				GetSampler(unsigned int id) = 0;
	virtual Sampler * 				GetSampler(const s2string &name) = 0;
	virtual void 					RemoveSampler(unsigned int id) = 0;
	
	//DepthStencilBuffer
	virtual DepthStencilBuffer * 	CreateDepthStencilBuffer(const Texture2D::Option &option) = 0;
	virtual DepthStencilBuffer * 	GetDepthStencilBuffer(unsigned int id) = 0;
	virtual DepthStencilBuffer * 	GetDepthStencilBuffer(const s2string &name) = 0;
	virtual void 					RemoveDepthStencilBuffer(unsigned int id) = 0;
	
	//
	
};

}

#endif		//GRAPHIC_RESOURCE_MANAGER_H_