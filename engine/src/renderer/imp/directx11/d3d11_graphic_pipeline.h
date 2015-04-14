#ifndef D3D11_GRAPHIC_PIPELINE_H_
#define D3D11_GRAPHIC_PIPELINE_H_

#include "renderer/graphic_pipeline.h"
#include "renderer/rasterization_option.h"
#include "renderer/depth_stencil_option.h"
#include "renderer/blend_option.h"

#include "d3d11_buffer.h"
#include "d3d11_resource_view.h"

#include "d3d11_input_stage.h"
#include "d3d11_output_stage.h"
#include "d3d11_drawing_state.h"
#include "resource_conflict_resolver.h"

#include <map>

struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11BlendState;

namespace s2 {
class D3D11VertexShader;
class D3D11PixelShader;
class D3D11GeometryShader;
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
	virtual D3D11VertexBuffer * GetVertexBuffer(unsigned int index, unsigned int *start_input_index=0);

	virtual void SetIndexBuffer(IndexBuffer *buf);
	virtual D3D11IndexBuffer * GetIndexBuffer();
	
	//Shaders
	virtual void SetVertexShader(VertexShader *shader);
	virtual VertexShader * GetVertexShader();
	
	virtual void SetPixelShader(PixelShader *shader);
	virtual PixelShader * GetPixelShader();

	virtual void SetGeometryShader(GeometryShader *gs);
	virtual GeometryShader * GetGeometryShader();
	
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
	virtual void SetRenderTarget(unsigned int index, RenderTarget *target);
	virtual D3D11RenderTarget * GetRenderTarget(unsigned int index);
	
	virtual void SetDepthStencil(DepthStencil *buffer);
	virtual D3D11DepthStencil * GetDepthStencil();

	virtual void SetStreamOut(unsigned int index, unsigned int start_output_index, StreamOut *stream_out);
	virtual D3D11StreamOut * GetStreamOut(unsigned int index, unsigned int *start_output_index=0);
	virtual void SetRasterizedStream(int index);
	virtual int GetRasterizedStream();

	virtual bool Validate(s2string *_error) const;

	virtual void ClearRenderTarget(RenderTarget *rt, const float rgba[4]);
	virtual void ClearDepthStencil(DepthStencil *ds, bool clear_depth, float depth, bool clear_stencil, int stencil);
	virtual void ResolveConflict();
	virtual void Draw(DrawingState **state=0, unsigned int vertex_count = 0, unsigned int instance_count = 0);
	virtual void Start();
	virtual void End();

private:
	void SetupRasterizationOption();
	void SetupDepthStencilOption();
	void SetupBlendOption();
	void Check();
	
private:
	bool active;
	D3D11GraphicResourceManager *manager;
	
	D3D11InputStage input_stage; 
	
	D3D11VertexShader *vs;
	D3D11PixelShader *ps;
	D3D11GeometryShader *gs;
	
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

	ResourceConflictResolver sr_rt_resolver;

};


}


#endif		//D3D11_GRAPHIC_PIPELINE_H_
