#ifndef GRAPHIC_PIPELINE_H_
#define GRAPHIC_PIPELINE_H_

#include <stdint.h>
#include <vector>

#include "utils/s2string.h"
#include "utils/type_info.h"

namespace s2 {

class GraphicResourceManger;
class VertexShader;
class PixelShader;
class GeometryShader;
class VertexBuffer;
class IndexBuffer;
class Texture2D;
class Resource;
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
	enum PrimitiveTopology {
		POINT_LIST,
		LINE_LIST,
		LINE_STRIP,
		TRIANGLE_LIST,
		TRIANGLE_STRIP
	};

public:
	//Input
	virtual void SetPrimitiveTopology(PrimitiveTopology newvalue) = 0;
	virtual PrimitiveTopology GetPrimitiveTopology() = 0;

	virtual void SetVertexBuffer(unsigned int index, unsigned int start_input_index, VertexBuffer *buf) = 0;
	virtual VertexBuffer * GetVertexBuffer(unsigned int index, unsigned int *start_input_index) = 0;

	virtual void SetStreamOut(StreamOut *stream_out) = 0;
	virtual void SetStreamOut(VertexBuffer *stream_out) = 0;
	virtual StreamOut * GetStreamOut() = 0;
	virtual VertexBuffer * GetStreamOutAsVertexBuffer() = 0;

	virtual void SetIndexBuffer(IndexBuffer *buf) = 0;
	virtual IndexBuffer * GetIndexBuffer() = 0;

	//Shaders
	/**
	 * The whole shader concept is the program itself and its data, which is stateful. 
	 * The concern is data validation. We can catch the error early in the setting up stage.
	 * And we can easily switch between different ways of drawing stuffs.
	 *
	 * Shaders passed in as pointers which means any change to shaders will affect the pipeline.
	 */
	virtual void SetVertexShader(VertexShader *vs) = 0;
	virtual VertexShader * GetVertexShader() = 0;
	
	virtual void SetPixelShader(PixelShader *ps) = 0;
	virtual PixelShader * GetPixelShader() = 0;

	virtual void SetGeometryShader(GeometryShader *gs) = 0;
	virtual GeometryShader * GetGeometryShader() = 0;
	
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
	virtual void SetRenderTarget(unsigned int index, Texture2D *target) = 0;
	virtual Resource * GetRenderTarget(unsigned int index) = 0;
	template <typename T>
	T * GetRenderTarget(unsigned int index) {
		Resource * res = GetRenderTarget(index);
	#ifdef NDEBUG
		return static_cast<T *>(res);
	#else
		return dynamic_cast<T *>(res);
	#endif
	}
	
	virtual void SetDepthStencilBuffer(Texture2D *buffer) = 0;
	virtual Resource* GetDepthStencilBuffer() = 0;
	template <typename T>
	T * GetDepthStencilBuffer(unsigned int index) {
		Resource * res = GetDepthStencilBuffer(index);
	#ifdef NDEBUG
		return static_cast<T *>(res);
	#else
		return dynamic_cast<T *>(res);
	#endif
	}
	
	//Validate whether each stage is settled properly.
	virtual bool Validate(s2string *error) const = 0;
	
	virtual void ClearRenderTarget(Texture2D *texture, const float rgba[4]) = 0;
	virtual void ClearDepthStencilBuffer(Texture2D *buffer, bool clear_depth, float depth, bool clear_stencil, int stencil) = 0;
	
	//With vertex_count and instance_count be zero, the draw call will figure out these 2 numbers by itself.
	//For instance_count, it will pick the size of first available vertex buffer which is considered as instance buffer.
	//For vertex_count, it will go to index buffer first, if no luck, pick the size of first available vertex buffer.
	virtual void Draw(unsigned int vertex_count=0, unsigned int instance_count=0) = 0;
};

}

#endif		//GRAPHIC_PIPELINE_H_