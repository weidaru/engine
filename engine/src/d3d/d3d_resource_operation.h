#ifndef D3D_RESOURCE_OPERATION_H_
#define D3D_RESOURCE_OPERATION_H_

struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11Resource;
struct D3D11_MAPPED_SUBRESOURCE;

struct D3DDepthStencil;

class D3DResourceOperation {
public:
	enum MapType {
		READ                = 1,
		WRITE               = 2,
		READ_WRITE          = 3,
		WRITE_DISCARD       = 4,
		WRITE_NO_OVERWRITE  = 5
	};

	D3DResourceOperation(ID3D11DeviceContext *context);
	
	D3D11_MAPPED_SUBRESOURCE *Map(ID3D11Resource *resource, int lod_count, MapType type, bool block);
	void UnMap(ID3D11Resource *resource, int lod_count);

	void ClearDepth(D3DDepthStencil *ds, float v);
	void ClearStencil(D3DDepthStencil *ds, int v);
	void ClearRenderTarger(ID3D11RenderTargetView *rt);

private:
	ID3D11DeviceContext *context_;
};

#endif		//D3D_RESOURCE_OPERATION_H_