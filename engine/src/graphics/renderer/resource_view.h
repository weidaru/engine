#ifndef RESOURCE_VIEW_H_
#define RESOURCE_VIEW_H_

#include "resource.h"

namespace s2 {

class UnorderedAccess {
public:
	virtual ~UnorderedAccess() {}
	virtual Resource *GetResource () = 0;
};

class RenderTarget {
public:
	virtual ~RenderTarget() {}
	virtual Resource *GetResource() = 0;
};

class DepthStencil {
public:
	virtual ~DepthStencil() {}
	virtual Resource *GetResource() = 0;
};

class VertexBuffer {
public:
	virtual ~VertexBuffer() {}
	virtual Resource *GetResource() = 0;
};

class IndexBuffer {
public:
	virtual ~IndexBuffer() {}
	virtual Resource *GetResource() = 0;
};

class ShaderResource {
public:
	virtual ~ShaderResource() {}
	virtual Resource *GetResource() = 0;
};

class StreamOut {
public:
	virtual ~StreamOut() {}
	virtual Resource *GetResource() = 0;
};

}


#endif			//RESOURCE_VIEW_H_