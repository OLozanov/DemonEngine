#include "EditConsole.h"

#include "Utils/Lexer.h"

static const std::map<std::string, TypeInfoMember::Type> TypesMap = { { "bool", TypeInfoMember::Type::Bool },
                                                                      { "integer", TypeInfoMember::Type::Integer },
                                                                      { "float", TypeInfoMember::Type::Float },
                                                                      { "vector", TypeInfoMember::Type::Vec3 },
                                                                      { "string", TypeInfoMember::Type::String },
                                                                      { "color", TypeInfoMember::Type::Color },
                                                                      { "model", TypeInfoMember::Type::Model },
                                                                      { "sound", TypeInfoMember::Type::Sound } };

EntityMemberValue DefaultValue(TypeInfoMember::Type type)
{
    switch(type)
    {
    case TypeInfoMember::Type::Bool: return EntityMemberValue(false);
    case TypeInfoMember::Type::Integer: return EntityMemberValue(0);
    case TypeInfoMember::Type::Float: return EntityMemberValue(0.0f);
    case TypeInfoMember::Type::Vec3: return EntityMemberValue(vec3(0.0f, 0.0f, 0.0f));
    case TypeInfoMember::Type::String: return EntityMemberValue("");
    case TypeInfoMember::Type::Color: return EntityMemberValue(vec3(1.0f, 1.0f, 1.0f));
    case TypeInfoMember::Type::Model: return EntityMemberValue("");
    case TypeInfoMember::Type::Enum: return EntityMemberValue(uint8_t(0));
    default: return EntityMemberValue(vec3(0.0f, 0.0f, 0.0f));
    }
}

EntityMemberValue DefaultValue(const EnumInfo* enumInfo)
{
    return EntityMemberValue((*enumInfo)[0].value);
}

EntityMemberValue ParseValue(Lexer& lexer, TypeInfoMember::Type type)
{
    if (type == TypeInfoMember::Type::Bool)
    {
        Lexer::Token tk = lexer.read();
        if (tk != Lexer::lex_id) throw std::exception{ "syntax error" };

        const std::string& id = lexer.tokenValue();

        if (id == "false") return EntityMemberValue(false);
        if (id == "true") return EntityMemberValue(true);

        throw std::exception{ "syntax error" };
    }

    if (type == TypeInfoMember::Type::Integer)
    {
        Lexer::Token tk = lexer.read();
        if (tk != Lexer::lex_num) throw std::exception{ "syntax error" };

        int32_t num = std::atoi(lexer.tokenValue().c_str());

        return EntityMemberValue(num);
    }

    if (type == TypeInfoMember::Type::Float)
    {
        Lexer::Token tk = lexer.read();
        if (tk != Lexer::lex_num) throw std::exception{ "syntax error" };

        float num = std::atof(lexer.tokenValue().c_str());

        return EntityMemberValue(num);
    }

    if (type == TypeInfoMember::Type::Vec3 || type == TypeInfoMember::Type::Color)
    {
        if (!lexer.match(Lexer::lex_blopen)) throw std::exception{ "syntax error" };

        Lexer::Token tk = lexer.read();
        if (tk != Lexer::lex_num) throw std::exception{ "syntax error" };

        float x = std::atof(lexer.tokenValue().c_str());

        if (!lexer.match(Lexer::lex_coma)) throw std::exception{ "syntax error" };

        tk = lexer.read();
        if (tk != Lexer::lex_num) throw std::exception{ "syntax error" };

        float y = std::atof(lexer.tokenValue().c_str());

        if (!lexer.match(Lexer::lex_coma)) throw std::exception{ "syntax error" };

        tk = lexer.read();
        if (tk != Lexer::lex_num) throw std::exception{ "syntax error" };

        float z = std::atof(lexer.tokenValue().c_str());

        if (!lexer.match(Lexer::lex_blclose)) throw std::exception{ "syntax error" };

        return EntityMemberValue(vec3(x, y, z));
    }

    if (type == TypeInfoMember::Type::String || 
        type == TypeInfoMember::Type::Model || 
        type == TypeInfoMember::Type::Sound)
    {
        Lexer::Token tk = lexer.read();
        if (tk != Lexer::lex_string) throw std::exception{ "syntax error" };
    
        const std::string& value = lexer.tokenValue();

        size_t size = value.size() + 1;

        if (size == 0) return EntityMemberValue("");

        char* str = new char[size];
        memcpy(str, value.c_str(), size);

        return EntityMemberValue(str);
    }

    return EntityMemberValue(false);
}

EntityMemberValue ParseValue(Lexer& lexer, const EnumInfo* enumInfo)
{
    Lexer::Token tk = lexer.read();
    if (tk != Lexer::lex_id) throw std::exception{ "syntax error" };

    const std::string& value = lexer.tokenValue();

    auto it = std::find_if(enumInfo->begin(), enumInfo->end(), [value](const EnumValue& enumValue){
        
        if (enumValue.name == value) return true;
        
        return false;
    });

    if (it != enumInfo->end())
    {
        return EntityMemberValue(it->value);
    }

    return EntityMemberValue(0);
}

