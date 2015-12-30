#ifndef D3D11_GRAPHIC_PIPELINE_H_
#define D3D11_GRAPHIC_PIPELINE_H_

#include "graphics/renderer/graphic_pipeline.h"
#include "graphics/renderer/rasterization_option.h"
#include "graphics/renderer/depth_stencil_option.h"
#include "graphics/renderer/blend_option.h"

#include "d3d11_input_stage.h"
#include "d3d11_output_stage.h"

#include <stack>

struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11BlendState;
struct ID3D11DeviceContext;

namespace s2 {
class D3D11GraphicResourceManager;
class D3D11VertexShader;
class D3D11PixelShader;
class D3D11GeometryShader;
class D3D11ShaderData;

class D3D11GraphicPipeline : public GraphicPipeline {
public:
	D3D11GraphicPipeline(D3D11GraphicResourceManager *_manager, ID3D11DeviceContext *_context);
	virtual ~D3D11GraphicPipeline();
	
	//Input
	virtual void SetPrimitiveTopology(PrimitiveTopology newvalue) override;
	virtual GraphicPipeline::PrimitiveTopology GetPrimitiveTopology() override;

	virtual void SetVertexBuffer(uint32_t index, VertexBuffer *buf) override;
	virtual void SetVertexBuffer(uint32_t start_index, const std::vector<VertexBuffer *> &vbs) override;
	virtual void SetVertexBuffer(uint32_t index, VertexBuffer *buf, uint32_t stride, uint32_t offset) override;
	virtual void SetVertexBuffer(uint32_t start_index, 
				const std::vector<VertexBufferBinding> &input) override;
	virtual VertexBuffer * GetVertexBuffer(uint32_t index, uint32_t *stride, uint32_t *offset) override;

	virtual void SetIndexBuffer(IndexBuffer *buf, uint32_t vertex_base = 0) override;
	virtual IndexBuffer * GetIndexBuffer(uint32_t *vertex_base) override;
	
	//Shader Data
	virtual void SetVertexShaderData(ShaderData *data) override;
	virtual ShaderData * GetVertexShaderData() override;
	
	virtual void SetPixelShaderData(ShaderData *data) override;
	virtual ShaderData * GetPixelShaderData() override;

	virtual void SetGeometryShaderData(ShaderData *data) override;
	virtual ShaderData * GetGeometryShaderData() override;
	
	//Output
	virtual void SetRenderTarget(uint32_t index, RenderTarget *target) override;
	virtual void SetRenderTarget(uint32_t start_index, const std::vector<RenderTarget *> &rts) override;
	virtual RenderTarget * GetRenderTarget(uint32_t index) override;
	
	virtual void SetDepthStencil(DepthStencil *buffer) override;
	virtual DepthStencil * GetDepthStencil() override;

	virtual void SetStencilRef(uint8_t stencil_ref) override;
	virtual uint8_t GetStencilRef() override;

	virtual void SetBlendFactor(float factor[4]) override;
	virtual std::tuple<float, float, float, float> GetBlendFactor() override;

	virtual void SetStreamOut(uint32_t index, uint32_t stream_index, StreamOut *stream_out) override;
	virtual void SetStreamOut(uint32_t start_index, const std::vector<StreamOutBinding> &stream_outs) override;
	virtual StreamOut * GetStreamOut(uint32_t index, uint32_t *stream_index = 0) override;

	virtual void ClearRenderTarget(RenderTarget *rt, const float rgba[4]) override;
	virtual void ClearDepthStencil(DepthStencil *ds, bool clear_depth, float depth, bool clear_stencil, int stencil) override;

	virtual void Draw(uint32_t vertex_start, uint32_t vertex_count) override;
	virtual void DrawIndex( uint32_t index_start, uint32_t index_count) override;
	virtual void DrawInstance(uint32_t vertex_start, uint32_t vertex_count, uint32_t instance_start, uint32_t instance_count) override;
	virtual void DrawInstanceIndex(uint32_t index_start, uint32_t index_count, uint32_t instance_start, uint32_t instance_count) override;


	ID3D11DeviceContext * GetDeviceContext() { return context; }

	static ID3D11RasterizerState * ParseRasterizationOption(ID3D11Device *device, const RasterizationOption &option);
	static ID3D11DepthStencilState * ParseDepthStencilOption(ID3D11Device *device,const DepthStencilOption &option);
	static ID3D11BlendState * ParseBlendOption(ID3D11Device *device, const BlendOption &option);

private:
	void D3D11GraphicPipeline::SetVertexShader(VertexShader *shader);
	void D3D11GraphicPipeline::SetPixelShader(PixelShader *shader);
	void D3D11GraphicPipeline::SetGeometryShader(GeometryShader *shader);

	void SetRasterizationOption(const RasterizationOption &opt, ID3D11RasterizerState *state);
	void SetDepthStencilOption(const DepthStencilOption &opt, ID3D11DepthStencilState *state);
	void SetBlendOption(const BlendOption &opt, ID3D11BlendState *state);
	void SetRenderTargetsAndDepthStencil(uint32_t rt_start_index, const std::vector<RenderTarget *> &rts, DepthStencil *ds);

	void SetRasterizationOption(const RasterizationOption &option);
	void SetDepthStencilOption(const DepthStencilOption &option);
	void SetBlendOption(const BlendOption &option);

	void SetupRasterizationOption();
	void SetupDepthStencilOption();
	void SetupBlendOption();
	
private:
	D3D11GraphicResourceManager *manager;
	ID3D11DeviceContext *context;
	
	D3D11InputStage input_stage; 
	
	D3D11VertexShader *vs;
	D3D11PixelShader *ps;
	D3D11GeometryShader *gs;
	D3D11ShaderData *vs_data, *ps_data, *gs_data;
	
	RasterizationOption rast_opt;
	ID3D11RasterizerState *rast_state;
	
	DepthStencilOption ds_opt;
	ID3D11DepthStencilState *ds_state;
	
	BlendOption blend_opt;
	ID3D11BlendState *blend_state;
	
	D3D11OutputStage output_stage;
};


}


#endif		//D3D11_GRAPHIC_PIPELINE_H_
