#ifndef D3D11_INPUT_STAGE
#define D3D11_INPUT_STAGE

#include "renderer/graphic_pipeline.h"

namespace s2 {

class D3D11VertexBuffer;
class D3D11IndexBuffer;

class D3D11InputStage {
private:
	struct VBInfo {
		unsigned int start_index;
		D3D11VertexBuffer *vb;
		VertexBufferUsage usage;
		
		VBInfo() {
			start_index = -1;
			vb = 0;
			usage = GraphicPipeline::PER_VERTEX;
		}
	};

public:
	D3D11InputStage(D3D11GraphicResourceManager *_manager);
	~D3D11InputStage();
	
	void SetPrimitiveTopology(GraphicPipeline::PrimitiveTopology newvalue);
	GraphicPipeline::PrimitiveTopology GetPrimitiveTopology();
	
	void SetVertexBuffer(unsigned int index, unsigned int start_input_index, VertexBuffer *buf, GraphicPipeline::VertexBufferUsage usage);
	D3D11VertexBuffer * GetVertexBuffer(unsigned int index, unsigned int *start_input_index, GraphicPipeline::VertexBufferUsage *usage);
	void SetIndexBuffer(IndexBuffer *buf);
	D3D11IndexBuffer * GetIndexBuffer();

	Clear();
	Flush();
	
private:
	SetInput();
	SetInputLayout();
	
private:
	D3D11GraphicResourceManager *manager;
	
	bool new_input;
	D3D11IndexBuffer *ib;
	std::vector<VBInfo> vbs;
	PrimitiveTopology topology;
	ID3D11InputLayout *input_layout;
	
};


}


#endif		//D3D11_INPUT_STAGE
