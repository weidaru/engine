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
	virtual int 						SetDepthBuffer(...) = 0;
	virtual int							SetStencilBuffer(...) = 0;
	virtual int 						BindRenderTarget(...) = 0;
	
	//Draw
	virtual int 						Draw(...) = 0;
	virtual int							DrawIndexed(...) = 0;
	virtual int							DrawInstanced(...) = 0;
	virtual int 						DrawIndexedInstanced(...) = 0;
};



#endif				//GRAPHIC_PIPELINE_H_