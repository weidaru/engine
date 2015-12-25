#ifndef D3D11_GRAPHIC_PIPELINE_H_
#define D3D11_GRAPHIC_PIPELINE_H_

#include "graphics/renderer/graphic_pipeline.h"
#include "graphics/renderer/rasterization_option.h"
#include "graphics/renderer/depth_stencil_option.h"
#include "graphics/renderer/blend_option.h"

#include "d3d11_input_stage.h"
#include "d3d11_output_stage.h"

#include <map>
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
private:
	struct State {
		D3D11VertexShader *vs;
		D3D11PixelShader *ps;
		D3D11GeometryShader *gs;
	
		RasterizationOption rast_opt;
		ID3D11RasterizerState *rast_state;
	
		DepthStencilOption ds_opt;
		ID3D11DepthStencilState *ds_state;
	
		BlendOption blend_opt;
		ID3D11BlendState *blend_state;
	};

public:
	D3D11GraphicPipeline(D3D11GraphicResourceManager *_manager, ID3D11DeviceContext *_context);
	virtual ~D3D11GraphicPipeline();
	
	//Input
	virtual void SetPrimitiveTopology(PrimitiveTopology newvalue) override;
	virtual GraphicPipeline::PrimitiveTopology GetPrimitiveTopology() override;

	virtual void SetInputLayout(InputLayout *layout) override;
	virtual InputLayout *GetInputLayout() override;
	
	virtual void SetVertexBuffer(uint32_t index, VertexBuffer *buf) override;
	virtual VertexBuffer * GetVertexBuffer(uint32_t index) override;

	virtual void SetIndexBuffer(IndexBuffer *buf, uint32_t vertex_base = 0) override;
	virtual IndexBuffer * GetIndexBuffer(uint32_t *vertex_base) override;
	
	//Shaders
	virtual void SetVertexShader(VertexShader *shader) override;
	virtual VertexShader * GetVertexShader() override;
	virtual void SetVertexShaderData(ShaderData *data) override;
	virtual ShaderData * GetVertexShaderData() override;
	
	virtual void SetPixelShader(PixelShader *shader) override;
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
	virtual RenderTarget * GetRenderTarget(uint32_t index) override;
	
	virtual void SetDepthStencil(DepthStencil *buffer) override;
	virtual DepthStencil * GetDepthStencil() override;

	virtual void SetStreamOut(uint32_t index, uint32_t stream_index, StreamOut *stream_out) override;
	virtual StreamOut * GetStreamOut(uint32_t index, uint32_t *stream_index = 0) override;
	virtual void SetRasterizedStream(int index) override;
	virtual int GetRasterizedStream() override;

	virtual void ClearRenderTarget(RenderTarget *rt, const float rgba[4]) override;
	virtual void ClearDepthStencil(DepthStencil *ds, bool clear_depth, float depth, bool clear_stencil, int stencil) override;

	virtual void Draw(uint32_t vertex_start, uint32_t vertex_count) override;
	virtual void DrawIndex( uint32_t index_start, uint32_t index_count) override;
	virtual void DrawInstance(uint32_t vertex_start, uint32_t vertex_count, uint32_t instance_start, uint32_t instance_count) override;
	virtual void DrawInstanceIndex(uint32_t index_start, uint32_t index_count, uint32_t instance_start, uint32_t instance_count) override;

	virtual void PushState() override;
	virtual void PopState() override;
	virtual void ClearSavedState() override;

	ID3D11DeviceContext * GetDeviceContext() { return context; }

private:
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

	std::stack<State> saved_states;
};


}


#endif		//D3D11_GRAPHIC_PIPELINE_H_
