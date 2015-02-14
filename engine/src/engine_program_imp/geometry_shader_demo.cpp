#include "engine.h"
#include "engine_program.h"

#include "renderer/all.h"
#include "asset/asset_path.h"

#include <glog/logging.h>

namespace s2 {

class GeometryShaderDemo : public EngineProgram {
public:
	GeometryShaderDemo() : vb(0), vs(0), gs(0), ps(0) {
		
	}

	virtual ~GeometryShaderDemo() {}

	virtual bool Initialize()  {
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		
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
		vb = manager->CreateBuffer();
		Buffer::Option vb_option;
		vb_option.element_count = 20;
		vb_option.element_member_count = 1;
		vb_option.element_bytewidth = 12;
		vb_option.data = vb_data;
		vb_option.binding = Buffer::VERTEX_BUFFER;
		vb->Initialize(vb_option);

		vs = manager->CreateVertexShader();
		CHECK(vs->Initialize(ResolveAssetPath("billboard.vs"), "main"))<<vs->GetLastError();

		gs = manager->CreateGeometryShader();
		CHECK(gs->Initialize(ResolveAssetPath("billboard.gs"), "main"))<<gs->GetLastError();

		ps = manager->CreatePixelShader();
		CHECK(ps->Initialize(ResolveAssetPath("billboard.ps"), "main"))<<ps->GetLastError();

		return true;
	}
	virtual s2string GetName() const {
		return "GeometryShaderDemo";
	}

	virtual void OneFrame(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();

		Texture2D *bf = manager->GetBackBuffer();
		
		float black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		pipeline->ClearRenderTarget(bf->AsRenderTarget(), black);
		pipeline->SetDepthStencil(0);
		pipeline->SetPrimitiveTopology(GraphicPipeline::POINT_LIST);
		pipeline->SetVertexBuffer(0, 0, vb->AsVertexBuffer());
		pipeline->SetIndexBuffer(0);
		pipeline->SetVertexShader(vs);
		pipeline->SetPixelShader(ps);
		pipeline->SetGeometryShader(gs);
		pipeline->SetRenderTarget(0, bf->AsRenderTarget());

		pipeline->Draw();
	}

private:
	Buffer *vb;
	VertexShader *vs;
	GeometryShader *gs;
	PixelShader *ps;
};

AddBeforeMain(GeometryShaderDemo)
}