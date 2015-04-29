#ifndef D3D11_RESOURCE_VIEW_H_
#define D3D11_RESOURCE_VIEW_H_

#include "graphics/renderer/resource_view.h"
#include "utils/non_copyable.h"

#include "d3d11_buffer.h"

struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11ShaderResourceView;
struct ID3D11Buffer;

namespace s2 {

class D3D11RenderTarget : public RenderTarget, public NonCopyable {
public:
	D3D11RenderTarget(Resource *_resource, ID3D11RenderTargetView *_view);
	virtual ~D3D11RenderTarget();
	virtual Resource *GetResource()  { return resource; }

	ID3D11RenderTargetView * GetRenderTargetView() { return view; }

private:
	Resource *resource;
	ID3D11RenderTargetView *view;
};

class D3D11DepthStencil : public DepthStencil, public NonCopyable {
public:
	D3D11DepthStencil(Resource *_resource, ID3D11DepthStencilView *_view);
	virtual ~D3D11DepthStencil();
	virtual  Resource *GetResource()   { return resource; }

	ID3D11DepthStencilView * GetDepthStencilView()  { return view; }

private:
	Resource *resource;
	ID3D11DepthStencilView *view;
};

class D3D11VertexBuffer : public VertexBuffer, public NonCopyable {
public:
	D3D11VertexBuffer(D3D11Buffer *_buffer);
	virtual ~D3D11VertexBuffer() {}
	virtual D3D11Buffer *GetResource() { return buffer; }

	ID3D11Buffer * GetBuffer() { return buffer->GetInternal(); }

private:
	D3D11Buffer *buffer;
};

class D3D11IndexBuffer : public IndexBuffer, public NonCopyable {
public:
	D3D11IndexBuffer(D3D11Buffer *_resource);
	virtual ~D3D11IndexBuffer() {}
	virtual D3D11Buffer *GetResource() { return buffer; }

	ID3D11Buffer * GetBuffer() { return buffer->GetInternal(); }

private:
	D3D11Buffer *buffer;
};


class D3D11ShaderResource : public ShaderResource, NonCopyable {
public:
	D3D11ShaderResource(Resource *_resource, ID3D11ShaderResourceView *_view);
	virtual ~D3D11ShaderResource();
	virtual Resource *GetResource() { return resource; }

	ID3D11ShaderResourceView * GetShaderResourceView() { return view; }

private:
	Resource *resource;
	ID3D11ShaderResourceView *view;
};

class D3D11StreamOut : public StreamOut, NonCopyable {
public:
	D3D11StreamOut(D3D11Buffer *buffer);
	virtual ~D3D11StreamOut() {}
	virtual D3D11Buffer *GetResource()  { return buffer;  }

	ID3D11Buffer *GetBuffer() { return buffer->GetInternal(); }

private:
	D3D11Buffer *buffer;
};
	
}


#endif			//D3D11_RESOURCE_VIEW_H_