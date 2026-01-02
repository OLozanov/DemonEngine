#include "Actor.h"

#include "Collision/BoxCollisionShape.h"
#include "Physics/PhysicsManager.h"
#include "Render/SceneManager.h"

#include "Game/Constants.h"

namespace GameLogic
{

Actor::Actor(const vec3& bbox, const vec3& pos, float mass, uint32_t health)
: RigidBody({}, mass, 0.0, 0.01, collision_actor, false, true)
, m_bbox(bbox)
, m_target(nullptr)
, m_health(health)
, m_maxHealth(health)
{
    //m_bbox = vec3(0.25, 0.5, 0.25);
    BBox box = { -m_bbox, m_bbox };

    m_collisionShape = new Collision::BoxCollisionShape(m_orientation, m_pos, m_bbox);
}

void Actor::updateBBox()
{
    static_cast<Collision::BoxCollisionShape*>(m_collisionShape)->setDimensions(m_bbox);
}

void Actor::hit(const vec3& point, const vec3& direction, uint32_t power)
{
    damage(power, power > 30 ? direction * power * 20.0f : vec3());
}

void Actor::update(float dt)
{
    m_pos += m_velocity * dt;
}

void Actor::moveTo(const vec3& location)
{
    m_pos = location;
}

void Actor::damage(uint32_t dmg)
{
    damage(dmg, {});
}

void Actor::damage(uint32_t damage, const vec3& impulse)
{
    if (damage == 0) return;
    if (m_health == 0) return;
    if (m_invulnerable) return;

    if (m_health > damage)
    {
        m_health -= damage;
        OnDamage(this, damage);
    }
    else
    {
        m_health = 0;
        OnDamage(this, damage);
        onDeath(damage, impulse);
    }
}

void Actor::kill()
{
    onDeath(m_health, {});
    m_health = 0;
}

bool Actor::heal(uint32_t health)
{
    if (m_health <= 0) return false;

    if (m_health < m_maxHealth)
    {
        uint32_t hl;

        if ((m_maxHealth - m_health) < health) hl = m_maxHealth - m_health;
        else hl = health;

        m_health += hl;
        return true;
    }

    return false;
}

void Actor::onDeath(uint32_t damage, const vec3& impulse)
{
    OnDie(this);
}

void Actor::testGroundHeight()
{
    auto& pm = Physics::PhysicsManager::GetInstance();

    float height;
    float tilt;

    if (pm.testHeight(m_pos, m_bbox, collision_solid, height, tilt))
    {
        float stepheight = height - (m_pos.y - m_bbox.y);

        constexpr float max = 0.25;
        float min = m_canjump ? -max : 0;

        if (stepheight > min && stepheight < max && tilt > 0.8)
        {
            if (m_velocity.y < -FallDamageThreshold) damage(-m_velocity.y * FallDamageCoefficient);

            m_pos.y += stepheight + 0.001;
            m_velocity.y = 0;
            m_onfloor = true;
            m_surfaceTilt = tilt;
        }
    }

    m_canjump = m_onfloor;
    m_onfloor = false;
}

void Actor::onUpdate(float dt)
{
    testGroundHeight();
}

} // namespace gamelogic