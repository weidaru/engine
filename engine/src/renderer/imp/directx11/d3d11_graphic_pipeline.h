#ifndef D3D11_GRAPHIC_PIPELINE_H_
#define D3D11_GRAPHIC_PIPELINE_H_

#include "renderer/graphic_pipeline.h"
#include "renderer/rasterization_option.h"
#include "renderer/depth_stencil_option.h"
#include "renderer/blend_option.h"

#include "d3d11_vertex_buffer.h"
#include "d3d11_index_buffer.h"

struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11BlendState;

namespace s2 {
class D3D11VertexShader;
class D3D11PixelShader;
class D3D11GraphicResourceManager;
class D3D11Texture2D;

class D3D11GraphicPipeline : public GraphicPipeline {
private:
	struct RenderTarget {
		D3D11Texture2D *tex;		//Only allow Texture2D for now.
		bool enable_clear;
		float rgba[4];
		
		RenderTarget() {
			tex=0;
			enable_clear=false;
		}
	};
	
	struct DepthStencil {
		D3D11Texture2D *tex;		//Only allow Texture2D for now.
		bool enable_depth_clear;
		bool enable_stencil_clear;
		float depth;
		uint8_t stencil;
		
		DepthStencil(){
			tex=0;
			enable_depth_clear = false;
			enable_stencil_clear = false;
		}
	};
	
	struct VBInfo {
		D3D11VertexBuffer *vb;
		VertexBufferUsage usage;
		s2string type_name;
		
		VBInfo() {
			vb = 0;
			usage = PER_VERTEX;
			type_name = "";
		}
	};

public:
	D3D11GraphicPipeline(D3D11GraphicResourceManager *_manager);
	virtual ~D3D11GraphicPipeline();
	
	//Input
	virtual void SetPrimitiveTopology(PrimitiveTopology newvalue);
	virtual GraphicPipeline::PrimitiveTopology GetPrimitiveTopology();
	virtual void SetVertexBuffer(unsigned int index, VertexBuffer *buf, VertexBufferUsage usage, const s2string &type_name);
	virtual D3D11VertexBuffer * GetVertexBuffer(unsigned int index, VertexBufferUsage *usage, s2string *type_name);
	virtual void SetIndexBuffer(IndexBuffer *buf);
	virtual D3D11IndexBuffer * GetIndexBuffer();
	
	//Shaders
	virtual void SetVertexShader(VertexShader *shader);
	virtual VertexShader * GetVertexShader();
	virtual void SetPixelShader(PixelShader *shader);
	virtual PixelShader * GetPixelShader();
	
	//Rasterization
	virtual void SetRasterizationOption(const RasterizationOption &option);
	virtual void GetRasterizationOption(RasterizationOption *option);

	//DepthStencil
	virtual void SetDepthStencilOption(const DepthStencilOption &option);
	virtual void GetDepthStencilOption(DepthStencilOption *option);
	
	//Blend
	virtual void SetBlendOption(const BlendOption &option);
	virtual void GetBlendOption(BlendOption *option);
	
	//Output
	virtual void SetRenderTarget(unsigned int index, Texture2D *target);
	virtual Resource * GetRenderTarget(unsigned int index);
	virtual void SetRenderTargetClearOption(unsigned int index, bool enable, const float rgba[4]);
	virtual void GetRenderTargetClearOption(unsigned int index, bool *enable, float *rgba);
	virtual void SetDepthStencilBuffer(Texture2D *buffer);
	virtual Resource * GetDepthStencilBuffer();
	virtual void SetDepthStencilBufferClearOption(bool enable_depth_clear, bool enable_stencil_clear,  float depth, uint8_t stencil);
	virtual void GetDepthStencilBufferClearOption(bool *enable_depth_clear, bool *enable_stencil_clear,  float *depth, uint8_t *stencil);
	
	virtual bool Validate(s2string *_error);
	
	//This is only function which really does something to the pipeline.
	virtual void Draw();
	
private:
	void SetInput();
	void SetRasterizationOption();
	void SetDepthStencilOption();
	void SetBlendOption();
	void SetOutput();
	
private:
	D3D11GraphicResourceManager *manager;
	
	bool new_input;
	D3D11IndexBuffer *ib;
	std::vector<VBInfo> vbs;
	PrimitiveTopology topology;
	
	bool new_vs;
	D3D11VertexShader *vs;
	bool new_ps;
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
	
	bool new_output;
	std::vector<RenderTarget> rts;
	DepthStencil ds;
};


}


#endif		//D3D11_GRAPHIC_PIPELINE_H_
