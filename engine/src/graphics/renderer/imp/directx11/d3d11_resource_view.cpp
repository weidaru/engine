#pragma comment(lib, "d3d11.lib")

#include "d3d11_resource_view.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

namespace s2 {

D3D11RenderTarget::D3D11RenderTarget(Resource *_resource, ID3D11RenderTargetView *_view)
			: resource(_resource), view(_view){
	CHECK_NOTNULL(resource);
	CHECK_NOTNULL(view);
}

D3D11RenderTarget::~D3D11RenderTarget() {
	view->Release();
}

D3D11DepthStencil::D3D11DepthStencil(Resource *_resource, ID3D11DepthStencilView *_view)
			: resource(_resource), view(_view){
	CHECK_NOTNULL(resource);
	CHECK_NOTNULL(view);
}

D3D11DepthStencil::~D3D11DepthStencil() {
	view->Release();
}

D3D11VertexBuffer::D3D11VertexBuffer(D3D11Buffer *_buffer)
		: buffer(_buffer){
	CHECK_NOTNULL(buffer);
}

D3D11IndexBuffer::D3D11IndexBuffer(D3D11Buffer *_buffer)
		: buffer(_buffer){
	CHECK_NOTNULL(buffer);
}

D3D11ShaderResource::D3D11ShaderResource(Resource *_resource, ID3D11ShaderResourceView *_view)
		: resource(_resource), view(_view){
	CHECK_NOTNULL(resource);
	CHECK_NOTNULL(view);
}

D3D11ShaderResource::~D3D11ShaderResource() {
	view->Release();
}

D3D11StreamOut::D3D11StreamOut(D3D11Buffer *_buffer)
		: buffer(_buffer){
	CHECK_NOTNULL(buffer);
}

}


