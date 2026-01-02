#include "Player.h"

#include "Physics/PhysicsManager.h"

#include "Game/Constants.h"

#include "Game/ClimbArea.h"

namespace GameLogic
{

constexpr vec3 Player::BBox;
constexpr vec3 Player::CrouchBBox;
constexpr vec3 Player::ProbeBBox;

Player::Player(const FirstPersonCamera& camera)
: Actor(BBox, {})
, m_collisionProbe(m_orientation, m_probePos, ProbeBBox)
, m_camera(camera)
, m_headDist(Head)
, m_climbArea(nullptr)
{
    m_layers = collision_actor | collision_target;
    m_object = this;
}

Player::Player(const vec3& pos, const FirstPersonCamera& camera)
: Actor(BBox, pos)
, m_collisionProbe(m_orientation, m_probePos, ProbeBBox)
, m_camera(camera)
, m_headDist(Head)
, m_climbArea(nullptr)
{
    m_layers = collision_actor | collision_target;
    m_object = this;
}

void Player::move(float speed, bool side)
{
    if (m_walk || side)
    {
        float ang = m_camera.horizontalAngle();
        if (side) ang += 90.0f;

        ang = ang / 180.0f * math::pi;

        vec3 dir = vec3(sin(ang), 0, cos(ang));

        m_force += dir * speed;
    }
    else
    {
        m_force += m_camera.direction() * speed;
    }

    m_rest = false;
}

void Player::updateForce()
{
    float speed = (m_canjump || !m_walk) ? (m_sitting ? 100 : 200) * m_mass : 
                                           20 * m_mass;

    m_force = {};

    if (m_moveForward)
    {
        move(speed, false);
    }

    if (m_moveBack)
    {
        move(-speed, false);
    }

    if (m_moveRight)
    {
        move(speed, true);
    }

    if (m_moveLeft)
    {
        move(-speed, true);
    }
}

void Player::climb(float dt)
{
    constexpr float climb_speed = 3;

    if (!m_moveForward && !m_moveBack) return;

    const vec3& dir = m_camera.direction();

    if (m_moveForward)
    {
        if (dir.y > 0) m_pos.y += climb_speed * dt;
        else m_pos.y -= climb_speed * dt;
    }

    if (m_moveBack) m_pos.y -= climb_speed * dt;

    const vec3& climbPos = m_climbArea->pos();
    const vec2& norm = m_climbArea->norm();
    float height = m_climbArea->size().y;

    if (m_pos.y > climbPos.y + height + m_bbox.y + 0.2)
    {
        const vec2& norm = m_climbArea->norm();

        m_pos -= vec3(norm.x, 0, norm.y) * 0.1;
        m_velocity = -vec3(norm.x, 0, norm.y) * 3;
        m_acceleration = { 0, -9.8, 0 };

        //m_force = -vec3(norm.x, 0, norm.y) * 200 * m_mass;

        m_climbArea = nullptr;
    }

    if (m_pos.y < climbPos.y - height - m_bbox.y - 0.1) m_climbArea = nullptr;
}

void Player::sit()
{
    if (m_sitting) return;

    m_sitting = true;
    m_tryStandUp = false;
    m_bbox = CrouchBBox;

    updateBBox();

    m_pos -= vec3(0.0f, SitDiff, 0.0f);
    m_headDist = CrouchHead;
}

void Player::standUp()
{
    if (!m_sitting) return;

    m_probePos = m_pos + vec3(0.0f, SitDiff, 0.0f);

    Collision::ContactInfo cinfo;
    if (Physics::PhysicsManager::GetInstance().testCollision(&m_collisionProbe, CrouchBBox, collision_solid, cinfo).first)
    {
        m_tryStandUp = true;
        return;
    }

    m_sitting = false;
    m_tryStandUp = false;
    m_bbox = BBox;

    updateBBox();

    m_pos += vec3(0.0f, SitDiff, 0.0f);
    m_headDist = SitDist;
}

void Player::moveTo(const vec3& pos)
{
    m_pos = pos;
    //m_camera.setPos(m_pos + vec3(0.0f, m_headDist, 0.0f));
}

void Player::fly()
{
    m_walk = false;
    m_acceleration = {};
}

void Player::walk()
{
    m_walk = true;
    m_acceleration = {0, -9.8, 0};
}

void Player::setGhostMode(bool ghost)
{
    m_layers = ghost ? 0 : collision_actor | collision_target;
}

void Player::attachToClimbArea(const ClimbArea* climbArea) 
{
    // Reset crouch position
    if (m_sitting || m_crouch)
    {
        m_sitting = false;
        m_tryStandUp = false;
        m_bbox = BBox;

        updateBBox();

        m_pos += vec3(0.0f, SitDiff, 0.0f);
        m_headDist = Head;
    }

    m_climbArea = climbArea; 

    const vec3& climbPos = m_climbArea->pos();
    const vec2& norm = m_climbArea->norm();
    float dst = m_climbArea->size().z;

    m_pos.x = climbPos.x + norm.x * 0.3;
    m_pos.z = climbPos.z + norm.y * 0.3;
    m_pos.y += 0.1;
    
    m_velocity = {};
    m_acceleration = {};
    m_force = {};
}

void Player::input(int key, bool keyDown)
{
    if (isDead()) return;

    switch (key)
    {
    case 'W':
        m_moveForward = keyDown;
        break;

    case 'S':
        m_moveBack = keyDown;
        break;

    case 'A':
        m_moveLeft = keyDown;
        break;

    case 'D':
        m_moveRight = keyDown;
        break;

    case 'C':
        m_crouch = keyDown;
        if (!keyDown) standUp();

        break;

    case ' ':
        if (!keyDown) break;

        if (m_climbArea)
        {
            const vec2& norm = m_climbArea->norm();

            m_pos += vec3(norm.x, 0, norm.y) * 0.01;

            m_acceleration = { 0, -9.8, 0 };
            m_velocity = vec3(norm.x, 0, norm.y) * 1.5;

            m_climbArea = nullptr;

            m_canjump = false;
        }

        if (m_canjump)
        {
            m_velocity += {0, 5, 0};
            m_canjump = false;
        }
        break;
    }

    //updateForce();
}

void Player::resetInput()
{
    m_moveForward = false;
    m_moveBack = false;
    m_moveLeft = false;
    m_moveRight = false;
    m_crouch = false;
}

void Player::update(float dt)
{
    if (isDead()) return;

    if (!m_climbArea)
    {
        updateForce();

        float dump = (m_canjump || !m_walk) ? 0.02 : 0.002;
        m_velocity -= vec3(m_velocity.x, m_walk ? 0 : m_velocity.y, m_velocity.z) * dump * (dt / 0.0015);

        if (m_tryStandUp) standUp();

        // Standing up
        if (!m_crouch && !m_sitting && m_headDist < Head)
        {
            m_headDist += CrouchSpeed * dt;
            if (m_headDist > Head) m_headDist = Head;
        }

        // Sitting down
        if (m_crouch && !m_sitting)
        {

            m_headDist -= CrouchSpeed * dt;
            if (m_headDist < SitDist) sit();
        }
    }

    //m_camera.setPos(m_pos + vec3(0.0f, m_headDist, 0.0f));
}

void Player::onUpdate(float dt)
{
    if (!m_walk) return;
    
    if (m_climbArea)
    {
        climb(dt);
        return;
    }

    testGroundHeight();
}

void Player::onCollide(const vec3& normal, float impulse)
{
    if (fabs(1 - normal.y) < 0.2)
    {
        m_onfloor = true;
    
        float groundHit = -normal.y * impulse;

        if (groundHit > FallDamageThreshold) damage(groundHit * FallDamageCoefficient);
    }

    //checkCollision = true;
}

void Player::onDeath(uint32_t damage, const vec3& impulse)
{
    //m_camera.setPos(m_pos - vec3(0.0f, 0.35f, 0.0f));
    OnDie(this);
}

} // namespace gamelogic