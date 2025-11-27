#pragma once

#include "Game/GameObject.h"
#include "Game/Camera/FirstPersonCamera.h"

#include "Game/Core/Actor.h"

#include "Collision/BoxCollisionShape.h"

namespace GameLogic
{

class ClimbArea;

class Player : public Actor
{
    const FirstPersonCamera& m_camera;

    Collision::BoxCollisionShape m_collisionProbe;
    vec3 m_probePos;

    bool m_moveForward = false;
    bool m_moveBack = false;
    bool m_moveLeft = false;
    bool m_moveRight = false;
    bool m_crouch = false;

    bool m_walk = true;

    bool m_sitting = false;
    bool m_canjump = true;
    bool m_onfloor = false;
    bool m_tryStandUp = false;

    float m_surfaceTilt;
    float m_headDist;

    const ClimbArea* m_climbArea;

    static constexpr float CrouchSpeed = 2.5f;

    static constexpr float Head = 0.4f;
    static constexpr float CrouchHead = 0.15f;

    static constexpr vec3 BBox = vec3(0.2f, 0.5f, 0.2f);
    static constexpr vec3 CrouchBBox = vec3(0.2f, 0.3f, 0.2f);

    static constexpr float SitDiff = BBox.y - CrouchBBox.y;
    static constexpr float SitDist = -(BBox.y - (CrouchBBox.y + CrouchHead));

    static constexpr vec3 ProbeBBox = vec3(0.199f, SitDiff + 0.05, 0.199f);

    static constexpr float FallDamageThreshold = 10.0f;
    static constexpr float FallDamageCoefficient = 0.3f;

private:

    void move(float speed, bool side);
    void updateForce();

    void climb(float dt);

    void sit();
    void standUp();

    void onUpdate(float dt) override;
    void onCollide(const vec3& normal, float impulse) override;

    void onDeath(uint32_t damage) override;

public:

    Player(const FirstPersonCamera& camera);
    Player(const vec3& pos, const FirstPersonCamera& camera);

    void moveTo(const vec3& pos);

    void fly();
    void walk();
    void setGhostMode(bool ghost);

    bool isWalking() { return m_walk; }

    void attachToClimbArea(const ClimbArea* climbArea);
    void resetClimbArea() { m_climbArea = nullptr; }
    bool isClimbing() { return m_climbArea != nullptr; }

    float headDist() const { return m_headDist; }

    void input(int key, bool keyDown);
    void resetInput();

    void update(float dt);
};

} // namespace gamelogic