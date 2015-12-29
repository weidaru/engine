#ifndef GRAPHIC_PIPEINE_STATE_H_
#define GRAPHIC_PIPELINE_STATE_H_

#include "graphic_pipeline_common.h"


namespace s2 {

class GraphicPipeline;

class GraphicPipelineState : public GraphicPipelineCommon {
public:
	virtual ~GraphicPipelineState() {}
	virtual void Flush(GraphicPipeline *pipeline) = 0;
	virtual void Prepare() = 0;
};

}


#endif // !GRAPHIC_PIPEINE_STATE_H_
