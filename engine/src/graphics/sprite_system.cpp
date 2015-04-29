#include "sprite_system.h"

#include "engine.h"

#include "graphics/renderer/all.h"

#include "sprite.h"

//[[TypeInfo]]//
struct SpriteVertex {
	float position[3];
};

namespace s2 {

unsigned int SpriteSystem::kSpriteBatchSize = 400;

SpriteSystem::SpriteSystem() 
		: vs(0), ps(0), vertex_buffer(0), instance_buffer(0), drawing_state(0) {
	auto manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();

	vs = manager->CreateVertexShader();
	vs->Initialize("shader/sprite.vs", "main");

	ps = manager->CreatePixelShader();
	ps->Initialize("shader/sprite.ps", "main");

	vertex_buffer = manager->CreateBuffer();
	Buffer::Option option;
	SpriteVertex verticies[4] = {
		{-0.1f, 0.1f, -1.0f},
		{0.1f, 0.1f, -1.0f},
		{0.1f, -0.1f, -1.0f},
		{-0.1f, -0.1f, -1.0f}
	};
	option.Initialize(4, &verticies);
	option.resource_write = RendererEnum::IMMUTABLE;
	vertex_buffer->Initialize(option);

	instance_buffer = manager->CreateBuffer();
	option.Initialize<SpriteInstance>(kSpriteBatchSize, 0);
	option.resource_write = RendererEnum::CPU_WRITE_FREQUENT;
	instance_buffer->Initialize(option);
}

SpriteSystem::~SpriteSystem() {
	auto manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();

	manager->RemoveBuffer(instance_buffer->GetID());
	manager->RemoveBuffer(vertex_buffer->GetID());
	manager->RemovePixelShader(ps->GetID());
	manager->RemoveVertexShader(vs->GetID());
	delete drawing_state;
}
	
void SpriteSystem::RegisterSprite(Sprite *s) {
	sprites.push_back(s);
}

void SpriteSystem::DeregisterSprite(Sprite *s) {
	sprites.erase(std::find(sprites.begin(), sprites.end(), s));
}

void SpriteSystem::OneFrame(float delta) {
	//Assume only 1 batch now.
	SpriteInstance *instances = new SpriteInstance[kSpriteBatchSize];
	for(unsigned int i=0; i<sprites.size(); i++) {
		memcpy(&instances[i], sprites[i]->GetData(), sizeof(SpriteInstance));
	}

	instance_buffer->WriteMap();
	instance_buffer->Write(0, instances, sprites.size());
	instance_buffer->WriteUnmap();

	GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
	pipeline->Start();
		pipeline->SetVertexShader(vs);
		pipeline->SetPixelShader(ps);
		pipeline->SetVertexBuffer(0,0,vertex_buffer->AsVertexBuffer());
		pipeline->SetVertexBuffer(0,1,instance_buffer->AsVertexBuffer());
		pipeline->SetPrimitiveTopology(GraphicPipeline::TRIANGLE_STRIP);
		pipeline->Draw(&drawing_state);
	pipeline->End();
}


}

