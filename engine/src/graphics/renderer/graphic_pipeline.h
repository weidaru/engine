#ifndef GRAPHIC_PIPELINE_H_
#define GRAPHIC_PIPELINE_H_

#include "graphic_pipeline_common.h"

namespace s2 {

class GraphicPipeline : public GraphicPipelineCommon {
public:
	//Additional rendering actions.
	virtual void ClearRenderTarget(RenderTarget *rt, const float rgba[4]) = 0;
	virtual void ClearDepthStencil(DepthStencil *ds, bool clear_depth, float depth, bool clear_stencil, int stencil) = 0;
	
	virtual void Draw(uint32_t vertex_start, uint32_t vertex_count) = 0;
	virtual void DrawIndex( uint32_t index_start, uint32_t index_count) = 0;
	virtual void DrawInstance(uint32_t vertex_start, uint32_t vertex_count, uint32_t instance_start, uint32_t instance_count) = 0;
	virtual void DrawInstanceIndex(uint32_t index_start, uint32_t index_count, uint32_t instance_start, uint32_t instance_count) = 0;

	virtual void PushState() = 0;
	virtual void PopState() = 0;
	virtual void ClearSavedState() = 0;
};

}

#endif		//GRAPHIC_PIPELINE_H_