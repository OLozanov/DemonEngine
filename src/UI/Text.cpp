//#include "UI\Canvas.h"
//#include <string>

#include "Text.h"

#include "UI/Font.h"
#include "UI/Canvas.h"

#include "System/Keys.h"

namespace UI
{

Text::Text(short x, short y, short width)
: m_font(0)
, m_car1(MinLength)
, m_car2(0)
, m_sel1(0)
, m_sel2(0)
, m_length(MinLength)
, m_flags(0)
, m_x(x)
, m_y(y)
, m_width(width)
{
}

Text::Text(const std::string & txt, short x, short y, short width)
: m_font(0)
, m_car1(MinLength)
, m_car2(0)
, m_sel1(0)
, m_sel2(0)
, m_length(MinLength)
, m_flags(0)
, m_x(x)
, m_y(y)
, m_width(width)
, m_text(txt)
{
}

void Text::setText(const std::string& text)
{ 
    m_text = text;
    m_sel1 = m_text.size();

    m_car1 = m_font->textWidth(m_text);
}

void Text::erase()
{
    int len = m_text.length();
    if(len != 0) m_text.erase(len-1, 1);
}

void Text::input(char chr)
{
    if (isprint(chr))
    {
        m_text.insert(m_sel1, 1, chr);
    
        short wd = m_font->charWidth(chr);

        m_car1 += wd;
        m_length += wd;
        m_sel1++;
    }
}

void Text::clear()
{
    m_sel1 = 0;
    m_car1 = MinLength;
    m_text.clear();
}

void Text::draw(Canvas& canvas)
{
    canvas.text(m_x, m_y, m_text);
}

void Text::onKeyPress(long key)
{
    if(key == KeyBack)
    {
        if(m_sel1 != 0)
        {
            m_sel1--;

            short wd = m_font->charWidth((char)m_text[m_sel1]);
            m_car1 -= wd;
            m_length -= wd;

            m_text.erase(m_sel1, 1);
        }

        return;
    }

    if((key == KeyDelete) && (m_sel1 != m_text.length()))
    {
        m_length -= m_font->charWidth((char)m_text[m_sel1]);
        m_text.erase(m_sel1, 1);

        return;
    }

    if((key == KeyLeft) && (m_sel1 != 0))
    {
        m_sel1--;
        m_car1 -= m_font->charWidth((char)m_text[m_sel1]);

        return;
    }

    if((key == KeyRight) && (m_sel1 != m_text.length()))
    {
        m_car1 += m_font->charWidth((char)m_text[m_sel1]);
        m_sel1++;

        return;
    }

    /*if((key < 0xFF) && isprint(key))
    {
        short wd = m_font->charWidth((char)key);

        if(m_length + wd > m_width) return;

        m_text.insert(m_sel1, 1, (char)key);

        m_car1 += wd;
        m_length += wd;

        m_sel1++;
    }*/

    //m_car1 = m_font->TextWidth((char*)m_text.c_str(), m_text.length());
    //SetCaretPos(2 + m_text.m_car1, 2);
}

/*void Text::onKeyUp(long key)
{
    if(key == VK_LSHIFT)
    {
        m_flags &= ~CFLAGS_LSHIFT;
        return;
    }

    if(key == VK_RSHIFT)
    {
        m_flags &= ~CFLAGS_RSHIFT;
        return;
    }
}*/

void Text::onClick(short x)
{
    if(x < 0) return;
    //if(x > width) return;

    m_car1 = 3;
    m_sel1 = 0;

    for(char ch : m_text)
    {
        long wd = m_font->charWidth(ch);

        if((m_car1+wd-2) <= x)
        {
            m_car1 += wd;
            m_sel1++;
        }
        else break;
    }
}

} //namespace ui