#pragma once

#include "Game/Core/Actor.h"
#include "Render/StaticObject.h"
#include "Render/ArticulatedObject.h"
#include "Physics/PhysicsManager.h"
#include "Resources/Resources.h"

#include <random>
#include <memory>

namespace GameLogic
{

class Character : public Actor
                , public Render::ArticulatedObject
{
public:
    struct AnimSet
    {
        uint16_t start;
        uint16_t stop;
    };

    enum State : uint8_t
    {
        Idle = 0,
        Move = 1,
        Attack = 2,
        Dead = 3
    };

    struct CharacterParams
    {
        const std::string& model;
        vec3 bbox;
        float mass;
        float speed;
        uint32_t health;
        uint32_t damage;
        float attackDistance;
        float attackTime;
        int weaponBone;
        vec3 weaponPos;
        vec3 weaponRot;
        AnimSet idleAnim;
        AnimSet moveAnim;
        AnimSet attackAnim;
        AnimSet deathAnim;
    };

    using CharacterEvent = Event<void(Character*, const vec3&)>;

public:
    Character(const vec3& pos, float ang, const CharacterParams& params);
    ~Character();

    void onDeath(uint32_t damage, const vec3& impulse) override;
    void update(float dt) override;

    CharacterEvent OnDeath;

protected:
    void setAnimation(const AnimSet& anim);

private:
    bool targetVisibility();
    void onCollide(const vec3& normal, float impulse) override;

private:
    Collision::CollisionShape* m_staticCollision;
    Physics::StationaryBody m_staticBody;
    
    std::unique_ptr<Render::StaticObject> m_weapon;
    SoundPtr m_shotSound;

    std::vector<AnimSet> m_animations;
    float m_ang;
    float m_timer;

    State m_state;
    
    const float m_damage;
    const float m_attackDistance;
    const float m_attackTime;
    bool m_hit;

    const float m_speed;

    const size_t m_weaponBone;
    const mat4 m_weaponTransform;

    std::mt19937 m_randomGenerator;
    std::uniform_real_distribution<float> m_distribution;
};

} // namespace gamelogic