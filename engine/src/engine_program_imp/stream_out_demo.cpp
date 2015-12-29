#include "engine.h"
#include "engine_program.h"

#include "graphics/renderer/all.h"
#include "asset/asset_path.h"

#include <glog/logging.h>

namespace s2 {

class StreamoutDemo : public EngineProgram {
public:
	StreamoutDemo()
		: streamout_state(0), normal_state(0),
		vb(0), stream_out(0),
		vs(0), input_layout(0), gs(0), ps(0) {

	}

	virtual ~StreamoutDemo() {
		delete streamout_state;
		delete normal_state;
	}

	virtual bool Initialize()  {
		RendererContext *context = Engine::GetSingleton()->GetRendererContext();
		GraphicResourceManager *manager = context->GetResourceManager();

		float vb_data[][3] = {
			{ -0.9f, -0.9f, 0.0f },
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

		stream_out = manager->CreateGraphicBuffer();
		GraphicBuffer::Option so_option;
		so_option.element_count = 80;
		so_option.element_member_count = 1;
		so_option.element_bytewidth = 16;
		so_option.binding = GraphicBuffer::VERTEX_BUFFER | GraphicBuffer::STREAM_OUT;
		stream_out->Initialize(so_option);

		vs = manager->CreateVertexShader();
		CHECK(vs->Initialize(ResolveTestAssetPath("billboard.vs"), "main")) << vs->GetLastError();

		gs = manager->CreateGeometryShader();
		CHECK(gs->Initialize(ResolveTestAssetPath("streamout.gs"), "main")) << gs->GetLastError();

		ps = manager->CreatePixelShader();
		CHECK(ps->Initialize(ResolveTestAssetPath("billboard.ps"), "main")) << ps->GetLastError();

		input_layout = manager->CreateInputLayout();
		input_layout->InitializeWithElement({{0,0}}, *vs);

		streamout_state = context->CreatePipelineState();
		streamout_state->SetPrimitiveTopology(GraphicPipeline::POINT_LIST);
		streamout_state->SetVertexBuffer(0, vb->AsVertexBuffer());
		streamout_state->SetIndexBuffer(0);
		streamout_state->SetVertexShader(vs);
		streamout_state->SetInputLayout(input_layout);
		streamout_state->SetGeometryShader(gs);
		streamout_state->SetStreamOut(0, 0, stream_out->AsStreamOut());
		streamout_state->SetRasterizedStream(-1);
		
		normal_state = context->CreatePipelineState();
		normal_state->SetPrimitiveTopology(GraphicPipeline::TRIANGLE_STRIP);
		normal_state->SetDepthStencil(0);
		normal_state->SetVertexShader(vs);
		normal_state->SetInputLayout(input_layout);
		normal_state->SetGeometryShader(0);
		normal_state->SetPixelShader(ps);
		normal_state->SetStreamOut(0, 0, 0);
		normal_state->SetVertexBuffer(0, stream_out->AsVertexBuffer());
		normal_state->SetRenderTarget(0, context->GetBackBuffer()->AsRenderTarget());

		return true;
	}
	virtual s2string GetName() const {
		return "StreamoutDemo";
	}

	void StreamOut(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();

		streamout_state->Flush(pipeline);
		pipeline->Draw(0, vb->GetElementCount());
	}

	void DrawStreamOut(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();

		normal_state->Flush(pipeline);
		pipeline->Draw(0, stream_out->GetElementCount());
	}

	virtual void OneFrame(float delta) {
		StreamOut(delta);
		DrawStreamOut(delta);
	}

private:
	GraphicPipelineState *streamout_state, *normal_state;
	GraphicBuffer *vb, *stream_out;
	VertexShader *vs;
	InputLayout *input_layout;
	GeometryShader *gs;
	PixelShader *ps;
};

//AddBeforeMain(StreamoutDemo)
}