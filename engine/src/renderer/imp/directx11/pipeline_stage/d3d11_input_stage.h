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
class D3D11GraphicResourceManager;

//TODO: Let it grow indefinitely for now. Things like LRU could be implemented later.
class D3D11InputLayoutManager {
public:
	struct VBBinding {
		unsigned int index;
		int start_index;
		const D3D11VertexBuffer *vb;
	};

	struct InputState {
		std::vector<VBBinding> vb_state;
		const D3D11VertexShader *vs;
	};

	void Put(const InputState &state, ID3D11InputLayout *new_layout);
	ID3D11InputLayout *Get(const InputState &state);

private:
	std::map<InputState, ID3D11InputLayout *> input_layouts;
};


class D3D11InputStage {
private:
	struct VBInfo {
		int start_index;
		D3D11VertexBuffer *vb;
		
		VBInfo() {
			Reset();
		}
		
		void Reset() {
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
	
	//Validate input stage with vertex shader, set message to NULL if you don't care.
	bool Validate(const D3D11VertexShader &shader, s2string *message) const;
	//Given an not NULL reflect, the input layout will be recomputed.
	void Setup(const D3D11VertexShader *shader);
	void Flush(unsigned int vertex_count, unsigned int instance_count);
	
private:
	void SetInput();
	void SetInputLayout(const D3D11VertexShader *shader);
	
	static bool VBCompare(const std::vector<VBInfo>::iterator lhs, const std::vector<VBInfo>::iterator rhs);
	s2string DumpVertexBufferInfo(const std::vector<VBInfo> &infos);

	void BuildState(const D3D11VertexShader *shader, D3D11InputLayoutManager::InputState *state) const;

	D3D11InputStage(const D3D11InputStage &);
	D3D11InputStage & operator=(const D3D11InputStage &);
	
private:
	D3D11GraphicResourceManager *manager;
	
	D3D11IndexBuffer *ib;
	std::vector<VBInfo> vbs;
	GraphicPipeline::PrimitiveTopology topology;
	
	ID3D11InputLayout *input_layout;
	unsigned int first_instance_count;
	
	const D3D11VertexShader *old_shader;
};


}


#endif		//D3D11_INPUT_STAGE
