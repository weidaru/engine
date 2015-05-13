#include "text_system.h"
#include "text.h"
#include "engine.h"
#include "entity/entity.h"

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

unsigned int ColorToUnsignedInt(const Vector4 &color) {
	 unsigned int result = 0;
	 result |= (unsigned int)(color[0]*0xff)<<24;
	 result |= (unsigned int)(color[1]*0xff)<<16;
	 result |= (unsigned int)(color[2]*0xff)<<8;
	 result |= (unsigned int)(color[3]*0xff);
}

}

void TextSystem::OneFrame(float delta) {
	for(auto it=data.begin(); it!=data.end(); it++) {
		Text *cur = *it;

		GraphicPipeline *_pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		D3D11GraphicPipeline *manager = NiceCast(D3D11GraphicPipeline *, _pipeline);
		CHECK_NOTNULL(manager);

		unsigned int flag = FW1_CENTER | FW1_VCENTER;
		FW1_RECTF *rect = 0;
		if(cur->isClipped) {
			flag |= FW1_CLIPRECT;
			rect = new FW1_RECTF;
		}

		unsigned int color_uint = ColorToUnsignedInt(cur->GetColor);

		Matrix4x4 transform_matrix = cur->GetEntity()->GetCascadeTransformMatrix();

		font_wrapper->DrawTextLayout(
			manager->GetDeviceContext(), 
			cur->GetLayout(), 
			0, 0, 
			color_uint, 
			rect, 
			transform_matrix.data[0],
			flag);
	}
}


}

