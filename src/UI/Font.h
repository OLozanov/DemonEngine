#pragma once

#include "Resources/Resource.h"
#include "stdint.h"

#include <string>

class Image;
class Lexer;

namespace UI
{

class Font
{
	struct Glyph
	{
		uint32_t width = 0;

		uint32_t x = 0;
		uint32_t y = 0;

		uint32_t oy = 0;
		uint32_t ox = 0;
	};

	static constexpr uint32_t GlyphSpace = 2;

	uint32_t m_height;

	uint32_t m_imgWidth;
	uint32_t m_imgHeight;
	ResourcePtr<Image> m_image;

	Glyph m_glyphs[256];

	static Image* ReadImage(Lexer& lexer);
	static bool ReadNum(Lexer& lexer, uint32_t& num);
	static bool LoadGlyph(Lexer& lexer, Font::Glyph* glyphs);

public:

	Font() = default;
	Font(const std::string& fnt);

	long height() const { return m_height; }
	long charWidth(char chr) const;
	long textWidth(const std::string& str) const;

	friend class Canvas;
};

} //namespace ui