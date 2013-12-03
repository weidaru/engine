#ifndef D3D_GRAPHCIS_PIPE_H_
#define D3D_GRPAHCIS_PIPE_H_

#include "singleton.h"

class D3DVertexShader;
class D3DPixelShader;
struct ID3D11RasterizerState;
struct D3D11_VIEWPORT;
struct ID3D11RenderTargetView;
struct D3DDepthStencil;
struct ID3D11DeviceContext;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;
struct D3D11_RECT;

class D3DGraphicsPipe {
public:
	D3DGraphicsPipe(ID3D11DeviceContext *context);

	//Input Assembler stage
	void SetInputLayout(ID3D11InputLayout *input_layout);
	void SetVertexBuffer( ID3D11Buffer *vb, int stride, int offset);
	//As DXGI_FORMAT enum.
	void SetIndexBuffer(ID3D11Buffer *ib, int dxgi_format, int offset);
	//As D3D11_PRIMITIVE_TOPOLOGY enum.
	void SetPrimitiveTopology(int topology);

	//Vertex Shader stage
	void SetVertexShader(D3DVertexShader *vs);
	void SetVertexShaderConstantBuffer(int slot, ID3D11Buffer *buffer);
	void SetVertexShaderResource(int slot, ID3D11ShaderResourceView  *view);
	void SetVertexShaderResourceArray(int start_slot, int count, ID3D11ShaderResourceView  * const * view);
	void SetVertexShaderSampler(int slot, ID3D11SamplerState *ppSamplers);

	//Rasterizer stage
	void SetRasterizer(ID3D11RasterizerState *rasterizer);
	void SetScissor(int count, const D3D11_RECT *rectx);
	void SetViewports(int count, D3D11_VIEWPORT *viewports);

	//Pixel Shader stage
	void SetPixelShader(D3DPixelShader *ps);
	void SetPixelShaderConstantBuffer(int slot, ID3D11Buffer *buffer);
	void SetPixelShaderResource(int slot, ID3D11ShaderResourceView  *view);
	void SetPixelShaderResourceArray(int start_slot, int count, ID3D11ShaderResourceView  * const * view);
	void SetPixelShaderSampler(int slot, ID3D11SamplerState *ppSamplers);

	//Output Merger stage(RenderTarget, Blend, DepthStencilTest)
	void SetRenderTargetAndDepthStencil(ID3D11RenderTargetView *rt, D3DDepthStencil *ds);
	
	//Command
	void DrawIndexed(int count, int index_start, int vertex_start);
	
private:
	ID3D11DeviceContext *context_;
};

#endif 		//D3D_GRAPHICS_PIPE_H_