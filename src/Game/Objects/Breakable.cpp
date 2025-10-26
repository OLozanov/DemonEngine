#include "Breakable.h"

#include "Resources/Resources.h"
#include "System/AudioManager.h"

namespace GameLogic
{

Breakable::Breakable(const vec3& pos, const mat3& rot, float solidity, float mass, Model* model, Sound* bumpSound, Sound* hitSound, Sound* breakSound)
: PhysicsObject(pos, rot, mass, CollisionLayers, model)
, m_solidity(solidity)
, m_hitSound(hitSound)
, m_breakSound(breakSound)
{
    m_bumpSnd = bumpSound;
}

Breakable::~Breakable()
{
}

void Breakable::hit(const vec3& point, const vec3& direction, uint32_t power)
{
    vec3 localPoint = (point - RigidBody::m_pos) * RigidBody::m_orientation;
    vec3 localDir = (direction * RigidBody::m_orientation);

    applyImpulse(localDir * 50, localPoint);
    takeDamage(power, point);
}

void Breakable::takeDamage(uint32_t damage, const vec3& impactPt)
{
    if (m_solidity > damage)
    {
        m_solidity -= damage;
        if (m_hitSound) AudioManager::Play(m_hitSound, impactPt);
    }
    else
    {
        if (m_breakSound) AudioManager::Play(m_breakSound, impactPt);
        OnDestroy(this);
    }
}

} // namespace gamelogic