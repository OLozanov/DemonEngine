#include "Suspension.h"
#include "Physics/PhysicsManager.h"

namespace Physics
{

Suspension::Suspension(RigidBody* body, const vec3& point, unsigned long layers,
                      float wheelRadius, float wheelFriction, float rollResistance,
                      float length, float stiffness, float damping)
: Constraint(3, body, nullptr)
, m_connectPt(point)
, m_layers(layers)
, m_wheelRadius(wheelRadius)
, m_wheelFriction(wheelFriction)
, m_rollResistance(rollResistance)
, m_length(length)
, m_stiffness(stiffness)
, m_damping(damping)
, m_motor(0.0f)
, m_steering(0.0f)
, m_handbrake(true)
{
    for (size_t i = 0; i < m_jacobian.size(); i++)
    {
        m_lowerBound[i] = -std::numeric_limits<float>::infinity();
        m_upperBound[i] = std::numeric_limits<float>::infinity();
    }
}

void Suspension::evaluate()
{
    PhysicsManager& physics = PhysicsManager::GetInstance();

    vec3 point = m_bodyA->orientation() * m_connectPt;
    vec3 dir = -m_bodyA->orientation()[1];

    const vec3& xaxis = m_bodyA->orientation()[2];
    const vec3& yaxis = m_bodyA->orientation()[0];

    float stcos = cosf(m_steering);
    float stsin = sinf(m_steering);

    vec3 fwdvec = xaxis * stcos + yaxis * stsin;
    vec3 sidevec = -xaxis * stsin + yaxis * stcos;

    m_orientation[0] = sidevec;
    m_orientation[1] = m_bodyA->orientation()[1];
    m_orientation[2] = fwdvec;

    Collision::TraceRayInfo traceInfo;

    bool result = physics.traceRay(point + m_bodyA->location(), dir, m_layers, traceInfo, m_wheelRadius + m_length);
    
    float err = m_wheelRadius + m_length - traceInfo.dist;

    if (result && m_motor == 0.0f)
    {
        if (m_bodyA->isAtRest() && err < 0.025f) result = false;
    }
    
    if (result)
    {
        m_dist = std::max(0.0f, traceInfo.dist - m_wheelRadius);
        vec3 contactPoint = point + dir * m_dist;

        m_jacobian[0] = { dir, contactPoint ^ dir, {}, {} };

        vec3 velocity = m_bodyA->velocity() + (m_bodyA->angularVelocity() ^ contactPoint);
        float speed = velocity * traceInfo.norm;
        float m = effectiveMass(m_jacobian[0], m_bodyA->inverseInertiaTensor());
        float impulse = m * speed;

        if (err < m_length + math::eps) // Act as a spring
        {
            m_error[0] = err;

            float compress = std::min(1.0f, err / m_length);

            m_lowerBound[0] = -compress * compress * m_stiffness + speed * m_damping;
            m_upperBound[0] = 0.0f;
        }
        else // Act as contact
        {
            m_error[0] = m_wheelRadius - traceInfo.dist;

            m_lowerBound[0] = -std::numeric_limits<float>::infinity();
            m_upperBound[0] = std::numeric_limits<float>::infinity();
        }

        sidevec = sidevec - traceInfo.norm * (traceInfo.norm * sidevec);
        fwdvec = fwdvec - traceInfo.norm * (traceInfo.norm * fwdvec);

        sidevec.normalize();
        fwdvec.normalize();

        m_wheelSpeed = (m_bodyA->velocity() * fwdvec) / m_wheelRadius;

        float friction = m_wheelFriction * m_bodyA->mass() * 0.25f * 9.8f; //m_wheelFriction * fabs(impulse)

        m_friction[1] = friction;
        m_jacobian[1] = { sidevec, contactPoint ^ sidevec, {}, {} };

        if (fabs(m_motor) > math::eps)
        {
            m_error[2] = m_motor > 0.0f ? -1.0f : 1.0f;
            m_friction[2] = 0.0f;
        
            m_lowerBound[2] = -fabs(m_motor * 0.1f);
            m_upperBound[2] = fabs(m_motor * 0.1f);
        }
        else if (m_handbrake)
        {
            m_error[2] = 0.0f;
            m_friction[2] = friction;

            m_lowerBound[2] = -std::numeric_limits<float>::infinity();
            m_upperBound[2] = std::numeric_limits<float>::infinity();
        }
        else
        {
            m_error[2] = 0.0f;
            m_friction[2] = m_rollResistance;
        
            m_lowerBound[2] = -std::numeric_limits<float>::infinity();
            m_upperBound[2] = std::numeric_limits<float>::infinity();
        }

        m_jacobian[2] = { fwdvec, contactPoint ^ fwdvec, {}, {} };
    }
    else
    {
        m_wheelSpeed = 0.0f;
        m_dist = m_length;

        m_error[0] = 0.0f;
        m_error[2] = 0.0f;

        m_friction[1] = 0.0f;
        m_friction[2] = 0.0f;
    }
}

} //namespace Physics