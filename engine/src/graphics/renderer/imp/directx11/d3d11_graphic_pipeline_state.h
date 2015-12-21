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
	D3D11GraphicPipelineState(D3D11GraphicResourceManager *_manager, bool shell_only);
	virtual ~D3D11GraphicPipelineState();

	virtual void SetInputLayout(InputLayout *layout) override;
	virtual InputLayout * GetInputLayout() const override;

	virtual void SetVertexShader(VertexShader *vs) override;
	virtual VertexShader * GetVertexShader() const override;
	
	virtual void SetPixelShader(PixelShader *ps) override;
	virtual PixelShader * GetPixelShader() const override;

	virtual void SetGeometryShader(GeometryShader *gs) override;
	virtual GeometryShader * GetGeometryShader() const override;
	
	//Rasterization
	virtual void SetRasterizationOption(const RasterizationOption &option) override;
	virtual const RasterizationOption & GetRasterizationOption() const override;
	ID3D11RasterizerState * GetRasterizationState() const { return rast_state; }
	void SetRasterizationState(const RasterizationOption &option, ID3D11RasterizerState * state);

	//DepthStencil
	virtual void SetDepthStencilOption(const DepthStencilOption &option) override;
	virtual const DepthStencilOption & GetDepthStencilOption() const override;
	ID3D11DepthStencilState * GetDepthStencilState() const { return ds_state; }
	void SetDepthStencilState(const DepthStencilOption &option, ID3D11DepthStencilState *state);
	
	//Blend
	virtual void SetBlendOption(const BlendOption &option) override;
	virtual const BlendOption & GetBlendOption() const override;
	ID3D11BlendState * GetBlendState() const { return blend_state; }
	void SetBlendState(const BlendOption &option, ID3D11BlendState *state);

	//Do nothing for preparing.
	virtual void Prepare() override { }

private:
	D3D11GraphicResourceManager *manager;

	InputLayout *input_layout;

	VertexShader *vs;
	PixelShader *ps;
	GeometryShader *gs;
	
	RasterizationOption rast_opt;	
	ID3D11RasterizerState *rast_state;
	DepthStencilOption ds_opt;	
	ID3D11DepthStencilState *ds_state;
	BlendOption blend_opt;
	ID3D11BlendState *blend_state;
};

}


#endif // !GRAPHIC_PIPEINE_STATE_H_
