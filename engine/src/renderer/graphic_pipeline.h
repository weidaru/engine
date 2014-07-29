#ifndef GRAPHIC_PIPELINE_H_
#define GRAPHIC_PIPELINE_H_

class GraphicResourceManger;

class GraphicPipeline {
public:
	struct Rectangle {
		float top_left_x;
		float top_left_y;
		float width;
		float height;
	};

public:
	virtual GraphicResourceManger *  	GetResourceManager() = 0;
	
	//Input
	virtual int							SetPrimitiveType(...) = 0;
	virtual int 						BindVertexBuffer(...) = 0;
	virtual int 						BindIndexBuffer(...) = 0;
	
	//Shaders
	virtual int 						BindVertexShader(...) = 0;
	virtual int							BindPixelShader(...) = 0;
	
	//Rasterization
	virtual int 						SetViewports(int n, const Rectangle *rects) = 0;
	virtual int							SetScissorRects(int n, const Rectangle *rects) = 0;
	virtual int							SetFillMode(...) = 0;
	virtual int							SetCullMode() = 0;
	virtual int							SetFrontOrientation(...) = 0;
	virtual int							SetScissorEnable(bool enable) = 0;
	
	//Output
	//	Depth
	virtual int 						SetDepthEnable(bool enable) = 0;
	virtual int							SetDepthWritable(bool enable) = 0;
	virtual int							SetDepthFunc() = 0;
	//	Stencil
	virtual int							SetStencilEnable(bool enable) = 0;
	virtual int							SetStencilReadMask(...) = 0;
	virtual int							SetStencilWriteMask(...) = 0;
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



#endif				//GRAPHIC_PIPELINE_H_