#pragma once

#include "Physics/Constraint.h"

namespace Physics
{

// Special constraint for vehicle suspension
class Suspension : public Constraint
{
public:
    Suspension(RigidBody* body, const vec3& point, unsigned long layers,
               float wheelRadius, float wheelFriction = 0.8f, float rollResistance = 0.1f,
               float length = 0.08f, float stiffness = 50.0f, float damping = 15.0f);

    void setHandbrake(bool brake) { m_handbrake = brake; }

    void setSteering(float ang) { m_steering = ang; }
    void setMotor(float motor) { m_motor = motor; }

    float suspensionDist() const { return m_dist; }
    float wheelSpeed() const { return m_wheelSpeed; }

    const mat3& orientation() const { return m_orientation; }

    void evaluate() override;

private:
    vec3 m_connectPt;

    unsigned long m_layers;

    float m_wheelRadius;
    float m_wheelFriction;
    float m_rollResistance;

    float m_length;
    float m_stiffness;
    float m_damping;

    float m_motor;
    float m_steering;

    bool m_handbrake;

    float m_wheelSpeed;
    float m_dist;

    mat3 m_orientation;

    static constexpr float RestThreshold = 0.025f; // 0.012f is enough on level surface
};

} //namespace Physics
