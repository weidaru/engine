#include "text.h"
#include "text_system.h"

#include "engine.h"

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

Text::Text(Entity *entity)
	:	Component(entity),
		layout(0), need_new_layout(true), 
		font_name(TextSystem::GetDefaultFontName()), font_size(32),
		isClipped(false), clipper_size(0.0f, 0.0f),
		depth(0.0f){

}

Text::~Text() {
	if(layout) {
		layout->Release();
	}
}

Text & Text::SetFontName(const s2string &_font_name) {
	font_name = _font_name;
	need_new_layout = true;

	return *this;
}

Text & Text::SetFontSize(uint32_t size) {
	font_size = size;
	need_new_layout = true;

	return *this;
}

Text & Text::SetContent(const s2string &_content) {
	content = _content;
	need_new_layout = true;

	return *this;
}

Text & Text::SetClipperWidth(float new_value) {
	CHECK_GE(new_value, 0.0f);

	clipper_size.first = new_value;
	return *this;
}

float Text::GetClipperWidth() const {
	return clipper_size.first;
}

Text & Text::SetClipperHeight(float new_value) {
	CHECK_GE(new_value, 0.0f);

	clipper_size.second = new_value;
	return *this;
}

float Text::GetClipperHeight() const {
	return clipper_size.second;
}

Text & Text::SetAbsoluteClipperWidth(float new_value) {
	RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_width = (float)setting.window_width;

	return SetClipperWidth(new_value*2.0f/w_width);
}

float Text::GetAbsoluteClipperWidth() const {
	RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_width = (float)setting.window_width;

	return GetClipperWidth() * w_width / 2.0f;
}

Text & Text::SetAbsoluteClipperHeight(float new_value) {
	RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_height = (float)setting.window_height;

	return SetClipperHeight(new_value*2.0f/w_height);
}

float Text::GetAbsoluteClipperHeight() const {
	RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_height = (float)setting.window_height;

	return GetClipperHeight() * w_height / 2.0f;
}

std::tuple<float, float, float, float> Text::GetBoundingBox() {
	GetLayout();
	DWRITE_OVERHANG_METRICS metrics;
	layout->GetOverhangMetrics(&metrics);

	return std::tuple<float, float, float, float>(
		-metrics.left, 
		metrics.right, 
		-metrics.top, 
		metrics.bottom); 
}

Text & Text::SetClipped(bool new_value) {
	isClipped = new_value;
	return *this;
}

bool Text::IsClipped() const {
	return isClipped;
}

Text & Text::SetDepth(float _depth) {
	depth = _depth;
	return *this;
}

float Text::GetDepth() const {
	return depth;
}


IDWriteTextLayout *Text::GetLayout() {
	if(need_new_layout) {	
		TextSystem *system = NiceCast(TextSystem *, GetSystem());
		if(system == 0) {
			return 0;
		}

		if(layout) {
			layout->Release();
		}
		HRESULT result = 1;
		IDWriteFactory *factory = system->GetDWriteFactory();
		IDWriteTextFormat *format;
		result = factory->CreateTextFormat(
			s2wstring(system->GetDefaultFontName().begin(), system->GetDefaultFontName().end()).c_str(),
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			(float)font_size,
			L"",
			&format
		);
		CHECK(!FAILED(result))<<"Fail to create format";

		result = factory->CreateTextLayout(
			s2wstring(content.begin(), content.end()).c_str(),
			content.size(),
			format,
			0.0f,
			0.0f,
			&layout
		);
		CHECK(!FAILED(result))<<"Fail to create layout.";
		layout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

		format->Release();
	}
	need_new_layout = false;

	return layout;
}

}