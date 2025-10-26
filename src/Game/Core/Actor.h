#pragma once

#include "Game/GameObject.h"
#include "Physics/RigidBody.h"
#include "Utils/EventHandler.h"

namespace GameLogic
{

class Actor : public GameObject
            , public Hitable
            , public Physics::RigidBody
{
public:
    using ActorEvent = Event<void(Actor*)>;
    using ActorDamageEvent = Event<void(Actor*, uint32_t)>;

public:

    Actor(const vec3& bbox, const vec3& pos);

    const vec3& center() const override { return m_pos; }
    void hit(const vec3& point, const vec3& direction, uint32_t power) override;
    void update(float dt) override;

    const vec3& pos() const { return m_pos; }
    const vec3& bbox() const { return m_bbox; }
    void moveTo(const vec3& location);

    void setTarget(Actor* target) { m_target = target; }

    void damage(uint32_t damage);
    bool heal(uint32_t health);
    bool isDead() { return m_health == 0; }
    void revive() { m_health = m_maxHealth; }

    void makeInvulnerable(bool invulnerable) { m_invulnerable = invulnerable; }
    bool toggleInvulnerability() {return m_invulnerable = !m_invulnerable; }

    virtual void onDeath(uint32_t damage);

    int health() { return m_health; }

    ActorDamageEvent OnDamage;
    ActorEvent OnDie;

protected:
    void updateBBox();
    void onCollide(const vec3& normal, float impulse) override {}

    Actor* m_target;

    vec3 m_bbox;

    bool m_invulnerable;
    uint32_t m_maxHealth;
    uint32_t m_health;
};

} // namespace gamelogic