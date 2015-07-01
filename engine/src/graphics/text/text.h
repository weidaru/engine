#ifndef TEXT_H_
#define TEXT_H_

#include "entity/component.h"
#include "utils/s2string.h"
#include "utils/vector4.h"

#include <tuple>
#include <stdint.h>

struct IDWriteTextLayout;

namespace s2 {

class Text : public Component {
public:
	Text(Entity *entity);
	virtual ~Text() override;

	Text & SetFontName(const s2string &_font_name);
	const s2string & GetFontName() const { return font_name; }

	Text & SetFontSize(uint32_t size);
	uint32_t GetFontSize() const { return font_size; }

	Text & SetContent(const s2string &_content);
	const s2string & GetContent() const { return content; }

	Text & SetColor(const Vector4 &_color) { color=_color; return *this; }
	const Vector4 & GetColor() const { return color;}

	Text & SetClipperWidth(float new_value);
	float GetClipperWidth() const;
	Text & SetClipperHeight(float new_value);
	float GetClipperHeight() const;

	Text & SetAbsoluteClipperWidth(float new_value);
	float GetAbsoluteClipperWidth() const;
	Text & SetAbsoluteClipperHeight(float new_value);
	float GetAbsoluteClipperHeight() const;

	Text & SetClipped(bool new_value);
	bool IsClipped() const;

	Text & SetDepth(float _depth);
	float GetDepth() const;

	//(LEFT, RIGHT, TOP, BOTTOM)
	std::tuple<float, float, float, float> GetBoundingBox();

private:
	IDWriteTextLayout *GetLayout();

private:
	friend class TextSystem;

private:
	IDWriteTextLayout *layout;
	bool need_new_layout;

	s2string content;
	s2string font_name;
	uint32_t font_size;
	Vector4 color;
	bool isClipped;
	std::pair<float, float> clipper_size;
	float depth;
};

}


#endif			//TEXT_H_