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

class D3D11GraphicPipeline : public GraphicPipeline {
public:
	enum PrimitiveTopology {
		POINT_LIST,
		LINE_LIST,
		LINE_STRIP,
		TRIANGLE_LIST,
		TRIANGLE_STRIP
	};

	enum VertexBufferUsage {
		PER_VERTEX,
		PER_INSTANCE
	};

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
		bool enable_clear;
		float depth;
		uint8_t stencil;
		
		DepthStencil(){
			tex=0;
			enable_clear=false;
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
	virtual bool SetPrimitiveTopology(PrimitiveTopology newvalue);
	virtual PrimitiveTopology GetPrimitiveTopology();
	virtual bool SetVertexBuffer(unsigned int index, VertexBuffer *buf, VertexBufferUsage usage, const s2string &type_name);
	virtual D3D11VertexBuffer * GetVertexBuffer(unsigned int index, VertexBufferUsage *usage, s2string *type_name);
	virtual bool SetIndexBuffer(IndexBuffer *buf);
	virtual D3D11IndexBuffer * GetIndexBuffer();
	
	//Shaders
	virtual bool SetVertexShader(VertexShader *shader);
	virtual VertexShader * GetVertexShader();
	virtual bool SetPixelShader(PixelShader *shader);
	virtual PixelShader * GetPixelShader();
	
	//Rasterization
	virtual bool SetRasterizationOption(const RasterizationOption &option);
	virtual void GetRasterizationOption(RasterizationOption *option);

	//DepthStencil
	virtual bool SetDepthStencilOption(const DepthStencilOption &option);
	virtual void GetDepthStencilOption(DepthStencilOption *option);
	
	//Blend
	virtual bool SetBlendOption(const BlendOption &option);
	virtual void GetBlendOption(BlendOption *option);
	
	//Output
	virtual bool SetRenderTarget(unsigned int index, Texture2D *target);
	virtual Resource * GetRenderTarget(unsigned int index);
	virtual bool SetRenderTargetClearOption(unsigned int index, bool enable, const float rgba[4]);
	virtual bool GetRenderTargetClearOption(unsigned int index, bool *enable, float *rgba);
	virtual bool SetDepthStencilBuffer(Texture2D *buffer);
	virtual Resource * GetDepthStencilBuffer();
	virtual void SetDepthStencilBufferClearOption(bool enable,  float depth, uint8_t stencil);
	virtual void GetDepthStencilBufferClearOption(bool *enable,  float *depth, uint8_t *stencil);
	
	//This is only function which really does something to the pipeline.
	virtual void Draw();
	
	virtual void GetLastError(s2string *str);
	
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
	
	s2string error;
};


}


#endif		//D3D11_GRAPHIC_PIPELINE_H_