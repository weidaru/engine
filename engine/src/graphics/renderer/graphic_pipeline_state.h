#ifndef GRAPHIC_PIPEINE_STATE_H_
#define GRAPHIC_PIPELINE_STATE_H_

namespace s2 {

class InputLayout;
class VertexShader;
class PixelShader;
class GeometryShader;
struct RasterizationOption;
struct DepthStencilOption;
struct BlendOption;

class GraphicPipelineState {
public:

	virtual ~GraphicPipelineState() {}

	virtual void SetInputLayout(InputLayout *layout) = 0;
	virtual InputLayout * GetInputLayout() const = 0;

	virtual void SetVertexShader(VertexShader *vs) = 0;
	virtual VertexShader * GetVertexShader() const = 0;
	
	virtual void SetPixelShader(PixelShader *ps) = 0;
	virtual PixelShader * GetPixelShader() const = 0;

	virtual void SetGeometryShader(GeometryShader *gs) = 0;
	virtual GeometryShader * GetGeometryShader() const = 0;

	//Rasterization
	virtual void SetRasterizationOption(const RasterizationOption &option) = 0;
	virtual const RasterizationOption & GetRasterizationOption() const = 0;

	//DepthStencil
	virtual void SetDepthStencilOption(const DepthStencilOption &option) = 0;
	virtual const DepthStencilOption & GetDepthStencilOption() const = 0;
	
	//Blend
	virtual void SetBlendOption(const BlendOption &option) = 0;
	virtual const BlendOption & GetBlendOption() const = 0;

	virtual void Prepare() = 0;
};

}


#endif // !GRAPHIC_PIPEINE_STATE_H_
