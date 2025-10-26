#pragma once

#include <string>
#include "stdio.h"

class Lexer
{
    enum class State
    {
        st_start,
        st_id,
        st_num1,
        st_num2,
        st_pt,
        st_str,
        st_div,
        st_com1,
        st_com2,
        st_com3
    };

public:
    enum Token 
    {
        lex_id,
        lex_num,
        lex_string,
        lex_blopen,
        lex_blclose,
        lex_bropen,
        lex_brclose,
        lex_iopen,
        lex_iclose,
        lex_assign,
        lex_coma,
        lex_plus,
        lex_minus,
        lex_mul,
        lex_div,
        lex_pt,
        lex_colon,
        lex_semicolon,
        lex_eof
    };

private:
    static const size_t BufferSize = 16;

    FILE* m_file = nullptr;

    State m_state;
    std::string m_tokenString;

    char m_fbuffer[BufferSize];
    size_t m_fptr;
    size_t m_bnum;

    unsigned int m_line;

public:
    Lexer(const std::string& filename);
    ~Lexer();

    Token read();
    bool match(Token token);
    bool match(Token token, const std::string& value);

    const std::string& tokenValue() { return m_tokenString; }
};