EntityMember ParseMemeber(Lexer& lexer)
{
    std::string name = lexer.tokenValue();

    if (!lexer.match(Lexer::lex_colon)) throw std::exception{ "syntax error" };

    Lexer::Token tk = lexer.read();
    if (tk != Lexer::lex_id) throw std::exception{ "syntax error" };

    std::string id = lexer.tokenValue(); 

    auto it = TypesMap.find(id);
    if (it != TypesMap.end())
    {
        TypeInfoMember::Type type = it->second;

        tk = lexer.read();

        EntityMemberValue value = tk == Lexer::lex_assign ? ParseValue(lexer, type) : DefaultValue(type);

        return { name, type, nullptr, value };
    }
    else
    {
        const EnumInfo* enumInfo = Entity::GetEnumInfo(id);

        if (!enumInfo) throw std::exception{ "syntax error: unknown type" };

        tk = lexer.read();
    
        EntityMemberValue value = tk == Lexer::lex_assign ? ParseValue(lexer, enumInfo) : DefaultValue(enumInfo);

        return { name, TypeInfoMember::Type::Enum, enumInfo, value };
    }
}

void ConsoleImpl::parseClass(Lexer& lexer)
{
    Lexer::Token tk = lexer.read();
    if (tk != Lexer::lex_id) throw std::exception{ "syntax error" };

    std::string name = lexer.tokenValue();

    tk = lexer.read();
    if (tk != Lexer::lex_num) throw std::exception{ "syntax error" };

    uint16_t classId = std::atoi(lexer.tokenValue().c_str());

    if (!lexer.match(Lexer::lex_blopen)) throw std::exception{ "syntax error" };

    EntityType* entityType = new EntityType;

    entityType->name = name;

    std::string mesh;
    std::string category = "misc";

    while (1)
    {
        Lexer::Token tk = lexer.read();

        // member
        if (tk == Lexer::lex_string)
        {
            entityType->members.push_back(ParseMemeber(lexer));
            continue;
        }

        if (tk == Lexer::lex_id)
        {
            const std::string& id = lexer.tokenValue();
        
            if (id == "mesh")
            {
                if (!lexer.match(Lexer::lex_assign)) throw std::exception{ "syntax error" };

                tk = lexer.read();
                if (tk != Lexer::lex_string) throw std::exception{ "syntax error" };
            
                mesh = lexer.tokenValue();

                continue;
            }

            if (id == "category")
            {
                if (!lexer.match(Lexer::lex_assign)) throw std::exception{ "syntax error" };

                tk = lexer.read();
                if (tk != Lexer::lex_string) throw std::exception{ "syntax error" };

                category = lexer.tokenValue();

                continue;
            }

            throw std::exception{ "syntax error" };
        }

        if (tk == Lexer::lex_blclose) break;

        throw std::exception{ "syntax error" };
    }

    EntityClass entityClass = static_cast<EntityClass>(classId);
    
    m_entityTypes[entityClass] = entityType;

    if (!entityType->members.empty()) Entity::AddEntityType(*entityType, entityClass);
    if (!mesh.empty()) Entity::SetEntityModel(entityClass, mesh);

    auto it = m_entityCategories.find(category);

    wxTreeItemId categoryId = (it != m_entityCategories.end()) ?
                              it->second :
                              m_entityTree->AppendItem(m_entityTreeRoot, category, -1, -1, 0);
    
    m_entityTree->AppendItem(categoryId, name, -1, -1, new ObjectInfo(ObjectType::Entity, entityClass));
}

void ParseEnum(Lexer& lexer)
{
    Lexer::Token tk = lexer.read();
    if (tk != Lexer::lex_id) throw std::exception{ "syntax error" };

    std::string name = lexer.tokenValue();

    EnumInfo* enumInfo = new EnumInfo;

    if (!lexer.match(Lexer::lex_blopen)) throw std::exception{ "syntax error" };

    uint8_t value = 0;

    while (1)
    {
        Lexer::Token tk = lexer.read();

        if (tk == Lexer::lex_id)
        {
            const std::string& vname = lexer.tokenValue();

            char* str = new char[vname.size() + 1];
            memcpy(str, vname.c_str(), vname.size() + 1);
        
            enumInfo->push_back({ str, value++ });
        }

        tk = lexer.read();

        if (tk == Lexer::lex_blclose)
        {
            //if (!lexer.match(Lexer::lex_eof)) return nullptr;
            break;
        }
        else if (tk != Lexer::lex_coma) throw std::exception{ "syntax error" };
    }

    Entity::AddEnum(enumInfo, name);
}

Lexer::Token Restore(Lexer& lexer)
{
    while (1)
    {
        Lexer::Token tk = lexer.read();

        if (tk == Lexer::lex_blclose)
        {
            Lexer::Token tk = lexer.read();
            if (tk == Lexer::lex_id && 
                (lexer.tokenValue() == "class") ||
                (lexer.tokenValue() == "enum")) return tk;
        }

        if (tk == Lexer::lex_id &&
            (lexer.tokenValue() == "class") ||
            (lexer.tokenValue() == "enum")) return tk;

        if (tk == Lexer::lex_eof) return tk;
    }

    return Lexer::lex_eof;
}

void ConsoleImpl::readEntityList(const std::string& filename)
{
        Lexer lexer(filename);

        bool restore = false;

        while (1)
        {
            Lexer::Token tk;

            if (restore)
            {
                tk = Restore(lexer);
                restore = false;
            }
            else tk = lexer.read();

            if (tk == Lexer::lex_eof) break;

            if (tk == Lexer::lex_id)
            {
                try
                {
                    if (lexer.tokenValue() == "class")
                    {
                        parseClass(lexer);
                        continue;
                    }

                    if (lexer.tokenValue() == "enum")
                    {
                        ParseEnum(lexer);
                        continue;
                    }

                    restore = true;
                }
                catch (...)
                {
                    restore = true;
                    continue;
                }
            }

            restore = true;
        }
}