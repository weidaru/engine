#ifndef GRAPHIC_PIPELINE_H_
#define GRAPHIC_PIPELINE_H_

#include <stdint.h>
#include <vector>

namespace s2 {

class GraphicResourceManger;
class VertexShader;
class PixelShader;
class Texture2D;
class DepthStencilBuffer;

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
	
	enum StencilOp {
		KEEP,
		ZERO,
		REPLACE,
		INC_SAT,
		DEC_SAT,
		INVERT,
		INC,
		DEC
	};
	
	enum BlendFactor {
		ZERO,
		ONE,
		SRC_COLOR,
		INV_SRC_COLOR,
		SRC_ALPHA,
		INV_SRC_ALPHA,
		DEST_COLOR,
		INV_DEST_COLOR,
		DEST_ALPHA,
		INV_DEST_ALPHA,
		BLEND_FACTOR,
		INV_BLEND_FACTOR,
	};
	
	enum BlendOp {
		ADD,
		SUBTRACT,
		REV_SUBTRACT,
		MIN,
		MAX
	};

public:
	virtual GraphicResourceManger *  	GetResourceManager() = 0;
	
	//Input
	virtual void						SetPrimitiveType(PrimitiveTopology topology) = 0;
	virtual PrimitiveTopology			GetPrimitiveType() const = 0;
	
	//Shaders
	virtual void 						SetVertexShader(VertexShader *vs) = 0;
	virtual VertexShader *				GetVertexShader() const = 0;
	virtual void						SetPixelShader(PixelShader *ps) = 0;
	virtual PixelShader *				GetPixelShader() const = 0;
	
	//Rasterization
	virtual void 						SetViewports(const std::vector<Rectangle> &rects) = 0;
	virtual void 						GetViewports(std::vector<Rectangle> *rects) const = 0;
	virtual void						SetScissorRects(const std::vector<Rectangle> &rects) = 0;
	virtual void						GetScissorRects(std::vector<Rectangle> *rects) const = 0;
	virtual void						SetFillMode(FillMode mode) = 0;
	virtual FillMode					GetFillMode() const = 0;
	virtual void						SetCullMode(CullMode mode) = 0;
	virtual CullMode					GetCullMode() const = 0;
	virtual void						SetFrontOrientation(FaceOrientation orientation) = 0;
	virtual FaceOrientation				GetFrontOrientation() const = 0;
	virtual void						SetScissorEnable(bool enable) = 0;
	virtual bool						GetScissorEnalbe() const = 0;
	
	//Output
	virtual void 						SetDepthStencilBuffer(DepthStencilBuffer *depth_stencil) = 0;
	virtual DepthStencilBuffer *		GetDepthStencilBuffer() = 0;
	//	Depth
	virtual void						ClearDepth(float depth) = 0;
	virtual void 						SetDepthEnable(bool enable) = 0;
	virtual bool						GetDepthEnable() const = 0;
	virtual void						SetDepthWritable(bool enable) = 0;
	virtual bool						GetDepthWritable() const = 0;
	virtual void						SetDepthFunc(ComparisonFunc func) = 0;
	virtual ComparisonFunc				GetDepthFunc() const = 0;
	//	Stencil
	virtual void 						ClearStencil(uint_8 stencil) = 0;
	virtual void						SetStencilEnable(bool enable) = 0;
	virtual bool						GetStencilEnable() const = 0;
	virtual void						SetStencilReadMask(unit_8 mask) = 0;
	virtual unit_8						GetStencilReadMask() const = 0;
	virtual void						SetStencilWriteMask(unit_8 mask) = 0;
	virtual unit_8						GetStencilWriteMask() const = 0;
	virtual void						SetStencilFront(StencilOp fail_op, 
														StencilOp pass_op, 
														StencilOp stencil_pass_depth_fail_op, 
														ComparisonFunc comp) = 0;
	virtual void						GetStencilFront(StencilOp *fail_op, 
														StencilOp *pass_op, 
														StencilOp *stencil_pass_depth_fail_op, 
														ComparisonFunc *comp) const = 0;
	virtual void						SetStencilBack(StencilOp fail_op, 
														StencilOp pass_op, 
														StencilOp stencil_pass_depth_fail_op, 
														ComparisonFunc comp) = 0;
	virtual void						GetStencilBack(StencilOp *fail_op, 
														StencilOp *pass_op, 
														StencilOp *stencil_pass_depth_fail_op, 
														ComparisonFunc *comp) const = 0;
	//	Render Target and Blend
	virtual void						SetBlendAlphaToCoverageEnable(bool enable) = 0;
	virtual bool						GetBlendAlphaToCoverageEnable() const = 0;
	virtual void						SetBlendFactor(const float color[4]) = 0;
	virtual void						GetBlendFactor(float *color) const = 0;
	virtual void						SetBlendSampleMask(uint_8 mask) = 0;
	virtual unit_8						GetBlendSampleMask() const = 0;
	virtual void						SetBlendEnable(int index, bool enable) = 0;
	virtual bool						GetBlendEnable() const = 0;
	virtual void						SetBLendSrcFactor(int index, BlendFactor factor) = 0;
	virtual BlendFactor					GetBLendSrcFactor() const = 0;
	virtual void						SetBlendDestFactor(int index, BlendFactor factor) = 0;
	virtual BlendFactor					GetBlendDestFactor(int index) const = 0;
	virtual void						SetBlendOp(int index, BlendOp op) = 0;
	virtual BlendOp						GetBlendOp(int index) const = 0;
	virtual void						SetBlendSrcAlphaFactor(int index, BlendFactor factor) = 0;
	virtual BlendFactor					GetBlendSrcAlphaFactor(int index) const = 0;
	virtual void						SetBlendDestAlphaFactor(int index, BlendFactor factor) = 0;
	virtual BlendFactor					GetBlendDestAlphaFactor(int index) const = 0;
	virtual void						SetBlendAlphaOp(int index, BlendOp op) = 0;
	virtual BlendOp						GetBLendAlphaOp(int index) const = 0;
	virtual void						SetBLendWriteMask(int index, uint_8 mask) = 0;
	virtual unit_8						GetBlendWriteMask(int index) const = 0;
	
	//Draw
	virtual void 						Draw() = 0;
};

}

#endif		//GRAPHIC_PIPELINE_H_