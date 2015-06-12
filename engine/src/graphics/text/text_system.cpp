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

void TextSystem::Register(Text *text) {
	CHECK_NOTNULL(text);
	data.push_back(text);
}

void TextSystem::Deregister(Text *text) {
	auto it = std::find(data.begin(), data.end(), text);
	if(it != data.end()) {
		data.erase(it);
	}
}

namespace {

uint32_t ColorToUnsignedInt(const Vector4 &color) {
	 uint32_t result = 0;
	 result |= (uint32_t)(color[0]*0xff)<<24;
	 result |= (uint32_t)(color[1]*0xff)<<16;
	 result |= (uint32_t)(color[2]*0xff)<<8;
	 result |= (uint32_t)(color[3]*0xff);
	 return result;
}

Matrix4x4 CalculateCascadeTranslateRotateMatrix(Entity *e) {
	Matrix4x4 result;
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
		D3D11GraphicPipeline *manager = NiceCast(D3D11GraphicPipeline *, _pipeline);
		CHECK_NOTNULL(manager);

		

		//The restore flag here is a little bit expensive, better to have our ownway of restoring state.
		uint32_t flag = FW1_RESTORESTATE | FW1_NOGEOMETRYSHADER;

		uint32_t color_uint = ColorToUnsignedInt(cur->GetColor());

		RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
		float w_height = (float)setting.window_height, w_width = (float)setting.window_width;
		
		auto bound = cur->GetBoundingBox();
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

		//TODO: Should only compute translate and rotate matrix.
		Matrix4x4 transform_matrix = CalculateCascadeTranslateRotateMatrix(cur->GetEntity());
		Matrix4x4 orth(
			2.0f/w_width, 0.0f, 0.0f, -1.0f,
			0.0f, -2.0f/w_height, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f ,0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			);
		Matrix4x4 shift_back;
		shift_back.SetTranslate(w_width/2.0f - center_x, w_height/2.0f - center_y, cur->GetDepth());
		transform_matrix *=  orth;
		transform_matrix *= shift_back;
		transform_matrix.Transpose();

		font_wrapper->DrawTextLayout(
			manager->GetDeviceContext(), 
			cur->GetLayout(), 
			0.0f, 0.0f, 
			color_uint, 
			rect, 
			transform_matrix.data[0],
			flag);
		delete rect;
	}
}


}

