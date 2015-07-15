#include "text_system.h"
#include "text.h"
#include "engine.h"
#include "entity/entity.h"
#include "entity/transform.h"

#include "graphics/renderer/imp/directx11/d3d11_graphic_resource_manager.h"
#include "graphics/renderer/imp/directx11/d3d11_graphic_pipeline.h"

#define WIN32_LEAN_AND_MEAN
#include <FW1FontWrapper.h>
#undef ERROR

#include <glog/logging.h>

#include <algorithm>

#ifdef NDEBUG
	#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
	#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif

namespace s2 {

TextSystem::TextSystem() : factory(0), font_wrapper(0), dwrite_factory(0) {
	HRESULT result = 1;
	result = FW1CreateFactory(FW1_VERSION, &factory);
	CHECK(!FAILED(result))<<"Cannot create FW1Factory.";

	GraphicResourceManager *_manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
	D3D11GraphicResourceManager *manager = NiceCast(D3D11GraphicResourceManager *, _manager);
	CHECK_NOTNULL(manager);
	
	result = factory->CreateFontWrapper(manager->GetDevice(), L"Arial", &font_wrapper);
	CHECK(!FAILED(result))<<"Cannot create FW1FontWrapper.";

	result = font_wrapper->GetDWriteFactory(&dwrite_factory);
	CHECK(!FAILED(result))<<"Cannot get DWriteFactory.";
}

TextSystem::~TextSystem() {
	dwrite_factory->Release();
	font_wrapper->Release();
	factory->Release();
}

static const char * kComponentDestroyCallbackName = "text_system_component_destroy_cb";

void TextSystem::Register(Text *t) {
	CHECK_NOTNULL(t);
	data.push_back(t);
	t->AddDestroyCallback(kComponentDestroyCallbackName, [this](Component *c){
		for(auto it=this->data.begin(); it!=this->data.end(); it++) {
			if((*it)->GetId() == c->GetId()) {
				this->data.erase(it);
				break;
			}
		}
	});
}

void TextSystem::Deregister(Text *t) {
	auto it = std::find(data.begin(), data.end(), t);
	if(it != data.end()) {
		data.erase(it);
		(*it)->RemoveDestroyCallback(kComponentDestroyCallbackName);
	}
}

namespace {

uint32_t ColorToUnsignedInt(const S2Vector4 &color) {
	 uint32_t result = 0;
	 result |= (uint32_t)(color[0]*0xff)<<24;
	 result |= (uint32_t)(color[1]*0xff)<<16;
	 result |= (uint32_t)(color[2]*0xff)<<8;
	 result |= (uint32_t)(color[3]*0xff);
	 return result;
}

S2Matrix4x4 CalculateCascadeTranslateRotateMatrix(Entity *e) {
	S2Matrix4x4 result;
	if(e->GetParent()) {
		result = CalculateCascadeTranslateRotateMatrix(e->GetParent());
	}

	result *= e->GetTransform()->CalculateTranslateMatrix();
	result *= e->GetTransform()->CalcualteRotateMatrix();
	return result;
}

}

void TextSystem::OneFrame(float delta) {
	for(auto it=data.begin(); it!=data.end(); it++) {
		Text *cur = *it;
		if(!cur->IsEnabled() || cur->GetContent() == "") {
			continue;
		}

		GraphicPipeline *_pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		D3D11GraphicPipeline *pipeline = NiceCast(D3D11GraphicPipeline *, _pipeline);
		CHECK_NOTNULL(pipeline);

		//The restore flag here is a little bit expensive, better to have our ownway of restoring state.
		uint32_t flag = FW1_NOGEOMETRYSHADER;

		uint32_t color_uint = ColorToUnsignedInt(cur->GetColor());

		RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
		float w_height = (float)setting.window_height, w_width = (float)setting.window_width;
		
		auto bound = cur->GetBoundingBox(this);
		float left=std::get<0>(bound), right=std::get<1>(bound), top=std::get<2>(bound), bottom=std::get<3>(bound);
		float center_x = (left+right)/2.0f, center_y = (top+bottom)/2.0f;
		
		FW1_RECTF *rect = 0;
		if(cur->isClipped) {
			flag |= FW1_CLIPRECT;
			rect = new FW1_RECTF;
			
			float half_clipper_width = cur->GetAbsoluteClipperWidth()/2.0f,  half_clipper_height = cur->GetAbsoluteClipperHeight()/2.0f;
			rect->Left = center_x - half_clipper_width;
			rect->Top = center_y - half_clipper_height;
			rect->Right = center_x + half_clipper_width;
			rect->Bottom = center_y + half_clipper_height;
		}

		S2Matrix4x4 transform_matrix = CalculateCascadeTranslateRotateMatrix(cur->GetEntity());
		//Orth project screen coordinates(with left-top corner being [0,0], right-bottom corner being [w, h]) to NDC.
		S2Matrix4x4 orth(
			2.0f/w_width, 0.0f, 0.0f, -1.0f,
			0.0f, -2.0f/w_height, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f ,0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			);
		S2Matrix4x4 shift_back;
		shift_back.SetTranslate(w_width/2.0f - center_x, w_height/2.0f - center_y, cur->GetDepth());
		transform_matrix *=  orth;
		transform_matrix *= shift_back;
		//By default, FW1 use column major matrix. But our system uses row major matrix. Transpose to be compatible.
		transform_matrix.Transpose();

		pipeline->PushState();

		pipeline->Start();
		pipeline->SetRenderTarget(0, Engine::GetSingleton()->GetRendererContext()->GetBackBuffer()->AsRenderTarget());
		font_wrapper->DrawTextLayout(
			pipeline->GetDeviceContext(), 
			cur->GetLayout(this), 
			0.0f, 0.0f, 
			color_uint, 
			rect, 
			transform_matrix.data[0],
			flag);
		
		pipeline->PopState();
		pipeline->End();
		delete rect;
	}
}


}

