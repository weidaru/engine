#ifndef GRAPHIC_PIPEINE_STATE_H_
#define GRAPHIC_PIPELINE_STATE_H_

#include "graphics/renderer/graphic_pipeline_state.h"

#include "graphics/renderer/rasterization_option.h"
#include "graphics/renderer/depth_stencil_option.h"
#include "graphics/renderer/blend_option.h"

struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11BlendState;

namespace s2 {

class D3D11GraphicPipeline;
class D3D11GraphicResourceManager;

class D3D11GraphicPipelineState : public GraphicPipelineCommon {
public:
	D3D11GraphicPipelineState(D3D11GraphicResourceManager *_manager);
	virtual ~D3D11GraphicPipelineState();

	//Input
	void SetPrimitiveTopology(PrimitiveTopology newvalue);
	PrimitiveTopology GetPrimitiveTopology();

	void SetInputLayout(InputLayout *layout);
	InputLayout * GetInputLayout();

	void SetVertexBuffer(uint32_t index, VertexBuffer *buf);
	void SetVertexBuffer(uint32_t start_index, const std::vector<VertexBuffer *> &iput);
	//If stride = 0, element bytewith will be used as stride.
	void SetVertexBuffer(uint32_t index, VertexBuffer *buf, uint32_t stride, uint32_t offset);
	void SetVertexBuffer(uint32_t start_index, 
				const std::vector<std::tuple<VertexBuffer *, uint32_t, uint32_t>> &input);
	VertexBuffer * GetVertexBuffer(uint32_t index, uint32_t *stride, uint32_t *offset);

	void SetIndexBuffer(IndexBuffer *buf, uint32_t vertex_base = 0);
	IndexBuffer * GetIndexBuffer(uint32_t *vertex_base);

	void SetVertexShader(VertexShader *vs);
	VertexShader * GetVertexShader();
	void SetVertexShaderData(ShaderData *data);
	ShaderData * GetVertexShaderData();
	
	void SetPixelShader(PixelShader *ps);
	PixelShader * GetPixelShader();
	void SetPixelShaderData(ShaderData *data);
	ShaderData * GetPixelShaderData();

	void SetGeometryShader(GeometryShader *gs);
	GeometryShader * GetGeometryShader();
	void SetGeometryShaderData(ShaderData *data);
	ShaderData * GetGeometryShaderData();
	
	//Rasterization
	void SetRasterizationOption(const RasterizationOption &option);
	const RasterizationOption & GetRasterizationOption() const;

	//DepthStencil
	void SetDepthStencilOption(const DepthStencilOption &option);
	const DepthStencilOption & GetDepthStencilOption() const;
	
	//Blend
	void SetBlendOption(const BlendOption &option);
	const BlendOption & GetBlendOption() const;
	
	//Output
	void SetRenderTarget(uint32_t index, RenderTarget *target);
	void SetRenderTarget(uint32_t start_index, const std::vector<RenderTarget *> &rts);
	RenderTarget * GetRenderTarget(uint32_t index);
	
	void SetDepthStencil(DepthStencil *buffer);
	DepthStencil* GetDepthStencil();

	void SetStreamOut(uint32_t index, uint32_t stream_index, StreamOut *stream_out);
	void SetStreamOut(uint32_t start_index, const std::vector<std::tuple<uint32_t, StreamOut *>> &stream_outs);
	StreamOut * GetStreamOut(uint32_t index, uint32_t *stream_index = 0);
	//negative index means no stream get rasterized.
	void SetRasterizedStream(int index);
	int GetRasterizedStream();

	void Flush(GraphicPipeline *pipeline);

private:
	D3D11GraphicResourceManager *manager;

	PrimitiveTopology topology;
	InputLayout *input_layout;
	
	std::vector<std::tuple<VertexBuffer *, uint32_t, uint32_t>> vbs;

	IndexBuffer *ib;
	uint32_t vertex_base;

	VertexShader *vs;
	PixelShader *ps;
	GeometryShader *gs;
	ShaderData *vs_data, *ps_data, *gs_data;
	
	RasterizationOption rast_opt;	
	ID3D11RasterizerState *rast_state;
	DepthStencilOption ds_opt;	
	ID3D11DepthStencilState *ds_state;
	BlendOption blend_opt;
	ID3D11BlendState *blend_state;

	std::vector<RenderTarget *> rts;
	DepthStencil *ds;

	std::vector<std::tuple<uint32_t, StreamOut *>> stream_outs;
	uint32_t rasterized_stream;
};

}


#endif // !GRAPHIC_PIPEINE_STATE_H_
