#ifndef D3D11_GRAPHIC_PIPELINE_H_
#define D3D11_GRAPHIC_PIPELINE_H_

#include "graphics/renderer/graphic_pipeline.h"
#include "graphics/renderer/rasterization_option.h"
#include "graphics/renderer/depth_stencil_option.h"
#include "graphics/renderer/blend_option.h"

#include "d3d11_graphic_buffer.h"
#include "d3d11_resource_view.h"

#include "d3d11_input_stage.h"
#include "d3d11_output_stage.h"
#include "d3d11_drawing_state.h"
#include "d3d11_vertex_shader.h"
#include "d3d11_pixel_shader.h"
#include "d3d11_geometry_shader.h"
#include "d3d11_input_layout.h"

#include <map>
#include <stack>

struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11BlendState;
struct ID3D11DeviceContext;

namespace s2 {
class D3D11GraphicResourceManager;
class D3D11Texture2D;

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
	virtual D3D11InputLayout *GetInputLayout() override;
	
	virtual void SetVertexBuffer(uint32_t index, VertexBuffer *buf) override;
	virtual D3D11VertexBuffer * GetVertexBuffer(uint32_t index) override;

	virtual void SetIndexBuffer(IndexBuffer *buf, uint32_t vertex_base = 0) override;
	virtual D3D11IndexBuffer * GetIndexBuffer(uint32_t *vertex_base) override;
	
	//Shaders
	virtual void SetVertexShader(VertexShader *shader) override;
	virtual D3D11VertexShader * GetVertexShader() override;
	
	virtual void SetPixelShader(PixelShader *shader) override;
	virtual D3D11PixelShader * GetPixelShader() override;

	virtual void SetGeometryShader(GeometryShader *gs) override;
	virtual D3D11GeometryShader * GetGeometryShader() override;
	
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
	virtual D3D11RenderTarget * GetRenderTarget(uint32_t index) override;
	
	virtual void SetDepthStencil(DepthStencil *buffer) override;
	virtual D3D11DepthStencil * GetDepthStencil() override;

	virtual void SetStreamOut(uint32_t index, uint32_t stream_index, StreamOut *stream_out) override;
	virtual D3D11StreamOut * GetStreamOut(uint32_t index, uint32_t *stream_index = 0) override;
	virtual void SetRasterizedStream(int index) override;
	virtual int GetRasterizedStream() override;

	virtual void ClearRenderTarget(RenderTarget *rt, const float rgba[4]) override;
	virtual void ClearDepthStencil(DepthStencil *ds, bool clear_depth, float depth, bool clear_stencil, int stencil) override;
	virtual void Draw(DrawingState **state,  uint32_t start_index, uint32_t vertex_count) override;
	virtual void DrawInstance(DrawingState **state, 
		uint32_t vertex_start, uint32_t vertex_count, uint32_t instance_start, uint32_t instance_count) override;
	virtual void Start() override;
	virtual void End() override;

	virtual void PushState() override;
	virtual void PopState() override;
	virtual void ClearSavedState() override;

	ID3D11DeviceContext * GetDeviceContext() { return context; }

private:
	void SetupRasterizationOption();
	void SetupDepthStencilOption();
	void SetupBlendOption();
	void Check();
	void DrawSetup(D3D11DrawingState *state);
	void DrawCleanup(D3D11DrawingState *state );
	
private:
	bool active;
	D3D11GraphicResourceManager *manager;
	 ID3D11DeviceContext *context;
	
	D3D11InputStage input_stage; 
	
	D3D11VertexShader *vs;
	D3D11PixelShader *ps;
	D3D11GeometryShader *gs;
	
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
