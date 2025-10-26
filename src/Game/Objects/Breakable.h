#pragma once

#include "Game/Constants.h"
#include "Game/Core/PhysicsObject.h"
#include "Utils/EventHandler.h"
#include "Resources/Resources.h"

class FileStream;

namespace GameLogic
{

class Breakable : public PhysicsObject
{
public:
    using OnHitEvent = Event<void(Breakable&, const vec3&, const vec3&)>;
    using OnDestroyEvent = Event<void(Breakable*)>;

protected:
    uint32_t m_solidity;

    SoundPtr m_hitSound;
    SoundPtr m_breakSound;

    void hit(const vec3& point, const vec3& direction, uint32_t power) override;

    static constexpr unsigned long CollisionLayers = collision_solid | collision_hitable | collision_actor;

public:
    Breakable(const vec3& pos, const mat3& rot,
        float solidity, float mass, Model* model,
        Sound* bumpSound, Sound* hitSound, Sound* breakSound);
    ~Breakable();

    void takeDamage(uint32_t damage, const vec3& impactPt);

    OnDestroyEvent OnDestroy;
};

} // namespace gamelogic