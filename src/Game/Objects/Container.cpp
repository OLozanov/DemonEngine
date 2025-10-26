#include "Container.h"

#include "Resources/Resources.h"
#include "Utils/FileStream.h"

namespace GameLogic
{

const std::string Container::ContainerModel[] = {"Containers/crate.msh",
                                                 "Containers/crate_small.msh",
                                                 "Containers/barrel.msh"};

const float Container::ContainerMass[] = {160, 30, 80};
const float Container::ContainerSolidity[] = {300, 150, 250};

Container::Container(const vec3& pos, const mat3& rot, uint32_t type, Item item)
: Breakable(pos, rot, ContainerSolidity[type], ContainerMass[type], 
            ResourceManager::GetModel(ContainerModel[type]),
            ResourceManager::GetSound("Wood/wood_bump.wav"),
            ResourceManager::GetSound("Wood/wood_hit.wav"),
            ResourceManager::GetSound("Wood/wood_crash.wav"))
, m_type(type)
, m_item(item)
{
}

void Container::save(FileStream & file)
{
    file << m_type;
    file << m_item;
    file << m_solidity;
    PhysicsObject::save(file);
}

void Container::restore(FileStream & file)
{
    file >> m_solidity;
    PhysicsObject::restore(file);
}

} // namespace gamelogic