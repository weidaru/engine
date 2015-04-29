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
			vs(0), gs(0), ps(0) {

		}

		virtual ~StreamoutDemo() {}

		virtual bool Initialize()  {
			GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();

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
			vb = manager->CreateBuffer();
			Buffer::Option vb_option;
			vb_option.element_count = 20;
			vb_option.element_member_count = 1;
			vb_option.element_bytewidth = 12;
			vb_option.data = vb_data;
			vb_option.binding = Buffer::VERTEX_BUFFER;
			vb->Initialize(vb_option);

			stream_out = manager->CreateBuffer();
			Buffer::Option so_option;
			so_option.element_count = 80;
			so_option.element_member_count = 1;
			so_option.element_bytewidth = 16;
			so_option.binding = Buffer::VERTEX_BUFFER | Buffer::STREAM_OUT;
			stream_out->Initialize(so_option);

			vs = manager->CreateVertexShader();
			CHECK(vs->Initialize(ResolveTestAssetPath("billboard.vs"), "main")) << vs->GetLastError();

			gs = manager->CreateGeometryShader();
			CHECK(gs->Initialize(ResolveTestAssetPath("streamout.gs"), "main")) << gs->GetLastError();

			ps = manager->CreatePixelShader();
			CHECK(ps->Initialize(ResolveTestAssetPath("billboard.ps"), "main")) << ps->GetLastError();

			return true;
		}
		virtual s2string GetName() const {
			return "StreamoutDemo";
		}

		void StreamOut(float delta) {
			GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
			GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();

			pipeline->Start();
				pipeline->SetPrimitiveTopology(GraphicPipeline::POINT_LIST);
				pipeline->SetVertexBuffer(0, 0, vb->AsVertexBuffer());
				pipeline->SetIndexBuffer(0);
				pipeline->SetVertexShader(vs);
				pipeline->SetGeometryShader(gs);
				pipeline->SetStreamOut(0, 0, stream_out->AsStreamOut());
				pipeline->SetRasterizedStream(-1);
				pipeline->Draw(&streamout_state);
			pipeline->End();
		}

		void DrawStreamOut(float delta) {
			GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
			GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();

			Texture2D *bf = manager->GetBackBuffer();

			float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

			pipeline->Start();
				pipeline->SetPrimitiveTopology(GraphicPipeline::TRIANGLE_STRIP);
				pipeline->ClearRenderTarget(bf->AsRenderTarget(), black);
				pipeline->SetDepthStencil(0);
				pipeline->SetVertexShader(vs);
				pipeline->SetGeometryShader(0);
				pipeline->SetPixelShader(ps);
				pipeline->SetVertexBuffer(0, 0, stream_out->AsVertexBuffer());
				pipeline->SetStreamOut(0, 0, 0);
				pipeline->SetRenderTarget(0, bf->AsRenderTarget());
				pipeline->Draw(&normal_state);
			pipeline->End();
		}

		virtual void OneFrame(float delta) {
			StreamOut(delta);
			DrawStreamOut(delta);
		}

	private:
		DrawingState *streamout_state, *normal_state;
		Buffer *vb, *stream_out;
		VertexShader *vs;
		GeometryShader *gs;
		PixelShader *ps;
	};

	AddBeforeMain(StreamoutDemo)
}