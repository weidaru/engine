#ifndef GRAPHIC_PIPELINE_H_
#define GRAPHIC_PIPELINE_H_

#include <stdint.h>
#include <vector>

namespace s2 {

class GraphicPipelineState;
class VertexBuffer;
class IndexBuffer;
class ShaderData;
class RenderTarget;
class DepthStencil;
class StreamOut;

struct VertexBufferBinding {
	VertexBuffer *buffer;
	uint32_t stride;
	uint32_t offset;
};

struct StreamOutBinding {
	uint32_t stream_index;
	StreamOut *streamout;
};

class GraphicPipeline {
public:
enum PrimitiveTopology {
	POINT_LIST,
	LINE_LIST,
	LINE_STRIP,
	TRIANGLE_LIST,
	TRIANGLE_STRIP
};

public:
	virtual ~GraphicPipeline() {}

	virtual void SetPrimitiveTopology(PrimitiveTopology newvalue) = 0;
	virtual PrimitiveTopology GetPrimitiveTopology() = 0;

	virtual void SetVertexBuffer(uint32_t index, VertexBuffer *buf) = 0;
	virtual void SetVertexBuffer(uint32_t start_index, const std::vector<VertexBuffer *> &iput) = 0;
	//If stride = 0, element bytewith will be used as stride.
	virtual void SetVertexBuffer(uint32_t index, VertexBuffer *buf, uint32_t stride, uint32_t offset) = 0;
	virtual void SetVertexBuffer(uint32_t start_index, 
				const std::vector<VertexBufferBinding> &input) = 0;
	virtual VertexBuffer * GetVertexBuffer(uint32_t index, uint32_t *stride, uint32_t *offset) = 0;
	
	virtual void SetIndexBuffer(IndexBuffer *buf, uint32_t vertex_base = 0) = 0;
	virtual IndexBuffer * GetIndexBuffer(uint32_t *vertex_base) = 0;

	virtual void SetVertexShaderData(ShaderData *data) = 0;
	virtual ShaderData * GetVertexShaderData() = 0;

	virtual void SetPixelShaderData(ShaderData *data) = 0;
	virtual ShaderData * GetPixelShaderData() = 0;

	virtual void SetGeometryShaderData(ShaderData *data) = 0;
	virtual ShaderData * GetGeometryShaderData() = 0;

	virtual void SetRenderTarget(uint32_t index, RenderTarget *target) = 0;
	virtual void SetRenderTarget(uint32_t start_index, const std::vector<RenderTarget *> &rts) = 0;
	virtual RenderTarget * GetRenderTarget(uint32_t index) = 0;
	
	virtual void SetDepthStencil(DepthStencil *buffer) = 0;
	virtual DepthStencil* GetDepthStencil() = 0;

	virtual void SetStencilRef(uint8_t stencil_ref) = 0;
	virtual uint8_t GetStencilRef() = 0;

	virtual void SetBlendFactor(float factor[4]) = 0;
	virtual std::tuple<float, float, float, float> GetBlendFactor() = 0;

	virtual void SetStreamOut(uint32_t index, uint32_t stream_index, StreamOut *stream_out) = 0;
	virtual void SetStreamOut(uint32_t start_index, const std::vector<StreamOutBinding> &stream_outs) = 0;
	virtual StreamOut * GetStreamOut(uint32_t index, uint32_t *stream_index = 0) = 0;

	virtual void ClearRenderTarget(RenderTarget *rt, const float rgba[4]) = 0;
	virtual void ClearDepthStencil(DepthStencil *ds, bool clear_depth, float depth, bool clear_stencil, int stencil) = 0;
	
	virtual void Draw(uint32_t vertex_start, uint32_t vertex_count) = 0;
	virtual void DrawIndex( uint32_t index_start, uint32_t index_count) = 0;
	virtual void DrawInstance(uint32_t vertex_start, uint32_t vertex_count, uint32_t instance_start, uint32_t instance_count) = 0;
	virtual void DrawInstanceIndex(uint32_t index_start, uint32_t index_count, uint32_t instance_start, uint32_t instance_count) = 0;

	virtual void SetState(const GraphicPipelineState &state) = 0;
	virtual GraphicPipelineState * QueryState() const = 0;
};

}

#endif		//GRAPHIC_PIPELINE_H_