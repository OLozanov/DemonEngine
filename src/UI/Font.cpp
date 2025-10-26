#include "Font.h"

#include "Resources/Resources.h"
#include "Utils/Lexer.h"

#include <stdexcept>

namespace UI
{

Image* Font::ReadImage(Lexer& lexer)
{
    Image* image;

    if (!lexer.match(Lexer::lex_assign)) throw std::runtime_error("syntax error");

    Lexer::Token tk = lexer.read();

    if (tk == Lexer::lex_string)
    {
        std::string tname("Fonts\\");
        tname += lexer.tokenValue();

        image = ResourceManager::GetImage(tname.c_str());
    
    } else throw std::runtime_error("syntax error");

    return image;
}

bool Font::ReadNum(Lexer& lexer, uint32_t& num)
{
    if (!lexer.match(Lexer::lex_assign)) return false;

    Lexer::Token tk = lexer.read();

    if (tk == Lexer::lex_num) num = atoi(lexer.tokenValue().c_str());
    else return false;

    return true;
}

bool Font::LoadGlyph(Lexer& lexer, Font::Glyph* glyphs)
{
    uint32_t code;
    uint32_t width;

    uint32_t x = 0;
    uint32_t y = 0;

    uint32_t oy = 0;
    uint32_t ox = 0;

    Lexer::Token tk = lexer.read();

    if (tk == Lexer::lex_num) code = atoi(lexer.tokenValue().c_str());
    else if (tk == Lexer::lex_string)
    {
        const std::string value = lexer.tokenValue();
        
        if (value.size() != 1) return false;

        code = value[0];
    }
    else return false;

    if (!lexer.match(Lexer::lex_blopen)) return false;

    while (1)
    {
        Lexer::Token tk = lexer.read();

        if (tk == Lexer::lex_id)
        {
            if (lexer.tokenValue() == "width")
            {
                if (!ReadNum(lexer, width)) return false;
                continue;
            }

            if (lexer.tokenValue() == "x")
            {
                if (!ReadNum(lexer, x)) return false;
                continue;
            }

            if (lexer.tokenValue() == "y")
            {
                if (!ReadNum(lexer, y)) return false;
                continue;
            }

            if (lexer.tokenValue() == "ox")
            {
                if (!ReadNum(lexer, ox)) return false;
                continue;
            }

            if (lexer.tokenValue() == "oy")
            {
                if (!ReadNum(lexer, oy)) return false;
                continue;
            }
        }

        if (tk == Lexer::lex_blclose) break;

        return false;
    }

    glyphs[code].width = width;
    glyphs[code].x = x;
    glyphs[code].y = y;
    glyphs[code].oy = oy;
    glyphs[code].ox = ox;

    return true;
}

Font::Font(const std::string& fnt)
{
    std::string fname("Textures\\Fonts\\");
    fname = fname + fnt + ".dcf";

    m_image = nullptr;

    Lexer lexer(fname);

    if (!lexer.match(Lexer::lex_id, "font")) return;
    if (!lexer.match(Lexer::lex_blopen)) return;

    while (1)
    {
        Lexer::Token tk = lexer.read();

        if (tk == Lexer::lex_id)
        {
            if (lexer.tokenValue() == "size")
            {
                if (!ReadNum(lexer, m_height)) throw std::runtime_error("syntax error");
                continue;
            }

            if (lexer.tokenValue() == "image")
            {
                m_image = ReadImage(lexer);
                if (!m_image) throw std::runtime_error("Failed to load font image");

                m_imgWidth = m_image->width;
                m_imgHeight = m_image->height;

                continue;
            }

            if (lexer.tokenValue() == "glyph")
            {
                if (!LoadGlyph(lexer, m_glyphs)) throw std::runtime_error("syntax error");
                continue;
            }
        }

        if (tk == Lexer::lex_blclose)
        {
            if (!lexer.match(Lexer::lex_eof)) throw std::runtime_error("syntax error");
            break;
        }

        throw std::runtime_error("syntax error");
    }
}

long Font::charWidth(char chr) const
{
    return m_glyphs[(unsigned char)chr].width - 2;
}

long Font::textWidth(const std::string& str) const
{
    unsigned long i;
    unsigned long width = 0;

    for (i = 0; i < str.size(); i++)
    {
        unsigned char ch = str[i];
        width += m_glyphs[ch].width - 2;
    }

    width += GlyphSpace * 2;

    return width;
}

} //namespace ui