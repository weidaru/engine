#ifndef GRAPHIC_PIPELINE_H_
#define GRAPHIC_PIPELINE_H_

#include <stdint.h>

namespace s2 {

class GraphicResourceManger;
class Buffer;
class VertexShader;
class PixelShader;

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
	virtual PrimitiveTopology			GetPrimitiveType() = 0;
	virtual void 						BindVertexBuffer(unsigned int slot, Buffer *buf, 
														 unsigned int stride, unsigned int offset) = 0;
	virtual Buffer *					GetVertexBuffer(unsigned int slot) = 0;
	virtual void 						BindIndexBuffer(unsigned int slot, Buffer *buf,
														unsigned int stride, unsigned int offset) = 0;
	virtual Buffer *					GetIndexBuffer(unsigned int slot) = 0;
	
	//Shaders
	virtual void 						BindVertexShader(VertexShader *vs) = 0;
	virtual VertexShader *				GetVertexShader() = 0;
	virtual void						BindPixelShader(PixelShader *ps) = 0;
	virtual PixelShader *				GetPixelShader() = 0;
	
	//Rasterization
	virtual void 						SetViewports(int n, const Rectangle *rects) = 0;
	virtual int 						GetViewports(Rectangle *rects);
	virtual void						SetScissorRects(int n, const Rectangle *rects) = 0;
	virtual int							GetScissorRects(Rectangle *rects);
	virtual void						SetFillMode(FillMode mode) = 0;
	virtual FillMode					GetFillMode() = 0;
	virtual void						SetCullMode(CullMode mode) = 0;
	virtual CullMode					GetCullMode() = 0;
	virtual void						SetFrontOrientation(FaceOrientation orientation) = 0;
	virtual FaceOrientation				GetFrontOrientation() = 0;
	virtual void						SetScissorEnable(bool enable) = 0;
	virtual bool						GetScissorEnalbe() = 0;
	
	//Output
	//	Depth
	virtual void						ClearDepth(float depth) = 0;
	virtual void 						SetDepthEnable(bool enable) = 0;
	virtual bool						GetDepthEnable() = 0;
	virtual void						SetDepthWritable(bool enable) = 0;
	virtual bool						GetDepthWritable() = 0;
	virtual void						SetDepthFunc(ComparisonFunc func) = 0;
	virtual ComparisonFunc				GetDepthFunc() = 0;
	//	Stencil
	virtual void 						ClearStencil(uint_8 stencil) = 0;
	virtual void						SetStencilEnable(bool enable) = 0;
	virtual bool						GetStencilEnable() = 0;
	virtual void						SetStencilReadMask(unit_8 mask) = 0;
	virtual unit_8						GetStencilReadMask() = 0;
	virtual void						SetStencilWriteMask(unit_8 mask) = 0;
	virtual unit_8						GetStencilWriteMask() = 0;
	virtual void						SetStencilFront(StencilOp fail_op, 
														StencilOp pass_op, 
														StencilOp stencil_pass_depth_fail_op, 
														ComparisonFunc comp) = 0;
	virtual void						GetStencilFront(StencilOp *fail_op, 
														StencilOp *pass_op, 
														StencilOp *stencil_pass_depth_fail_op, 
														ComparisonFunc *comp);
	virtual void						SetStencilBack(StencilOp fail_op, 
														StencilOp pass_op, 
														StencilOp stencil_pass_depth_fail_op, 
														ComparisonFunc comp) = 0;
	virtual void						GetStencilBack(StencilOp fail_op, 
														StencilOp pass_op, 
														StencilOp stencil_pass_depth_fail_op, 
														ComparisonFunc comp) = 0;
	//	Render Target and Blend
	virtual void						SetBlendAlphaToCoverageEnable(bool enable) = 0;
	virtual bool						GetBlendAlphaToCoverageEnable() = 0;
	virtual void						SetBlendFactor(const float color[4]) = 0;
	virtual void						GetBlendFactor(float *color) = 0;
	virtual void						SetBlendSampleMask(uint_8 mask) = 0;
	virtual unit_8						GetBlendSampleMask() = 0;
	virtual void						SetBlendEnable(int index, bool enable) = 0;
	virtual bool						GetBlendEnable() = 0;
	virtual void						SetBLendSrcFactor(int index, BlendFactor factor) = 0;
	virtual BlendFactor					GetBLendSrcFactor() = 0;
	virtual void						SetBlendDestFactor(int index, BlendFactor factor) = 0;
	virtual BlendFactor					GetBlendDestFactor(int index) = 0;
	virtual void						SetBlendOp(int index, BlendOp op) = 0;
	virtual BlendOp						GetBlendOp(int index) = 0;
	virtual void						SetBlendSrcAlphaFactor(int index, BlendFactor factor) = 0;
	virtual BlendFactor					GetBlendSrcAlphaFactor(int index) = 0;
	virtual void						SetBlendDestAlphaFactor(int index, BlendFactor factor) = 0;
	virtual BlendFactor					GetBlendDestAlphaFactor(int index) = 0;
	virtual void						SetBlendAlphaOp(int index, BlendOp op) = 0;
	virtual BlendOp						GetBLendAlphaOp(int index) = 0;
	virtual void						SetBLendWriteMask(int index, uint_8 mask) = 0;
	virtual unit_8						GetBlendWriteMask(int index) = 0;
	virtual void 						BindRenderTargets(int num, Texture2D *target) = 0;
	virtual int							GetRenderTargets(Texture2D *target) = 0;
	
	//Draw
	virtual void 						Draw(unsigned int vertex_count, unsigned int vertex_start_loc) = 0;
	virtual void						DrawIndexed(unsigned int vertex_count, unsigned int vertex_start_loc, int step) = 0;
	virtual void						DrawInstanced(unsigned int vertex_count_per_instance, unsigned int instance_count,
													  unsigned int vertex_start_loc, unsigned int instance_start_loc) = 0;
	virtual void						DrawIndexedInstanced(unsigned int index_count_per_instance, unsigned int instance_count,
															 unsigned int index_start_loc, int step, unsigned int instance_start_loc) = 0;
};

}

#endif		//GRAPHIC_PIPELINE_H_