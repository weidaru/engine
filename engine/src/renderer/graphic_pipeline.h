#ifndef GRAPHIC_PIPELINE_H_
#define GRAPHIC_PIPELINE_H_

#include <stdint.h>

namespace s2 {

class GraphicResourceManger;
class Buffer;

class GraphicPipeline {
public:
	struct Rectangle {
		float top_left_x;
		float top_left_y;
		float width;
		float height;
	};
	
	enum PrimitiveTopology {
		POINT_LIST,
		LINE_LIST,
		LINE_STRIP,
		TRIANGLE_LIST,
		TRIANGLE_STRIP
	};
	
	enum FillMode {
		WIREFRAME,
		SOLID
	};
	
	enum CullMode {
		NONE,
		FRONT,
		BACK
	};
	
	enum FaceOrientation {
		CLOCKWISE,
		COUNTER_CLOCKWISE;
	};
	
	enum ComparisonFunc {
		NEVER,
		ALWAYS,
		EQUAL,
		NOT_EQUAL,
		LESS,
		LESS_EQUAL,
		GREATER,
		GREATER_EQUAL
	};

public:
	virtual GraphicResourceManger *  	GetResourceManager() = 0;
	
	//Input
	virtual int							SetPrimitiveType(PrimitiveTopology topology) = 0;
	virtual int 						BindVertexBuffer(Buffer *buf) = 0;
	virtual int 						BindIndexBuffer(Buffer *buf) = 0;
	
	//Shaders
	virtual int 						BindVertexShader(...) = 0;
	virtual int							BindPixelShader(...) = 0;
	
	//Rasterization
	virtual int 						SetViewports(int n, const Rectangle *rects) = 0;
	virtual int							SetScissorRects(int n, const Rectangle *rects) = 0;
	virtual int							SetFillMode(FillMode mode) = 0;
	virtual int							SetCullMode(CullMode mode) = 0;
	virtual int							SetFrontOrientation(FaceOrientation orientation) = 0;
	virtual int							SetScissorEnable(bool enable) = 0;
	
	//Output
	//	Depth
	virtual int							ClearDepth(float depth) = 0;
	virtual int 						SetDepthEnable(bool enable) = 0;
	virtual int							SetDepthWritable(bool enable) = 0;
	virtual int							SetDepthFunc(ComparisonFunc func) = 0;
	//	Stencil
	virtual int 						ClearStencil(uint_8 stencil) = 0;
	virtual int							SetStencilEnable(bool enable) = 0;
	virtual int							SetStencilReadMask(unit_8 mask) = 0;
	virtual int							SetStencilWriteMask(unit_8 mask) = 0;
	virtual int							SetStencilFront(FailOp, PassOp, StencilPassDepthFailOp, CompareFunc) = 0;
	virtual int							SetStencilBack(FailOp, PassOp, StencilPassDepthFailOp, CompareFunc) = 0;
	//	Render Target and Blend
	virtual int							SetBlendAlphaToCoverageEnable(bool enable) = 0;
	virtual int							SetBlendFactor(...) = 0;
	virtual int							SetBlendSampleMask(...) = 0;
	virtual int							SetBlendEnable(int index, bool enable) = 0;
	virtual int							SetBLendSrcFactor(int index, ...) = 0;
	virtual int							SetBlendDestFactor(int index, ...) = 0;
	virtual int							SetBLendOp(int index, ...) = 0;
	virtual int							SetBlendSrcAlphaFactor(int index, ...) = 0;
	virtual int							SetBLendDestALphaFactor(int index, ...) = 0;
	virtual int							SetBlendAlphaOp(int index, ...) = 0;
	virtual int							SetBlendLogicOp(int index, ...) = 0;
	virtual int							SetBLendWriteMask(int index, ...) = 0;
	virtual int 						BindRenderTargets(...) = 0;
	
	//Draw
	virtual int 						Draw(...) = 0;
	virtual int							DrawIndexed(...) = 0;
	virtual int							DrawInstanced(...) = 0;
	virtual int 						DrawIndexedInstanced(...) = 0;
};

}

#endif				//GRAPHIC_PIPELINE_H_