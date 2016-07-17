#include "texture_paint.h"

#include "engine_program.h"
#include "engine.h"
#include "input/input_system.h"

#include "asset/asset_path.h"

#include "graphics/renderer/all.h"

#include "utils/s2matrix4x4.h"

#include <glog/logging.h>

namespace s2 {

TexturePaint::TexturePaint()  {
	RendererContext *context = Engine::GetSingleton()->GetRendererContext();
	GraphicResourceManager *manager = context->GetResourceManager();

	vs = manager->CreateVertexShader();
	CHECK(vs->Initialize(ResolveTestAssetPath("particle/texture_paint_vs.hlsl"), "main")) << vs->GetLastError();

	ps = manager->CreatePixelShader();
	CHECK(ps->Initialize(ResolveTestAssetPath("particle/texture_paint_ps.hlsl"), "main")) << ps->GetLastError();

	vs_data = manager->CreateShaderData();
	vs_data->Initialize(vs->GetShaderBytecode());

	sampler = manager->CreateSampler();
	sampler->Initialize(Sampler::Option());

	ps_data = manager->CreateShaderData();
	ps_data->Initialize(ps->GetShaderBytecode());
	ps_data->SetSampler("paint_sampler", sampler);

	quad_buffer = manager->CreateGraphicBuffer();
	{
		float vb_data[][5] = {
			//position(float3), tex(float2)
			{ -1.0f, -1.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, -1.0f, 0.0f, 1.0f, 1.0f },
			{ -1.0f, 1.0f, 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 0.0f, 1.0f, 0.0f },
		};
		quad_buffer = manager->CreateGraphicBuffer();
		GraphicBuffer::Option vb_option;
		vb_option.element_count = 4;
		vb_option.element_member_count = 1;
		vb_option.element_bytewidth = 20;
		vb_option.data = vb_data;
		vb_option.binding = RendererBinding::VERTEX_BUFFER;
		quad_buffer->Initialize(vb_option);
	}

	layout = manager->CreateInputLayout();
	layout->InitializeWithElement({ { 0, 0 }, { 0, 12 } }, *vs);

	state = context->CreatePipelineState();
	state->SetVertexShader(vs);
	state->SetPixelShader(ps);
	state->SetInputLayout(layout);
}

TexturePaint::~TexturePaint() {
	RendererContext *context = Engine::GetSingleton()->GetRendererContext();
	GraphicResourceManager *manager = context->GetResourceManager();

	manager->RemoveVertexShader(vs->GetID());
	manager->RemovePixelShader(ps->GetID());
	manager->RemoveShaderData(vs_data->GetID());
	manager->RemoveShaderData(ps_data->GetID());
	manager->RemoveSampler(sampler->GetID());
	manager->RemoveGraphicBuffer(quad_buffer->GetID());
	manager->RemoveInputLayout(layout->GetID());

	delete state;
}

//(center_x, center_y, width, height) in NDC.
void TexturePaint::Paint(GraphicPipeline *pipeline, ShaderResource *source, RenderTarget *dest,
	const std::tuple<float, float, float, float> &rect) {
	S2Matrix4x4 matrix;
	matrix.SetTranslate({ std::get<0>(rect), std::get<1>(rect), 0.0 });
	S2Matrix4x4 scale;
	scale.SetScale({ std::get<2>(rect) / 2.0f, std::get<3>(rect) / 2.0f, 1.0f });
	matrix *= scale;
	vs_data->SetUniform("transform", matrix);
	vs_data->FlushConstantBuffer(pipeline);

	ps_data->SetShaderResource("paint_texture", source);

	pipeline->SetState(*state);
	pipeline->SetVertexShaderData(vs_data);
	pipeline->SetPixelShaderData(ps_data);
	pipeline->SetRenderTargetAndDepthStencil(0, dest, 0);
	pipeline->SetVertexBuffer(0, quad_buffer->AsVertexBuffer());

	pipeline->Draw(0, 4);
}

}

