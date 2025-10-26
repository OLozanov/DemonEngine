#pragma once

#include "Game/Objects/Breakable.h"

class FileStream;

namespace GameLogic
{

class Container : public Breakable
{
public:

    enum Item : uint8_t
    {
        None = 0,
        Medkit = 1,
    };

private:
    uint32_t m_type;
    Item m_item;

    static const std::string ContainerModel[];
    static const float ContainerMass[];
    static const float ContainerSolidity[];

public:
    Container(const vec3& pos, const mat3& rot, uint32_t type, Item item);

    Item item() { return m_item; }
    uint32_t type() override { return object_container; }

    void save(FileStream& stream) override;
    void restore(FileStream& stream);
};

} // namespace gamelogic
