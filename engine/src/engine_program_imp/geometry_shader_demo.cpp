#include "engine.h"
#include "engine_program.h"

#include "graphics/renderer/all.h"
#include "asset/asset_path.h"

#include <glog/logging.h>

namespace s2 {

class GeometryShaderDemo : public EngineProgram {
public:
	GeometryShaderDemo() : vb(0), vs(0), gs(0), ps(0), input_layout(0), state(0)  {
		
	}

	virtual ~GeometryShaderDemo() {
		delete state;
	}

	virtual bool Initialize()  {
		RendererContext *context = Engine::GetSingleton()->GetRendererContext();
		GraphicResourceManager *manager = context->GetResourceManager();
		
		float vb_data[][3] = {
			{-0.9f, -0.9f, 0.0f},
			{ -0.7f, -0.7f, 0.0f },
			{ -0.5f, -0.5f, 0.0f },
			{ -0.3f, -0.3f, 0.0f },
			{ -0.1f, -0.1f, 0.0f },
			{ 0.1f, 0.1f, 0.0f },
			{ 0.3f, 0.3f, 0.0f },
			{ 0.5f, 0.5f, 0.0f },
			{ 0.7f, 0.7f, 0.0f },
			{ 0.9f, 0.9f, 0.0f },
			{ -0.9f, 0.9f, 0.0f },
			{ -0.7f, 0.7f, 0.0f },
			{ -0.5f, 0.5f, 0.0f },
			{ -0.3f, 0.3f, 0.0f },
			{ -0.1f, 0.1f, 0.0f },
			{ 0.1f, -0.1f, 0.0f },
			{ 0.3f, -0.3f, 0.0f },
			{ 0.5f, -0.5f, 0.0f },
			{ 0.7f, -0.7f, 0.0f },
			{ 0.9f, -0.9f, 0.0f }
		};
		vb = manager->CreateGraphicBuffer();
		GraphicBuffer::Option vb_option;
		vb_option.element_count = 20;
		vb_option.element_member_count = 1;
		vb_option.element_bytewidth = 12;
		vb_option.data = vb_data;
		vb_option.binding = GraphicBuffer::VERTEX_BUFFER;
		vb->Initialize(vb_option);

		vs = manager->CreateVertexShader();
		CHECK(vs->Initialize(ResolveTestAssetPath("billboard.vs"), "main"))<<vs->GetLastError();

		gs = manager->CreateGeometryShader();
		CHECK(gs->Initialize(ResolveTestAssetPath("billboard.gs"), "main"))<<gs->GetLastError();

		ps = manager->CreatePixelShader();
		CHECK(ps->Initialize(ResolveTestAssetPath("billboard.ps"), "main"))<<ps->GetLastError();

		input_layout = manager->CreateInputLayout();
		input_layout->InitializeWithElement({{0, 0}}, *vs);

		state = context->CreatePipelineState();
		state->SetDepthStencil(0);
		state->SetPrimitiveTopology(GraphicPipeline::POINT_LIST);
		state->SetVertexBuffer(0, vb->AsVertexBuffer());
		state->SetIndexBuffer(0);
		state->SetVertexShader(vs);
		state->SetInputLayout(input_layout);
		state->SetPixelShader(ps);
		state->SetGeometryShader(gs);
		state->SetRenderTarget(0, context->GetBackBuffer()->AsRenderTarget());

		return true;
	}
	virtual s2string GetName() const {
		return "GeometryShaderDemo";
	}

	virtual void OneFrame(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();

		state->Flush(pipeline);
		pipeline->Draw(0, vb->GetElementCount());
	}

private:
	GraphicBuffer *vb;
	VertexShader *vs;
	GeometryShader *gs;
	PixelShader *ps;
	InputLayout *input_layout;

	GraphicPipelineState *state;
};

//AddBeforeMain(GeometryShaderDemo)
}