#ifndef D3D11_INPUT_STAGE
#define D3D11_INPUT_STAGE

#include "graphics/renderer/graphic_pipeline.h"
#include <vector>
#include <stack>

struct ID3D11DeviceContext;

namespace s2 {

class D3D11InputLayout;
class D3D11VertexBuffer;
class D3D11IndexBuffer;
class D3D11VertexShader;
class D3D11ShaderReflection;
class D3D11GraphicResourceManager;
class D3D11GraphicPipeline;
class D3D11DrawingState;

class D3D11InputStage {
private:
	struct VBInfo {
		D3D11VertexBuffer *vb;
		uint32_t stride;
		uint32_t offset;
		
		VBInfo() {
			Reset();
		}
		
		void Reset() {
			vb = 0;
			stride = 0;
			offset = 0;
		}
	};

	struct IBInfo {
		uint32_t vertex_base;
		D3D11IndexBuffer *buffer;

		IBInfo() {
			vertex_base = 0;
			buffer = 0;
		}
	};

	struct State {
		IBInfo ib;
		std::vector<VBInfo> vbs;
		GraphicPipeline::PrimitiveTopology topology;
		D3D11InputLayout *input_layout;
	};

public:
	D3D11InputStage(D3D11GraphicResourceManager *_manager, ID3D11DeviceContext *_context);
	~D3D11InputStage();
	
	void SetPrimitiveTopology(GraphicPipeline::PrimitiveTopology newvalue);
	GraphicPipeline::PrimitiveTopology GetPrimitiveTopology();
	
	void SetInputLayout(InputLayout *_layout);
	D3D11InputLayout *GetInputLayout();

	void SetVertexBuffer(uint32_t index, VertexBuffer *buf);
	void SetVertexBuffer(uint32_t start_index, const std::vector<VertexBuffer *> &vbs);
	void SetVertexBuffer(uint32_t index, VertexBuffer *buf, uint32_t stride, uint32_t offset);
	void SetVertexBuffer(uint32_t start_index, 
			const std::vector<std::tuple<VertexBuffer *, uint32_t, uint32_t>> &input);
	D3D11VertexBuffer * GetVertexBuffer(uint32_t index, uint32_t *stride, uint32_t *offset);

	void SetIndexBuffer(IndexBuffer *buf, uint32_t vertex_base);
	D3D11IndexBuffer * GetIndexBuffer(uint32_t * vertex_base);

	void PushState();
	void PopState();
	void ClearSavedState();

	void Sync(ID3D11DeviceContext *context);
	
private:
	D3D11InputStage(const D3D11InputStage &);
	D3D11InputStage & operator=(const D3D11InputStage &);
	
private:
	D3D11GraphicResourceManager *manager;
	ID3D11DeviceContext *context;
	
	IBInfo ib;
	std::vector<VBInfo> vbs;
	GraphicPipeline::PrimitiveTopology topology;
	
	D3D11InputLayout *input_layout;

	std::stack<State> saved_states;
};


}


#endif		//D3D11_INPUT_STAGE
