#include "Lexer.h"
#include <stdexcept>

Lexer::Lexer(const std::string& filename)
: m_state(State::st_start)
, m_fptr(BufferSize)
, m_line(0)
{
    errno_t error = fopen_s(&m_file, filename.c_str(), "rb");
    if (error) throw std::runtime_error("Can't open file.");
}

Lexer::~Lexer()
{
    fclose(m_file);
}

Lexer::Token Lexer::read()
{
    m_tokenString.clear();

    while (1)
    {
        if (m_fptr == BufferSize)
        {
            m_bnum = fread(m_fbuffer, 1, BufferSize, m_file);
            m_fptr = 0;
        }

        if (m_fptr == m_bnum) return lex_eof;

        char chr = m_fbuffer[m_fptr];

        if (chr == 0x0A) { m_fptr++; continue; }
        if (chr == 0x0D)
        {
            m_line++;
            m_fptr++;

            if (m_state == State::st_com1) m_state = State::st_start;

            continue;
        }

        switch (m_state)
        {
        case State::st_start:

            if (((chr >= 'a') && (chr <= 'z')) ||
                ((chr >= 'A') && (chr <= 'Z')))
            {
                m_tokenString += chr;

                m_state = State::st_id;
                m_fptr++;

                continue;
            }

            if ((chr >= '0') && (chr <= '9') || (chr == '-'))
            {
                m_tokenString += chr;

                m_state = State::st_num1;
                m_fptr++;

                continue;
            }

            if (chr == '.')
            {
                m_tokenString += chr;

                m_state = State::st_pt;
                m_fptr++;

                continue;
            }

            if (chr == '\'')
            {
                m_state = State::st_str;
                m_fptr++;

                continue;
            }

            if (chr == '/')
            {
                m_state = State::st_div;
                m_fptr++;
            }

            if (chr == '{') { m_fptr++; return lex_blopen; }
            if (chr == '}') { m_fptr++; return lex_blclose; }
            if (chr == '(') { m_fptr++; return lex_bropen; }
            if (chr == ')') { m_fptr++; return lex_brclose; }
            if (chr == '[') { m_fptr++; return lex_iopen; }
            if (chr == ']') { m_fptr++; return lex_iclose; }
            if (chr == '=') { m_fptr++; return lex_assign; }
            if (chr == ',') { m_fptr++; return lex_coma; }
            if (chr == '+') { m_fptr++; return lex_plus; }
            if (chr == '-') { m_fptr++; return lex_minus; }
            if (chr == '*') { m_fptr++; return lex_mul; }
            if (chr == ':') { m_fptr++; return lex_colon; }
            if (chr == ';') { m_fptr++; return lex_semicolon; }

            if (chr == ' ') { m_fptr++; continue; }
            if (chr == '	') { m_fptr++; continue; }

            break;

        case State::st_id:

            if (((chr >= 'a') && (chr <= 'z')) ||
                ((chr >= 'A') && (chr <= 'Z')) ||
                ((chr >= '0') && (chr <= '9')) ||
                (chr == '_'))
            {
                m_tokenString += chr;
                m_fptr++;

                continue;
            }

            m_state = State::st_start;
            return lex_id;

            break;

        case State::st_num1:

            if ((chr >= '0') && (chr <= '9'))
            {
                m_tokenString += chr;
                m_fptr++;

                continue;
            }

            if (chr == '.')
            {
                m_state = State::st_num2;

                m_tokenString += chr;
                m_fptr++;

                continue;
            }

            m_state = State::st_start;
            return lex_num;

            break;

        case State::st_num2:

            if ((chr >= '0') && (chr <= '9'))
            {
                m_tokenString += chr;
                m_fptr++;

                continue;
            }

            m_state = State::st_start;
            return lex_num;

            break;

        case State::st_pt:

            if ((chr >= '0') && (chr <= '9'))
            {
                m_state = State::st_num2;

                m_tokenString += chr;
                m_fptr++;

                continue;
            }

            m_state = State::st_start;
            return lex_pt;

            break;

        case State::st_str:

            if (chr == '\'')
            {
                m_fptr++;

                m_state = State::st_start;
                return lex_string;
            }

            m_tokenString += m_fbuffer[m_fptr];
            m_fptr++;

            break;

        case State::st_div:

            if (chr == '/')
            {
                m_state = State::st_com1;
                m_fptr++;

                continue;
            }

            if (chr == '*')
            {
                m_state = State::st_com2;
                m_fptr++;

                continue;
            }

            m_state = State::st_start;
            return lex_div;

            break;

        case State::st_com1:

            m_fptr++;

            break;

        case State::st_com2:

            if (chr == '*') m_state = State::st_com3;
            m_fptr++;

            break;

        case State::st_com3:

            if (chr == '/') m_state = State::st_start;
            else m_state = State::st_com2;

            m_fptr++;

            break;

        }

    }

    return lex_eof;
}

bool Lexer::match(Lexer::Token token)
{
    Token tk = read();

    if (token == tk) return true;
    return false;
}

bool Lexer::match(Lexer::Token token, const std::string& value)
{
    Token tk = read();

    if (token == tk)
    {
        if (token == lex_id) return value == m_tokenString;
        else return true;
    }

    return false;
}