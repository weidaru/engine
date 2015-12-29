#ifndef GRAPHIC_PIPELINE_COMMON_H_
#define GRAPHIC_PIPELINE_COMMON_H_

#include <stdint.h>
#include <vector>

#include "utils/s2string.h"
#include "utils/type_info.h"

namespace s2 {

class GraphicResourceManger;
class InputLayout;
class VertexShader;
class PixelShader;
class GeometryShader;
class VertexBuffer;
class IndexBuffer;
class Texture2D;
class Resource;
class RenderTarget;
class DepthStencil;
class StreamOut;
class ShaderData;
struct RasterizationOption;
struct DepthStencilOption;
struct BlendOption; 

struct VertexBufferBinding {
	VertexBuffer *buffer;
	uint32_t stride;
	uint32_t offset;
};

struct StreamOutBinding {
	uint32_t stream_index;
	StreamOut *streamout;
};

class GraphicPipelineCommon {
public:
	enum PrimitiveTopology {
		POINT_LIST,
		LINE_LIST,
		LINE_STRIP,
		TRIANGLE_LIST,
		TRIANGLE_STRIP
	};

public:
	virtual ~GraphicPipelineCommon() {}

	//Input
	virtual void SetPrimitiveTopology(PrimitiveTopology newvalue) = 0;
	virtual PrimitiveTopology GetPrimitiveTopology() = 0;

	virtual void SetInputLayout(InputLayout *layout) = 0;
	virtual InputLayout * GetInputLayout() = 0;

	virtual void SetVertexBuffer(uint32_t index, VertexBuffer *buf) = 0;
	virtual void SetVertexBuffer(uint32_t start_index, const std::vector<VertexBuffer *> &iput) = 0;
	//If stride = 0, element bytewith will be used as stride.
	virtual void SetVertexBuffer(uint32_t index, VertexBuffer *buf, uint32_t stride, uint32_t offset) = 0;
	virtual void SetVertexBuffer(uint32_t start_index, 
				const std::vector<VertexBufferBinding> &input) = 0;
	virtual VertexBuffer * GetVertexBuffer(uint32_t index, uint32_t *stride, uint32_t *offset) = 0;

	virtual void SetIndexBuffer(IndexBuffer *buf, uint32_t vertex_base = 0) = 0;
	virtual IndexBuffer * GetIndexBuffer(uint32_t *vertex_base) = 0;

	virtual void SetVertexShader(VertexShader *vs) = 0;
	virtual VertexShader * GetVertexShader() = 0;
	virtual void SetVertexShaderData(ShaderData *data) = 0;
	virtual ShaderData * GetVertexShaderData() = 0;
	
	virtual void SetPixelShader(PixelShader *ps) = 0;
	virtual PixelShader * GetPixelShader() = 0;
	virtual void SetPixelShaderData(ShaderData *data) = 0;
	virtual ShaderData * GetPixelShaderData() = 0;

	virtual void SetGeometryShader(GeometryShader *gs) = 0;
	virtual GeometryShader * GetGeometryShader() = 0;
	virtual void SetGeometryShaderData(ShaderData *data) = 0;
	virtual ShaderData * GetGeometryShaderData() = 0;
	
	//Rasterization
	virtual void SetRasterizationOption(const RasterizationOption &option) = 0;
	virtual const RasterizationOption & GetRasterizationOption() const = 0;

	//DepthStencil
	virtual void SetDepthStencilOption(const DepthStencilOption &option) = 0;
	virtual const DepthStencilOption & GetDepthStencilOption() const = 0;
	
	//Blend
	virtual void SetBlendOption(const BlendOption &option) = 0;
	virtual const BlendOption & GetBlendOption() const = 0;
	
	//Output
	virtual void SetRenderTarget(uint32_t index, RenderTarget *target) = 0;
	virtual void SetRenderTarget(uint32_t start_index, const std::vector<RenderTarget *> &rts) = 0;
	virtual RenderTarget * GetRenderTarget(uint32_t index) = 0;
	
	virtual void SetDepthStencil(DepthStencil *buffer) = 0;
	virtual DepthStencil* GetDepthStencil() = 0;

	virtual void SetStreamOut(uint32_t index, uint32_t stream_index, StreamOut *stream_out) = 0;
	virtual void SetStreamOut(uint32_t start_index, const std::vector<StreamOutBinding> &stream_outs) = 0;
	virtual StreamOut * GetStreamOut(uint32_t index, uint32_t *stream_index = 0) = 0;
	//negative index means no stream get rasterized.
	virtual void SetRasterizedStream(int index) = 0;
	virtual int GetRasterizedStream() = 0;
};

}

#endif		//GRAPHIC_PIPELINE_COMMON_H_