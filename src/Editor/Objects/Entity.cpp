#include "Entity.h"
#include "ObjectFactory.h"
#include "Resources/Resources.h"

std::map<std::string, const EnumInfo*> Entity::EntityEnums;
std::map<EntityClass, std::string> Entity::ModelMapping;
std::map<EntityClass, const EntityType*> Entity::EntityTypeDesc;
std::map<EntityClass, TypeInfo*> Entity::Types;
std::map<EntityClass, size_t> Entity::TypeSizes;

void Entity::AddEnum(const EnumInfo* enumInfo, const std::string& name)
{
    EntityEnums[name] = enumInfo;
}

const EnumInfo* Entity::GetEnumInfo(const std::string& name)
{
    auto it = EntityEnums.find(name);

    if (it != EntityEnums.end()) return it->second;
    else return nullptr;
}

void Entity::AddEntityType(const EntityType& type, EntityClass eclass)
{
    TypeInfo* typeInfo = new TypeInfo;

    typeInfo->name = type.name.c_str();
    typeInfo->parentTypeInfo = &Object::ObjectTypeInfo;

    size_t offset = 0;

    for (const EntityMember& member : type.members)
    {
        if (member.type == TypeInfoMember::Type::Model)
        {
            typeInfo->members.push_back({ member.name.c_str(), TypeInfoMember::Type::Model, 0, offsetof(Entity, m_data) + offset, offsetof(Entity, m_model) });
            offset += TypeInfoMember::SizeOf(TypeInfoMember::Type::Model);
            continue;
        }

        typeInfo->members.push_back({ member.name.c_str(), member.type, member.enumInfo, offsetof(Entity, m_data) + offset, 0 });
        offset += TypeInfoMember::SizeOf(member.type);
    }

    EntityTypeDesc[eclass] = &type;
    Types[eclass] = typeInfo;
    TypeSizes[eclass] = offset;
}

void Entity::SetEntityModel(EntityClass eclass, const std::string& model)
{
    ModelMapping[eclass] = model;
}

Entity* Entity::CreateEntity(EntityClass entityClass, bool initData)
{
    auto it = Types.find(entityClass);

    if (it == Types.end())
        return new Entity(entityClass);
    else
    {
        TypeInfo * typeInfo = it->second;
        
        const TypeInfoMember& last = typeInfo->members.back();
        size_t size = offsetof(Entity, m_data) + TypeSizes[entityClass];
        size = ceil(size / 4.0f) * 4;

        uint8_t* mem = new uint8_t[size];

        memset(mem, 0, size);

        return new (mem) Entity(entityClass, initData);
    }
}

Entity* Entity::CreateEntity(EntityClass entityClass, FILE* file)
{
    Entity* entity = CreateEntity(entityClass, false);

    vec3 pos;
    mat3 mat;

    fread(&pos, sizeof(vec3), 1, file);
    fread(&mat[0], sizeof(vec3), 1, file);
    fread(&mat[1], sizeof(vec3), 1, file);

    mat[1] -= mat[0] * (mat[0] * mat[1]);   //ortagonize
    mat[2] = mat[0] ^ mat[1];

    entity->setPos(pos);
    entity->setOrientation(mat);

    const TypeInfo* typeInfo = entity->m_typeInfo;

    if (entity->m_typeInfo != nullptr) entity->restore(file);

    return entity;
}

Entity::Entity(EntityClass entityClass, bool initData)
: MeshObject(ResourceManager::GetModel(ModelMapping[entityClass]))
, m_entityClass(entityClass)
, m_typeInfo(nullptr)
{
    auto it = Types.find(entityClass);

    if (it != Types.end())
    {
        m_typeInfo = it->second;
        if (initData) init(entityClass);
    }
}

Entity::~Entity()
{
    if (m_typeInfo != nullptr) destroy();
}

void Entity::init(EntityClass entityClass)
{
    const EntityType* typeDesc = EntityTypeDesc[entityClass];

    for (size_t i = 0; i < typeDesc->members.size(); i++)
    {
        const TypeInfoMember& member = m_typeInfo->members[i];

        if (member.type == TypeInfoMember::Type::Model ||
            member.type == TypeInfoMember::Type::Sound ||
            member.type == TypeInfoMember::Type::String)
        {
            new ((uint8_t*)this + member.offset) (std::string) (typeDesc->members[i].defaultValue.str_val);

            if (member.type == TypeInfoMember::Type::Model)
            {
                std::string* string = reinterpret_cast<std::string*>((uint8_t*)this + member.offset);
                m_model = ResourceManager::GetModel(*string);
            }
        }
        else
            memcpy((uint8_t*)this + member.offset, (uint8_t*)&typeDesc->members[i].defaultValue, TypeInfoMember::SizeOf(member.type));
    }
}

void Entity::restore(FILE* file)
{
    for (const TypeInfoMember& member : m_typeInfo->members)
    {
        if (member.type == TypeInfoMember::Type::Model ||
            member.type == TypeInfoMember::Type::Sound || 
            member.type == TypeInfoMember::Type::String)
        {
            new ((uint8_t*)this + member.offset) (std::string) ();
            std::string* string = reinterpret_cast<std::string*>((uint8_t*)this + member.offset);

            uint16_t size = 0;

            fread(&size, sizeof(uint16_t), 1, file);
            if (size > 0)
            {
                string->resize(size);
                fread(string->data(), 1, size, file);
            }

            if (member.type == TypeInfoMember::Type::Model) m_model = ResourceManager::GetModel(*string);
        }
        else
            fread((uint8_t*)this + member.offset, TypeInfoMember::SizeOf(member.type), 1, file);
    }
}

void Entity::destroy()
{       
    for (const TypeInfoMember& member : m_typeInfo->members)
    {
        if (member.type == TypeInfoMember::Type::String ||
            member.type == TypeInfoMember::Type::Sound ||
            member.type == TypeInfoMember::Type::Model)
        {
            std::string* string = reinterpret_cast<std::string*>((uint8_t*)this + member.offset);
            string->std::string::~string();
        }
    }
}

ObjectType Entity::type() const
{
    return ObjectType::Entity;
}

const TypeInfo& Entity::getTypeInfo() const
{
    if (m_typeInfo != nullptr)
        return *m_typeInfo;
    else
        return Object::ObjectTypeInfo;
}

Object* Entity::clone() const
{
    return new Entity(*this);
}

void Entity::write(FILE* file) const
{
    fwrite(&m_entityClass, 1, sizeof(EntityClass), file);
    fwrite(&m_pos, sizeof(vec3), 1, file);
    fwrite(&m_mat[0], sizeof(vec3), 1, file);
    fwrite(&m_mat[1], sizeof(vec3), 1, file);

    if (m_typeInfo != nullptr)
    {
        for (const TypeInfoMember& member : m_typeInfo->members)
        {
            if (member.type == TypeInfoMember::Type::String ||
                member.type == TypeInfoMember::Type::Sound ||
                member.type == TypeInfoMember::Type::Model)
            {
                std::string* string = reinterpret_cast<std::string*>((uint8_t*)this + member.offset);

                uint16_t size = string->size();

                fwrite(&size, sizeof(uint16_t), 1, file);
                if (size > 0) fwrite(string->data(), 1, size, file);
            }
            else
                fwrite((uint8_t*)this + member.offset, TypeInfoMember::SizeOf(member.type), 1, file);
        }
    }
}