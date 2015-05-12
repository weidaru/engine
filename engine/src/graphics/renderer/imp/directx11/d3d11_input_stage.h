#ifndef D3D11_INPUT_STAGE
#define D3D11_INPUT_STAGE

#include "graphics/renderer/graphic_pipeline.h"
#include <vector>

struct ID3D11InputLayout;

namespace s2 {

class D3D11VertexBuffer;
class D3D11IndexBuffer;
class D3D11VertexShader;
class D3D11ShaderReflection;
class D3D11GraphicResourceManager;
class D3D11GraphicPipeline;
class D3D11DrawingState;

struct D3D11InputLayout {
	ID3D11InputLayout *layout;
	unsigned int first_instance_count;

	D3D11InputLayout() : layout(0), first_instance_count(0) {}
	~D3D11InputLayout();
};

class D3D11InputStage {
private:
	struct VBInfo {
		bool is_new;
		unsigned int start_index;
		D3D11VertexBuffer *vb;
		
		VBInfo() {
			Reset();
		}
		
		void Reset() {
			is_new = false;
			start_index = 0;
			vb = 0;
		}
	};

	struct IBInfo {
		bool is_new;
		unsigned int vertex_base;
		D3D11IndexBuffer *buffer;

		IBInfo() {
			is_new = false;
			vertex_base = 0;
			buffer = 0;
		}
	};

public:
	D3D11InputStage(D3D11GraphicResourceManager *_manager, D3D11GraphicPipeline *_pipeline);
	~D3D11InputStage();
	
	void SetPrimitiveTopology(GraphicPipeline::PrimitiveTopology newvalue);
	GraphicPipeline::PrimitiveTopology GetPrimitiveTopology();
	
	void SetVertexBuffer(unsigned int index, unsigned int start_input_index, VertexBuffer *buf);
	D3D11VertexBuffer * GetVertexBuffer(unsigned int index, unsigned int *start_input_index);
	void SetIndexBuffer(IndexBuffer *buf, unsigned int vertex_base);
	D3D11IndexBuffer * GetIndexBuffer(unsigned int * vertex_base);
	
	//Validate input stage with vertex shader, set message to NULL if you don't care.
	bool Validate(const D3D11VertexShader &shader, s2string *message) const;
	//Given an not NULL reflect, the input layout will be recomputed.
	void Setup(const D3D11VertexShader *shader);
	void Setup(const D3D11VertexShader *shader, D3D11DrawingState *draw_state);

	void Flush(unsigned int start_index,unsigned int vertex_count);
	void FlushWithInstancing(unsigned int vertex_start, unsigned int vertex_count, unsigned int instance_start, unsigned int instance_count);
	void Refresh();

	void SetAutoSizeOnDraw(bool auto_size);
	bool GetAutoSizeOnDraw();
	
private:
	D3D11InputLayout * CreateInputLayout(const D3D11VertexShader *shader);
	
	static bool VBCompare(const std::vector<VBInfo>::iterator lhs, const std::vector<VBInfo>::iterator rhs);
	s2string DumpVertexBufferInfo(const std::vector<VBInfo> &infos);

	D3D11InputStage(const D3D11InputStage &);
	D3D11InputStage & operator=(const D3D11InputStage &);
	
private:
	D3D11GraphicResourceManager *manager;
	D3D11GraphicPipeline *pipeline;
	
	IBInfo ib;
	std::vector<VBInfo> vbs;
	GraphicPipeline::PrimitiveTopology topology;
	
	unsigned int current_first_instance_count;
	ID3D11InputLayout *owned_layout;

};


}


#endif		//D3D11_INPUT_STAGE
