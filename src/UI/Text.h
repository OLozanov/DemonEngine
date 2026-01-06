#pragma once

#include <string>

namespace UI
{

class Font;
class Canvas;

class Text
{
	static constexpr short MinLength = 3;

	int m_sel1;
	int m_sel2;

	uint32_t m_flags;

	short m_length;

	short m_car1;
	short m_car2;

	const Font * m_font;
	std::string m_text;

	short m_x;
	short m_y;

	short m_width;
public:

	Text::Text(short x, short y, short width);
	Text(const std::string& txt, short x, short y, short width);

	const std::string& text() { return m_text; }
	void setText(const std::string& text);

	short caretPos() { return m_car1; }

	void setFont(const Font* font) { m_font = font; }

	void input(char chr);
	void erase();
    void clear();

    void onKeyPress(long key);
    //void onKeyUp(long key);
    void onClick(short x);

	void display(Canvas& canvas);
};

} //namespace ui
