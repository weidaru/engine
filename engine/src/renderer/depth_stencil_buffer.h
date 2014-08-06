#ifndef DEPTH_STENCIL_BUFFER_H_
#define DEPTH_STENCIL_BUFFER_H_

#include "resource.h"
#include "texture2d.h"

namespace s2 {

class DepthStencilBuffer : public Resource {
public:
	virtual ~DepthStencilBuffer() {}
	
	virtual void Initialize(const Texture2D::Option &option) = 0;
	virtual Texture2D * GetBufferData() = 0;
};

}

#endif		//DEPTH_STENCIL_BUFFER_H_