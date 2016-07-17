#pragma once

#include "engine.h"

#include "renderer_context.h"
#include "graphic_resource_manager.h"
#include "graphic_pipeline.h"


#define DeclareContext() \
	RendererContext *context = Engine::GetSingleton()->GetRendererContext(); \
	GraphicResourceManager *manager = context->GetResourceManager(); \
	GraphicPipeline *pipeline = context->GetPipeline()