#include "sprite_system.h"
#include "engine.h"
#include "graphics/renderer/all.h"
#include "sprite.h"
#include "asset/asset_path.h"
#include <glog/logging.h>

#include <algorithm>

#ifdef NDEBUG
	#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
	#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif

namespace s2 {

uint32_t SpriteSystem::kSpriteBatchSize = 400;

SpriteSystem::SpriteSystem() 
		: vs(0), ps(0), vertex_buffer(0), instance_buffer(0), input_layout(0), pipeline_state(0) {

	RendererContext *context = Engine::GetSingleton()->GetRendererContext();
	GraphicResourceManager * manager = context->GetResourceManager();

	vs = manager->CreateVertexShader();
	CHECK(vs->Initialize(ResolveAssetPath("shader/sprite_vs.hlsl"), "main")) << vs->GetLastError();

	ps = manager->CreatePixelShader();
	CHECK(ps->Initialize(ResolveAssetPath("shader/sprite_ps.hlsl"), "main")) << ps->GetLastError();

	vertex_buffer = manager->CreateGraphicBuffer();
	GraphicBuffer::Option option;
	SpriteVertex verticies[4] = {
		{-0.1f, -0.1f, 0.0f},
		{0.1f, -0.1f, 0.0f},
		{-0.1f, 0.1f, 0.0f},
		{0.1f, 0.1f, 0.0f}
	};
	option.Initialize(4, verticies);
	option.resource_write = RendererEnum::IMMUTABLE;
	vertex_buffer->Initialize(option);

	instance_buffer = manager->CreateGraphicBuffer();
	option.Initialize<SpriteInstance>(kSpriteBatchSize, 0);
	option.resource_write = RendererEnum::CPU_WRITE_FREQUENT;
	instance_buffer->Initialize(option);

	input_layout = manager->CreateInputLayout();
	input_layout->InitializeWithElement(
	{
		{0, 0},
		{1, 0},
		{1, 16},
		{1, 32},
		{1, 48},
		{1, 64}
	}, 
	*vs);

	
	pipeline_state = context->CreatePipelineState();
	pipeline_state->SetRenderTarget(0, Engine::GetSingleton()->GetRendererContext()->GetBackBuffer()->AsRenderTarget());
	pipeline_state->SetVertexShader(vs);
	pipeline_state->SetPixelShader(ps);
	pipeline_state->SetVertexBuffer(0,vertex_buffer->AsVertexBuffer());
	pipeline_state->SetVertexBuffer(1,instance_buffer->AsVertexBuffer());
	pipeline_state->SetInputLayout(input_layout);
	pipeline_state->SetPrimitiveTopology(GraphicPipeline::TRIANGLE_STRIP);
}

SpriteSystem::~SpriteSystem() {
	GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();

	manager->RemoveGraphicBuffer(instance_buffer->GetID());
	manager->RemoveGraphicBuffer(vertex_buffer->GetID());
	manager->RemovePixelShader(ps->GetID());
	manager->RemoveVertexShader(vs->GetID());
	manager->RemoveInputLayout(input_layout->GetID());
	delete pipeline_state;
}

static const char * kComponentDestroyCallbackName = "sprite_system_component_destroy_cb";

void SpriteSystem::Register(Sprite *s) {
	CHECK_NOTNULL(s);
	sprites.push_back(s);
	s->AddDestroyCallback(kComponentDestroyCallbackName, [this](Component *c){
		for(auto it=this->sprites.begin(); it!=this->sprites.end(); it++) {
			if((*it)->GetId() == c->GetId()) {
				this->sprites.erase(it);
				break;
			}
		}
	});
}

void SpriteSystem::Deregister(Sprite *s) {
	auto it = std::find(sprites.begin(), sprites.end(), s);
	if(it != sprites.end()) {
		sprites.erase(it);
		(*it)->RemoveDestroyCallback(kComponentDestroyCallbackName);
	}
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

	pipeline_state->Flush(pipeline);
	pipeline->DrawInstance(0, 4, 0, sprites.size());

}


}

