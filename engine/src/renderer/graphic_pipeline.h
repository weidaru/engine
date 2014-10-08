#ifndef GRAPHIC_PIPELINE_H_
#define GRAPHIC_PIPELINE_H_

#include <stdint.h>
#include <vector>

#include "utils/s2string.h"

namespace s2 {

class GraphicResourceManger;
class VertexShader;
class PixelShader;
struct RasterizationOption;
struct DepthStencilOption;
struct BlendOption; 

/**
 * The core part of renderer is modelled as stateful pipeline.
 * We can modify multiple things, such as shaders, rasterization, depth, stencil, blend and so on.
 * The intention is to switch to new state easily but still be able to maintain old states. At the same time,
 * we will able to easily serialize the state for debug use.
 * This is essentially the same idea as OpenGL. In addition we get better validation and concept grouping.
 * Validation is made before the command/state really get pushed/set in GPU. 
 * Options for the same stage are grouped together in class for consistent access.
 */
class GraphicPipeline {
public:
	//Shaders
	/**
	 * The whole shader concept is the program itself and its data, which is stateful. 
	 * The concern is data validation. We can catch the error early in the setting up stage.
	 * And we can easily switch between different ways of drawing stuffs.
	 *
	 * Shaders passed in as pointers which means any change to shaders will affect the pipeline.
	 */
	virtual bool 					SetVertexShader(VertexShader *vs) = 0;
	virtual VertexShader *		GetVertexShader() = 0;
	virtual bool						SetPixelShader(PixelShader *ps) = 0;
	virtual PixelShader *		GetPixelShader() = 0;
	
	//Rasterization
	virtual bool 					SetRasterizationOption(const RasterizationOption &option) = 0;
	virtual void						GetRasterizationOption(RasterizationOption *option) = 0;

	//DepthStencil
	virtual bool 					SetDepthStencilOption(const DepthStencilOption &option) = 0;
	virtual void 					GetDepthStencilOption(DepthStencilOption *option) = 0;
	
	//Blend
	virtual bool 					SetBlendOption(const BlendOption &option) = 0;
	virtual void 					GetBlendOption(BlendOption *option) = 0;
	
	//This is only function which really does something to the pipeline.
	virtual void 					Draw() = 0;
	
	virtual void 					GetLastError(s2string *str) = 0;
};

}

#endif		//GRAPHIC_PIPELINE_H_