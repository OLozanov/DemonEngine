#include "Debris.h"

#include "System/AudioManager.h"
#include "Resources/Resources.h"
#include "Utils/FileStream.h"

namespace GameLogic
{

static const std::string DebrisModel[] = { "Tech/scrap_metal.msh",
                                           "Containers/crate_debris_01.msh",
                                           "Containers/crate_debris_02.msh" };

Debris::Debris(const vec3& pos, const mat3& rot, float lifetime, uint32_t type)
: PhysicsObject(pos, rot, 5, collision_debris, ResourceManager::GetModel(DebrisModel[type]))
, m_type(type)
, m_lifetime(lifetime)
{
    if (type > 0) m_bumpSnd = ResourceManager::GetSound("Wood/wood_debris.wav");
    
    m_bumpImpulse = 5;
}

Debris::~Debris()
{
}

void Debris::update(float dt)
{
    if(m_lifetime < dt) OnLifetimeExpires(this);

    m_lifetime -= dt;
    PhysicsObject::update(dt);
}

void Debris::save(FileStream & file)
{
    file << m_type;
    file << m_lifetime;
    PhysicsObject::save(file);
}

void Debris::restore(FileStream & file)
{
    file >> m_lifetime;
    PhysicsObject::restore(file);
}

} // namespace gamelogic
