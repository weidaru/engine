#include "sprite_system.h"
#include "engine.h"
#include "sprite.h"
#include "asset/asset_path.h"

#include "graphics/renderer/renderer_context.h"
#include "graphics/renderer/graphic_resource_manager.h"
#include "graphics/renderer/graphic_pipeline.h"
#include "graphics/renderer/pixel_shader.h"
#include "graphics/renderer/vertex_shader.h"
#include "graphics/renderer/graphic_buffer.h"
#include "graphics/renderer/input_layout.h"
#include "graphics/renderer/graphic_pipeline_state.h"
#include "graphics/renderer/texture2d.h"
#include "graphics/renderer/sampler.h"
#include "graphics/renderer/shader_data.h"

#include <glog/logging.h>

#include <algorithm>

#ifdef NDEBUG
	#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
	#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif

namespace s2 {

uint32_t SpriteSystem::kSpriteBatchSize = 500;

SpriteSystem::SpriteSystem() 
		:	vs(0), ps(0), sampler(0), ps_data(0),
			instance_buffer(0), input_layout(0), pipeline_state(0),
			instances(0) {

	RendererContext *context = Engine::GetSingleton()->GetRendererContext();
	GraphicResourceManager * manager = context->GetResourceManager();

	vs = manager->CreateVertexShader();
	CHECK(vs->Initialize(ResolveAssetPath("shader/sprite_vs.hlsl"), "main")) << vs->GetLastError();

	ps = manager->CreatePixelShader();
	CHECK(ps->Initialize(ResolveAssetPath("shader/sprite_ps.hlsl"), "main")) << ps->GetLastError();

	GraphicBuffer::Option option;
	instance_buffer = manager->CreateGraphicBuffer();
	option.Initialize<SpriteInstance>(kSpriteBatchSize, 0);
	option.resource_write = RendererResourceWrite::CPU_WRITE_FREQUENT;
	instance_buffer->Initialize(option);

	input_layout = manager->CreateInputLayout();
	CHECK(input_layout->InitializeWithElement(
	{
		{1, 0},		//transform
		{1, 16},
		{1, 32},
		{1, 48},
		{1, 64},	//color
		{1, 80},	//tex
		{1, 88},
		{1, 96},
		{1, 104},
		{2, 0}	//vertex_id, system semantic will be ignored.
	}, 
	*vs))<<input_layout->GetLastError();

	sampler = manager->CreateSampler();
	sampler->Initialize(Sampler::Option());

	ps_data = manager->CreateShaderData();
	CHECK(ps_data->Initialize(ps->GetShaderBytecode()))<<ps_data->GetLastError();
	ps_data->SetSampler("shader_sampler", sampler);

	pipeline_state = context->CreatePipelineState();
	
	pipeline_state->SetVertexShader(vs);
	pipeline_state->SetPixelShader(ps);
	pipeline_state->SetInputLayout(input_layout);

	instances = new SpriteInstance[kSpriteBatchSize];
}

SpriteSystem::~SpriteSystem() {
	GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();

	manager->RemoveGraphicBuffer(instance_buffer->GetID());
	manager->RemovePixelShader(ps->GetID());
	manager->RemoveVertexShader(vs->GetID());
	manager->RemoveSampler(sampler->GetID());
	manager->RemoveShaderData(ps_data->GetID());
	manager->RemoveInputLayout(input_layout->GetID());
	
	delete pipeline_state;

	delete[] instances;
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

void SpriteSystem::RenderOneBatch(SpriteInstance *instances, int count, Texture2D *texture) {
	GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();

	instance_buffer->WriteMap(pipeline);
	instance_buffer->Write(0, instances, count);
	instance_buffer->WriteUnmap();

	if(texture != 0) {
		ps_data->SetShaderResource("shader_texture", texture->AsShaderResource());
	} else {
		ps_data->SetShaderResource("shader_texture", 0);
	}

	pipeline->SetPixelShaderData(ps_data);
	pipeline->DrawInstance(0, 4, 0, count);
	pipeline->SetPixelShaderData(0);
}

void SpriteSystem::OneFrame(float delta) {
	if(sprites.size() == 0) {
		return;
	}

	//Sort based on texture.
	std::map<Texture2D *, std::vector<Sprite *>> sortedMap;
	for(uint32_t i=0; i<sprites.size(); i++) {
		Sprite *cur = sprites[i];
		if(cur != 0 && cur->IsEnabled()) {
			auto it = sortedMap.find(cur->GetTexture());
		
			if(sortedMap.find(cur->GetTexture()) == sortedMap.end()) {
				auto res = sortedMap.insert(std::make_pair(cur->GetTexture(), std::vector<Sprite *>()));
				it = res.first;
			}
			it->second.push_back(cur);
		}
	}

	if(sortedMap.size()==0) {
		return;
	}

	GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
	pipeline->SetState(*pipeline_state);
	pipeline->SetPrimitiveTopology(GraphicPipeline::TRIANGLE_STRIP);
	pipeline->SetRenderTargetAndDepthStencil(
		0, Engine::GetSingleton()->GetRendererContext()->GetBackBuffer()->AsRenderTarget(), 0);
	pipeline->SetVertexBuffer(1,instance_buffer->AsVertexBuffer());

	for(auto it=sortedMap.begin(); it!=sortedMap.end(); it++) {
		Texture2D *texture = it->first;
		std::vector<Sprite *> &spriteCandidates = it->second;

		int count = 0;
		for(uint32_t i=0; i < spriteCandidates.size(); i++) {
			memcpy(instances+count, spriteCandidates[i]->GetData(), sizeof(SpriteInstance));
			count++;

			//Have a batch.
			if(count == kSpriteBatchSize) {
				RenderOneBatch(instances, count, texture);
				count = 0;
			}
		}
		//Render the rest.
		if(count !=0 ) {
			RenderOneBatch(instances, count, texture);
		}
	}
}


}

