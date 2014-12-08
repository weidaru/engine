#ifndef D3D11_GRAPHIC_PIPELINE_H_
#define D3D11_GRAPHIC_PIPELINE_H_

#include "renderer/graphic_pipeline.h"
#include "renderer/rasterization_option.h"
#include "renderer/depth_stencil_option.h"
#include "renderer/blend_option.h"

#include "d3d11_vertex_buffer.h"
#include "d3d11_index_buffer.h"

#include "pipeline_stage/d3d11_input_stage.h"
#include "pipeline_stage/d3d11_output_stage.h"

struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11BlendState;

namespace s2 {
class D3D11VertexShader;
class D3D11PixelShader;
class D3D11GraphicResourceManager;
class D3D11Texture2D;

class D3D11GraphicPipeline : public GraphicPipeline {
public:
	D3D11GraphicPipeline(D3D11GraphicResourceManager *_manager);
	virtual ~D3D11GraphicPipeline();
	
	//Input
	virtual void SetPrimitiveTopology(PrimitiveTopology newvalue);
	virtual GraphicPipeline::PrimitiveTopology GetPrimitiveTopology();
	
	virtual void SetVertexBuffer(unsigned int index, unsigned int start_input_index, VertexBuffer *buf);
	virtual D3D11VertexBuffer * GetVertexBuffer(unsigned int index, unsigned int *start_input_index);
	virtual void SetIndexBuffer(IndexBuffer *buf);
	virtual D3D11IndexBuffer * GetIndexBuffer();
	
	//Shaders
	virtual void SetVertexShader(VertexShader *shader);
	virtual VertexShader * GetVertexShader();
	virtual void SetPixelShader(PixelShader *shader);
	virtual PixelShader * GetPixelShader();
	
	//Rasterization
	virtual void SetRasterizationOption(const RasterizationOption &option);
	virtual const RasterizationOption & GetRasterizationOption() const;

	//DepthStencil
	virtual void SetDepthStencilOption(const DepthStencilOption &option);
	virtual const DepthStencilOption & GetDepthStencilOption() const;
	
	//Blend
	virtual void SetBlendOption(const BlendOption &option);
	virtual const BlendOption & GetBlendOption() const;
	
	//Output
	virtual void SetRenderTarget(unsigned int index, Texture2D *target);
	virtual Resource * GetRenderTarget(unsigned int index);
	virtual void SetDepthStencilBuffer(Texture2D *buffer);
	virtual Resource * GetDepthStencilBuffer();
	
	virtual void Clear();
	
	virtual bool Validate(s2string *_error) const;

	virtual void ClearRenderTarget(Texture2D *texture, const float rgba[4]);
	virtual void ClearDepthStencilBuffer(Texture2D *buffer, bool clear_depth, float depth, bool clear_stencil, int stencil);
	virtual void Draw();
	
	
private:
	void SetupRasterizationOption();
	void SetupDepthStencilOption();
	void SetupBlendOption();
	
private:
	D3D11GraphicResourceManager *manager;
	
	D3D11InputStage input_stage; 
	
	D3D11VertexShader *vs;
	D3D11PixelShader *ps;
	
	RasterizationOption rast_opt;
	bool new_rast;
	ID3D11RasterizerState *rast_state;
	
	DepthStencilOption ds_opt;
	bool new_ds;
	ID3D11DepthStencilState *ds_state;
	
	BlendOption blend_opt;
	bool new_blend;
	ID3D11BlendState *blend_state;
	
	D3D11OutputStage output_stage;
};


}


#endif		//D3D11_GRAPHIC_PIPELINE_H_
