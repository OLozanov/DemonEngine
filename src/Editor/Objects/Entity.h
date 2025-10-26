#pragma once

#include "Object.h"
#include <map>

enum class EntityClass : uint16_t
{
    Crate = 0,
    Barrel = 1,
    SlideDoor = 10,
    Mover = 11,
    Medkit = 100,
    Trooper = 500,
    Invalid = 0xFFFF
};

struct EntityMemberValue
{
    union
    {
        bool bool_val;
        uint8_t byte_val;
        int32_t int_val;
        float float_val;
        vec3 vec_val;
        const char* str_val;
    };

    EntityMemberValue(bool val) { bool_val = val; }
    EntityMemberValue(uint8_t val) { byte_val = val; }
    EntityMemberValue(int32_t val) { int_val = val; }
    EntityMemberValue(float val) { float_val = val; }
    EntityMemberValue(const vec3& val) { vec_val = val; }
    EntityMemberValue(const char* val) { str_val = val; }
};

struct EntityMember
{
    std::string name;
    TypeInfoMember::Type type;

    const EnumInfo* enumInfo = nullptr;

    EntityMemberValue defaultValue;
};

struct EntityType
{
    std::string name;
    std::string parent;

    std::vector<EntityMember> members;
};

class Entity : public MeshObject
{
public:
    static void AddEnum(const EnumInfo* enumInfo, const std::string& name);
    static const EnumInfo* GetEnumInfo(const std::string& name);

    static void AddEntityType(const EntityType& type, EntityClass eclass);
    static void SetEntityModel(EntityClass eclass, const std::string& model);

    static Entity* CreateEntity(EntityClass entityClass, bool initData = true);
    static Entity* CreateEntity(EntityClass entityClass, FILE* file);

    Entity(EntityClass entityClass, bool initData = true);
    ~Entity();

    ObjectType type() const override;
    const TypeInfo& getTypeInfo() const override;
    Object* clone() const override;
    void write(FILE* file) const override;

private:
    void init(EntityClass entityClass);
    void restore(FILE* file);
    void destroy();

private:
    EntityClass m_entityClass;
    const TypeInfo* m_typeInfo;

    uint8_t m_data[1];

    static std::map<std::string, const EnumInfo*> EntityEnums;
    static std::map<EntityClass, std::string> ModelMapping;
    static std::map<EntityClass, const EntityType*> EntityTypeDesc;
    static std::map<EntityClass, TypeInfo*> Types;
    static std::map<EntityClass, size_t> TypeSizes;
};