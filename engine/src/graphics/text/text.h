#ifndef TEXT_H_
#define TEXT_H_

#include "entity/component.h"
#include "utils/s2string.h"
#include "utils/s2vector4.h"

#include <tuple>
#include <stdint.h>

struct IDWriteTextLayout;

namespace s2 {

class TextSystem;

class Text : public Component {
public:
	enum WrapMode {
		/// Words are broken across lines to avoid text overflowing the layout box.
		WRAP = 0,

		/// Words are kept within the same line even when it overflows the layout box.
		/// This option is often used with scrolling to reveal overflow text. 
		/// This is the default behavior.
		NO_WRAP = 1,

		/// Words are broken across lines to avoid text overflowing the layout box.
		/// Emergency wrapping occurs if the word is larger than the maximum width.
		EMERGENCY_BREAK = 2,

		/// Only wrap whole words, never breaking words (emergency wrapping) when the
		/// layout width is too small for even a single word.
		WHOLE_WORD = 3,

		/// Wrap between any valid characters clusters.
		CHARACTER = 4,
	};

public:
	Text(Entity *entity);
	virtual ~Text() override;

	Text & SetFontName(const s2string &_font_name);
	const s2string & GetFontName() const { return font_name; }

	Text & SetFontSize(uint32_t size);
	uint32_t GetFontSize() const { return font_size; }

	Text & SetContent(const s2string &_content);
	const s2string & GetContent() const { return content; }

	Text & SetColor(const S2Vector4 &_color) { color=_color; return *this; }
	const S2Vector4 & GetColor() const { return color;}

	Text & SetClipperWidth(float new_value);
	float GetClipperWidth() const;
	Text & SetClipperHeight(float new_value);
	float GetClipperHeight() const;

	Text & SetAbsoluteClipperWidth(float new_value);
	float GetAbsoluteClipperWidth() const;
	Text & SetAbsoluteClipperHeight(float new_value);
	float GetAbsoluteClipperHeight() const;

	Text & SetWarpMode(WrapMode new_value);
	WrapMode GetWrapMode() const;

	Text & SetClipped(bool new_value);
	bool IsClipped() const;

	Text & SetDepth(float _depth);
	float GetDepth() const;

	//(LEFT, RIGHT, TOP, BOTTOM)
	std::tuple<float, float, float, float> GetBoundingBox(TextSystem *system);

private:
	IDWriteTextLayout *GetLayout(TextSystem *system);

private:
	friend class TextSystem;

private:
	IDWriteTextLayout *layout;
	bool need_new_layout;

	s2string content;
	s2string font_name;
	uint32_t font_size;
	S2Vector4 color;
	bool is_clipped;
	std::pair<float, float> clipper_size;
	float depth;
	WrapMode wrap_mode;
};

}


#endif			//TEXT_H_