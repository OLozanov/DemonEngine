#pragma once

#include "Game/GameObject.h"
#include "Render/CompositeObject.h"
#include "Physics/PhysicsManager.h"
#include "Physics/RigidBody.h"
#include "Physics/Suspension.h"
#include "Resources/Model.h"

namespace GameLogic
{

struct VehicleParams
{
    vec3 viewPoint;
    vec3 steeringWheelPos;
    float steeringWheelAngle;
    float motorPower;
    float reverseMotorPower;
    float mass;
    float wheelRadius;
    float wheelFriction;
    float rollResistance;
    float suspensionLength;
    float suspensionStiffness;
    float suspensionDamping;
    size_t numWheels;
    const vec3* wheelPos;
};

class DragForceGenerator : public Physics::ForceGenerator
{
public:
    DragForceGenerator(float dragFactor);

    void update(Physics::RigidBody& body, float dt) override;

private:
    const float m_dragFactor;
};

struct WheelParams
{
    float ang = 0.0f;
    float dang = 0.0f;
};

class Vehicle : public GameObject
              , public Usable
              , public Physics::RigidBody
              , public Render::CompositeObject
{
public:
    using OnMountEvent = Event<void(Vehicle*)>;

public:
    Vehicle(const vec3& pos, const mat3& orientation, const VehicleParams& params, Model* model, Model* wheelModel, Model* steerWheelModel);
    ~Vehicle();

    const vec3& viewPoint() { return m_viewPoint; }

    void onCollide(const vec3& normal, float impulse) override;

    void use() override;
    void dismount();

    void input(int key, bool keyDown);
    void update(float dt) override;

    OnMountEvent OnMount;

protected:
    void setupWheels(const VehicleParams& params, Model* wheelModel);

private:
    const vec3 m_viewPoint;

    const vec3 m_steeringWheelPos;
    const float m_steeringWheelAngle;

    const float m_motor;
    const float m_reverseMotor;

    DragForceGenerator m_dragForce;

    bool m_moveForward = false;
    bool m_moveBack = false;
    bool m_turnLeft = false;
    bool m_turnRight = false;

    size_t m_fwdWheel;

    float m_steering;
    float m_steerVelocity;

    std::vector<vec3> m_wheelPos;
    std::vector<WheelParams> m_wheelParams;
    std::vector<Physics::Suspension*> m_suspension;

    Collision::CollisionShape* m_staticCollision;
    Physics::StationaryBody m_staticBody;           // for player collision

    static constexpr float SteerAng = 15.0f / 180.0f * math::pi;
    static constexpr float SteerVel = 30.0f / 180.0f * math::pi;
};

} // namespace GameLogic