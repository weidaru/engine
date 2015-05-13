#include "text.h"
#include "text_system.h"

#define WIN32_LEAN_AND_MEAN
#include <FW1FontWrapper.h>
#undef ERROR

#include <glog/logging.h>

namespace s2 {

Text::Text(TextSystem *_system)
	:	system(_system), layout(0), need_new_layout(true), 
		font_name(TextSystem::GetDefaultFontName()), font_size(8),
		isClipped(false), clipper_size(0, 0){
	system->Register(this);
}

Text::~Text() {
	system->Deregister(this);
}

Text & Text::SetFontName(const s2string &_font_name) {
	font_name = _font_name;
	need_new_layout = true;

	return *this;
}

Text & Text::SetFontSize(unsigned int size) {
	font_size = size;
	need_new_layout = true;

	return *this;
}

Text & Text::SetContent(const s2string &_content) {
	content = _content;
	need_new_layout = true;

	return *this;
}

IDWriteTextLayout *Text::GetLayout() {
	if(need_new_layout) {
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
			64.0f,
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