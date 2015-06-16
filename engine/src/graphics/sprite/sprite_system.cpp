#include "sprite_system.h"
#include "engine.h"
#include "graphics/renderer/all.h"
#include "sprite.h"
#include "asset/asset_path.h"
#include <glog/logging.h>

namespace s2 {

uint32_t SpriteSystem::kSpriteBatchSize = 400;

SpriteSystem::SpriteSystem() 
		: vs(0), ps(0), vertex_buffer(0), instance_buffer(0), drawing_state(0) {
	auto manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();

	vs = manager->CreateVertexShader();
	CHECK(vs->Initialize(ResolveAssetPath("shader/sprite.hlsl"), "fs_main")) << vs->GetLastError();

	ps = manager->CreatePixelShader();
	CHECK(ps->Initialize(ResolveAssetPath("shader/sprite.hlsl"), "ps_main")) << ps->GetLastError();

	vertex_buffer = manager->CreateBuffer();
	Buffer::Option option;
	SpriteVertex verticies[4] = {
		{-0.1f, -0.1f, 0.0f},
		{0.1f, -0.1f, 0.0f},
		{-0.1f, 0.1f, 0.0f},
		{0.1f, 0.1f, 0.0f}
	};
	option.Initialize(4, verticies);
	option.resource_write = RendererEnum::IMMUTABLE;
	vertex_buffer->Initialize(option);

	instance_buffer = manager->CreateBuffer();
	option.Initialize<SpriteInstance>(kSpriteBatchSize, 0);
	option.resource_write = RendererEnum::CPU_WRITE_FREQUENT;
	instance_buffer->Initialize(option);
}

SpriteSystem::~SpriteSystem() {
	GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();

	manager->RemoveBuffer(instance_buffer->GetID());
	manager->RemoveBuffer(vertex_buffer->GetID());
	manager->RemovePixelShader(ps->GetID());
	manager->RemoveVertexShader(vs->GetID());
	delete drawing_state;
}
	
void SpriteSystem::Register(Sprite *s) {
	CHECK_NOTNULL(s);
	sprites.push_back(s);
}

void SpriteSystem::Deregister(Sprite *s) {
	sprites.erase(std::find(sprites.begin(), sprites.end(), s));
}

void SpriteSystem::OneFrame(float delta) {
	GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
	//Assume only 1 batch now.
	if(sprites.size() == 0) {
		return;
	}
	SpriteInstance *instances = new SpriteInstance[kSpriteBatchSize];
	uint32_t count = 0;
	for(uint32_t i=0; i<sprites.size(); i++) {
		if(sprites[i]->IsEnabled()) {
			memcpy(instances+count, sprites[i]->GetData(), sizeof(SpriteInstance));
			count++;
		}
	}

	instance_buffer->WriteMap(pipeline);
	instance_buffer->Write(0, instances, count);
	instance_buffer->WriteUnmap();
	
	delete[] instances;

	
	pipeline->Start();
		pipeline->SetRenderTarget(0, Engine::GetSingleton()->GetRendererContext()->GetBackBuffer()->AsRenderTarget());
		pipeline->SetVertexShader(vs);
		pipeline->SetPixelShader(ps);
		pipeline->SetVertexBuffer(0,0,vertex_buffer->AsVertexBuffer());
		pipeline->SetVertexBuffer(1,1,instance_buffer->AsVertexBuffer());
		pipeline->SetPrimitiveTopology(GraphicPipeline::TRIANGLE_STRIP);
		pipeline->DrawInstance(&drawing_state, 0, 4, 0, sprites.size());
	pipeline->End();
}


}

