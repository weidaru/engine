#ifndef D3D11_INPUT_STAGE
#define D3D11_INPUT_STAGE

#include "renderer/graphic_pipeline.h"
#include <vector>

struct ID3D11InputLayout;

namespace s2 {

class D3D11VertexBuffer;
class D3D11IndexBuffer;
class D3D11VertexShader;
class D3D11ShaderReflection;

class D3D11InputStage {
private:
	struct VBInfo {
		unsigned int start_index;
		D3D11VertexBuffer *vb;
		
		VBInfo() {
			start_index = -1;
			vb = 0;
		}
	};

public:
	D3D11InputStage(D3D11GraphicResourceManager *_manager);
	~D3D11InputStage();
	
	void SetPrimitiveTopology(GraphicPipeline::PrimitiveTopology newvalue);
	GraphicPipeline::PrimitiveTopology GetPrimitiveTopology();
	
	void SetVertexBuffer(unsigned int index, unsigned int start_input_index, VertexBuffer *buf);
	D3D11VertexBuffer * GetVertexBuffer(unsigned int index, unsigned int *start_input_index);
	void SetIndexBuffer(IndexBuffer *buf);
	D3D11IndexBuffer * GetIndexBuffer();

	void Clear();
	//Validate input stage with vertex shader, set message to NULL if you don't care.
	bool Validate(const D3D11VertexShader &shader, s2string *message) const;
	//Given an not NULL reflect, the input layout will be recomputed.
	void Setup(const D3D11VertexShader *shader);
	void Flush();
	
private:
	SetInput();
	SetInputLayout(const D3D11VertexShader &shader);
	
	static bool VBCompare(const std::vector<VBInfo>::iterator lhs, const std::vector<VBInfo>::iterator rhs);
	s2string DumpVertexBufferInfo(const std::vector<VBInfo> infos);
	
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
