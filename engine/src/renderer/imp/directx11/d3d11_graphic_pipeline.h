#ifndef D3D11_GRAPHIC_PIPELINE_H_
#define D3D11_GRAPHIC_PIPELINE_H_

#include "renderer/graphic_pipeline.h"
#include "renderer/rasterization_option.h"
#include "renderer/depth_stencil_option.h"
#include "renderer/blend_option.h"

struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11BlendState;

namespace s2 {
class D3D11VertexShader;
class D3D11PixelShader;
class D3D11GraphicResourceManager;

class D3D11GraphicPipeline : public GraphicPipeline {
public:
	D3D11GraphicPipeline(D3D11GraphicResourceManager *_manager);
	virtual ~D3D11GraphicPipeline();
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
	
	//This is only function which really does something to the pipeline.
	virtual void Draw();
	
	virtual void GetLastError(s2string *str);
	
private:
	void FlushRasterizationOption();
	void FlushDepthStencilOption();
	void FlushBlendOption();
	
private:
	D3D11GraphicResourceManager *manager;
	
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
	
	s2string error;
};


}


#endif		//D3D11_GRAPHIC_PIPELINE_H_
