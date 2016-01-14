#ifndef D3D11_GRAPHIC_PIPELINE_H_
#define D3D11_GRAPHIC_PIPELINE_H_

#include "graphics/renderer/graphic_pipeline.h"
#include "graphics/renderer/rasterization_option.h"
#include "graphics/renderer/depth_stencil_option.h"
#include "graphics/renderer/blend_option.h"

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
class D3D11VertexBuffer;
class D3D11IndexBuffer;
class D3D11RenderTarget;
class D3D11DepthStencil;
class D3D11StreamOut;
class D3D11InputLayout;

class InputLayout;
class VertexShader;
class PixelShader;
class GeometryShader;

class D3D11GraphicPipeline : public GraphicPipeline {
private:
struct VBInfo {
	D3D11VertexBuffer *vb;
	uint32_t stride;
	uint32_t offset;
		
	VBInfo() {
		Reset();
	}
		
	void Reset() {
		vb = 0;
		stride = 0;
		offset = 0;
	}
};

struct IBInfo {
	uint32_t vertex_base;
	D3D11IndexBuffer *buffer;

	IBInfo() {
		vertex_base = 0;
		buffer = 0;
	}
};

struct  RTInfo {
	D3D11RenderTarget *data;
		
	RTInfo() {
		data = 0;
	}
};
struct  DSInfo {
	D3D11DepthStencil *data;

	DSInfo() {
		data = 0;
	}
};
struct  SOInfo {
	D3D11StreamOut *data;
	uint32_t stream_index;

	SOInfo() {
		data = 0;
		stream_index = 0;
	}
};


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
	virtual void SetRenderTargetAndDepthStencil(uint32_t index, RenderTarget *target, DepthStencil *ds) override;
	virtual void SetRenderTargetAndDepthStencil(uint32_t start_index, const std::vector<RenderTarget *> &rts, DepthStencil *ds) override;
	virtual RenderTarget * GetRenderTarget(uint32_t index) override;

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

	virtual void SetState(const GraphicPipelineState &state) override;
	virtual GraphicPipelineState * QueryState() const override;


	ID3D11DeviceContext * GetDeviceContext() { return context; }

	static ID3D11RasterizerState * ParseRasterizationOption(ID3D11Device *device, const RasterizationOption &option);
	static ID3D11DepthStencilState * ParseDepthStencilOption(ID3D11Device *device,const DepthStencilOption &option);
	static ID3D11BlendState * ParseBlendOption(ID3D11Device *device, const BlendOption &option);

private:
	void SetInputLayout(InputLayout *_layout);
	void SetVertexShader(VertexShader *shader);
	void SetPixelShader(PixelShader *shader);
	void SetGeometryShader(GeometryShader *shader);

	void SetRasterizationOption(const RasterizationOption &opt, ID3D11RasterizerState *state);
	void SetDepthStencilOption(const DepthStencilOption &opt, ID3D11DepthStencilState *state);
	void SetBlendOption(const BlendOption &opt, ID3D11BlendState *state);

	void SetRasterizationOption(const RasterizationOption &option);
	void SetDepthStencilOption(const DepthStencilOption &option);
	void SetBlendOption(const BlendOption &option);

	void SyncRenderTargetsAndDepthStencil();
	void SyncStreamOuts();

	void SyncRasterizationOption();
	void SyncDepthStencilOption();
	void SyncBlendOption();
	
private:
	D3D11GraphicResourceManager *manager;
	ID3D11DeviceContext *context;
	
	//Input
	IBInfo ib;
	std::vector<VBInfo> vbs;
	GraphicPipeline::PrimitiveTopology topology;
	D3D11InputLayout *input_layout;
	
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

	uint8_t stencil_ref;
	float blend_factor[4];
	
	//Output 
	std::vector<RTInfo > rts;
	DSInfo ds;
	std::vector<SOInfo> stream_outs;
};


}


#endif		//D3D11_GRAPHIC_PIPELINE_H_
