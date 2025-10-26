#include "TypeInfo.h"
#include "Utils/Lexer.h"

#include <iostream>

TypeInfoMember ParseMemeber(Lexer& lexer)
{
    TypeInfoMember member;

    std::string type = lexer.tokenValue();

    if (type == "bool") member.type = TypeInfoMember::Type::Bool;
    if (type == "integer") member.type = TypeInfoMember::Type::Integer;
    if (type == "float") member.type = TypeInfoMember::Type::Float;
    if (type == "vec3") member.type = TypeInfoMember::Type::Vec3;
    if (type == "string") member.type = TypeInfoMember::Type::String;
    if (type == "color") member.type = TypeInfoMember::Type::Color;
    if (type == "model") member.type = TypeInfoMember::Type::Model;

    Lexer::Token tk = lexer.read();

    if (tk != Lexer::lex_id) throw std::exception{"syntax error"};

    std::string name = lexer.tokenValue();

    return member;
}

TypeInfo* ParseClass(Lexer& lexer)
{
    if (!lexer.match(Lexer::lex_id, "class")) return nullptr;

    Lexer::Token tk = lexer.read();
    if (tk != Lexer::lex_id) return nullptr;

    std::string name = lexer.tokenValue();

    if (!lexer.match(Lexer::lex_blopen)) return nullptr;

    TypeInfo* typeInfo = new TypeInfo;

    while (1)
    {
        Lexer::Token tk = lexer.read();

        if (tk == Lexer::lex_id)
        {

        }

        if (tk == Lexer::lex_blclose) break;

        return nullptr;
    }

    return typeInfo;
}

EnumInfo* ParseEnum(Lexer& lexer)
{
    if (!lexer.match(Lexer::lex_id, "enum")) return nullptr;

    Lexer::Token tk = lexer.read();
    if (tk != Lexer::lex_id) return nullptr;

    std::string name = lexer.tokenValue();

    if (!lexer.match(Lexer::lex_blopen)) return nullptr;

    while (1)
    {
        Lexer::Token tk = lexer.read();

        if (tk == Lexer::lex_id)
        {

        }

        if (tk == Lexer::lex_blclose)
        {
            if (!lexer.match(Lexer::lex_eof)) return nullptr;
            break;
        }
    }

    return nullptr;
}

void ParseTypeInfo(const std::string& filename)
{
    try
    {
        Lexer lexer(filename);

        while (1)
        {
            Lexer::Token tk = lexer.read();

            if (tk == Lexer::lex_eof) break;

            if (tk == Lexer::lex_id)
            {
                if (lexer.tokenValue() == "class")
                {
                    ParseClass(lexer);
                    continue;
                }

                if (lexer.tokenValue() == "enum")
                {
                    ParseEnum(lexer);
                    continue;
                }
            }
        }

    }
    catch (...)
    {
        std::cout << "Fail to parse type info " << filename << std::endl;
    }
}