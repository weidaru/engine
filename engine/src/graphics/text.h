#ifndef TEXT_H_
#define TEXT_H_

#include "entity/component.h"
#include "utils/s2string.h"
#include "utils/vector4.h"

#include <utility>

struct IDWriteTextLayout;

namespace s2 {
class TextSystem;

class Text : public Component {
public:
	Text(TextSystem *_system);
	virtual ~Text() override;

	Text & SetFontName(const s2string &_font_name);
	const s2string & GetFontName() const { return font_name; }

	Text & SetFontSize(unsigned int size);
	unsigned int GetFontSize() const { return font_size; }

	Text & SetContent(const s2string &_content);
	const s2string & GetContextName() const { return content; }

	Text & SetColor(const Vector4 &_color) { color=_color; return *this; }
	const Vector4 & GetColor() const { return color;}

	Text & SetClipperSize(unsigned int width, unsigned int height) {
		clipper_size.first=width;
		clipper_size.second=height; 
		return *this; 
	}
	const std::pair<unsigned int, unsigned int> & GetClipperSize() { return clipper_size; }

	Text & SetClipped(bool new_value) { isClipped=new_value; return *this; }
	bool IsClipped() { return isClipped; }

private:
	IDWriteTextLayout *GetLayout();

private:
	friend class TextSystem;

private:
	TextSystem *system;
	IDWriteTextLayout *layout;
	bool need_new_layout;

	s2string content;
	s2string font_name;
	unsigned int font_size;
	Vector4 color;
	bool isClipped;
	std::pair<unsigned int, unsigned int> clipper_size;
};

}


#endif			//TEXT_H_