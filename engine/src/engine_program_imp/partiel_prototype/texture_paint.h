#pragma once

#include <tuple>

namespace s2 {

class RendererContext;
class GraphicResourceManager;
class VertexShader;
class PixelShader;
class ShaderData;
class Sampler;
class GraphicBuffer;
class InputLayout;
class GraphicPipelineState;
class GraphicPipeline;
class ShaderResource;
class RenderTarget;

class TexturePaint {
public:
	TexturePaint();

	~TexturePaint();

	//(center_x, center_y, width, height) in NDC.
	void Paint(GraphicPipeline *pipeline, ShaderResource *source, RenderTarget *dest,
			   const std::tuple<float, float, float, float> &rect);

private:
	VertexShader *vs;
	PixelShader *ps;
	ShaderData *vs_data, *ps_data;
	Sampler *sampler;
	GraphicBuffer *quad_buffer;
	InputLayout *layout;
	GraphicPipelineState *state;
};

}

