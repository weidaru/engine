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

class D3D11GraphicPipelineState : public GraphicPipelineState {
public:
	D3D11GraphicPipelineState(D3D11GraphicResourceManager *_manager);
	virtual ~D3D11GraphicPipelineState();

	//Input
	virtual void SetPrimitiveTopology(PrimitiveTopology newvalue) override;
	virtual PrimitiveTopology GetPrimitiveTopology() override;

	virtual void SetInputLayout(InputLayout *layout) override;
	virtual InputLayout * GetInputLayout() override;

	virtual void SetVertexBuffer(uint32_t index, VertexBuffer *buf) override;
	virtual void SetVertexBuffer(uint32_t start_index, const std::vector<VertexBuffer *> &iput) override;
	//If stride = 0, element bytewith will be used as stride.
	virtual void SetVertexBuffer(uint32_t index, VertexBuffer *buf, uint32_t stride, uint32_t offset) override;
	virtual void SetVertexBuffer(uint32_t start_index, 
				const std::vector<VertexBufferBinding> &input) override;
	virtual VertexBuffer * GetVertexBuffer(uint32_t index, uint32_t *stride, uint32_t *offset) override;

	virtual void SetIndexBuffer(IndexBuffer *buf, uint32_t vertex_base = 0) override;
	virtual IndexBuffer * GetIndexBuffer(uint32_t *vertex_base) override;

	virtual void SetVertexShader(VertexShader *vs) override;
	virtual VertexShader * GetVertexShader() override;
	virtual void SetVertexShaderData(ShaderData *data) override;
	virtual ShaderData * GetVertexShaderData() override;
	
	virtual void SetPixelShader(PixelShader *ps) override;
	virtual PixelShader * GetPixelShader() override;
	virtual void SetPixelShaderData(ShaderData *data) override;
	virtual ShaderData * GetPixelShaderData() override;

	virtual void SetGeometryShader(GeometryShader *gs) override;
	virtual GeometryShader * GetGeometryShader() override;
	virtual void SetGeometryShaderData(ShaderData *data) override;
	virtual ShaderData * GetGeometryShaderData() override;
	
	//Rasterization
	virtual void SetRasterizationOption(const RasterizationOption &option) override;
	virtual const RasterizationOption & GetRasterizationOption() const override;

	//DepthStencil
	virtual void SetDepthStencilOption(const DepthStencilOption &option) override;
	virtual const DepthStencilOption & GetDepthStencilOption() const override;
	
	//Blend
	virtual void SetBlendOption(const BlendOption &option) override;
	virtual const BlendOption & GetBlendOption() const override;
	
	//Output
	virtual void SetRenderTarget(uint32_t index, RenderTarget *target) override;
	virtual void SetRenderTarget(uint32_t start_index, const std::vector<RenderTarget *> &rts) override;
	virtual RenderTarget * GetRenderTarget(uint32_t index) override;
	
	virtual void SetDepthStencil(DepthStencil *buffer) override;
	virtual DepthStencil* GetDepthStencil() override;

	virtual void SetStreamOut(uint32_t index, uint32_t stream_index, StreamOut *stream_out) override;
	virtual void SetStreamOut(uint32_t start_index, const std::vector<StreamOutBinding> &stream_outs) override;
	virtual StreamOut * GetStreamOut(uint32_t index, uint32_t *stream_index = 0) override;
	//negative index means no stream get rasterized.
	virtual void SetRasterizedStream(int index) override;
	virtual int GetRasterizedStream() override;

	virtual void Flush(GraphicPipeline *pipeline) override;

	//Do nothing for preparing.
	virtual void Prepare() override { }

private:
	D3D11GraphicResourceManager *manager;

	PrimitiveTopology topology;
	InputLayout *input_layout;
	
	std::vector<VertexBufferBinding> vbs;

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

	std::vector<StreamOutBinding> stream_outs;
	uint32_t rasterized_stream;
};

}


#endif // !GRAPHIC_PIPEINE_STATE_H_
