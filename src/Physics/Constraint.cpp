#include "Constraint.h"
#include "Physics/RigidBody.h"

namespace Physics
{

Constraint::Constraint(size_t rows, RigidBody* bodyA, RigidBody* bodyB, bool collision)
: m_bodyA(bodyA) 
, m_bodyB(bodyB)
, m_jacobian(rows)
, m_error(rows)
, m_friction(rows)
, m_lowerBound(rows)
, m_upperBound(rows)
{
    if (!collision && m_bodyB) m_bodyB->m_adjacentBody = m_bodyA;      // ignore collision between bodies

    for (size_t i = 0; i < rows; i++)
    {
        m_error[i] = 0.0f;
        m_friction[i] = 0.0f;
    }
}

float Constraint::effectiveMass(const JacobianRow& jacobian, const mat3& inverseInertiaTensor)
{
    return 1.0f / (m_bodyA->m_invMass * (jacobian.linearA * jacobian.linearA) +
                  (inverseInertiaTensor * jacobian.angularA) * jacobian.angularA);
}

float Constraint::effectiveMass(const JacobianRow& jacobian, const mat3& inverseInertiaTensorA, const mat3& inverseInertiaTensorB)
{
    return 1.0f / ( m_bodyA->m_invMass * (jacobian.linearA * jacobian.linearA) + 
                  (inverseInertiaTensorA * jacobian.angularA) * jacobian.angularA +
                  m_bodyB->m_invMass * (jacobian.linearB * jacobian.linearB) +
                  (inverseInertiaTensorB * jacobian.angularB) * jacobian.angularB);
}

void Constraint::resolveSingle(float dt)
{
    mat3 inverseInertiaTensorA = m_bodyA->inverseInertiaTensor();

    for (size_t i = 0; i < m_jacobian.size(); i++)
    {
        //if (fabs(m_error[i]) < math::eps) continue;

        if (fabs(m_error[i]) < math::eps && m_friction[i] < math::eps) continue;

        float m = effectiveMass(m_jacobian[i], inverseInertiaTensorA);
        float Jv = m_jacobian[i].linearA * m_bodyA->m_velocity + m_jacobian[i].angularA * m_bodyA->m_angularVelocity;

        float lambda = -m * (Jv + 0.5f / dt * m_error[i]);
       
        if (m_friction[i] > math::eps)
            lambda = std::max(-m_friction[i], std::min(lambda, m_friction[i]));
        else
            lambda = std::max(m_lowerBound[i], std::min(lambda, m_upperBound[i]));

        if (fabs(lambda) < math::eps) continue;

        m_bodyA->m_rest = false;

        m_bodyA->m_velocity += m_jacobian[i].linearA * m_bodyA->m_invMass * lambda;
        m_bodyA->m_angularMomentum += m_jacobian[i].angularA * lambda;
        m_bodyA->m_angularVelocity = inverseInertiaTensorA * m_bodyA->m_angularMomentum;
    }
}

void Constraint::resolve(float dt)
{
    if (!m_bodyB)
    {
        resolveSingle(dt);
        return;
    }

    mat3 inverseInertiaTensorA = m_bodyA->inverseInertiaTensor();
    mat3 inverseInertiaTensorB = m_bodyB->inverseInertiaTensor();

    for (size_t i = 0; i < m_jacobian.size(); i++)
    {
        if (fabs(m_error[i]) < math::eps) continue;

        float m = effectiveMass(m_jacobian[i], inverseInertiaTensorA, inverseInertiaTensorB);

        float Jv = m_jacobian[i].linearA * m_bodyA->m_velocity + m_jacobian[i].angularA * m_bodyA->m_angularVelocity +
                   m_jacobian[i].linearB * m_bodyB->m_velocity + m_jacobian[i].angularB * m_bodyB->m_angularVelocity;

        float lambda = -m * (Jv + 0.5f / dt * m_error[i]);

        if (lambda + math::eps < m_lowerBound[i]) continue;
        if (lambda - math::eps > m_upperBound[i]) continue;

        if (fabs(lambda) < math::eps) continue;

        m_bodyA->m_rest = false;
        m_bodyB->m_rest = false;

        m_bodyA->m_velocity += m_jacobian[i].linearA * m_bodyA->m_invMass * lambda;
        m_bodyA->m_angularMomentum += m_jacobian[i].angularA * lambda;
        m_bodyA->m_angularVelocity = inverseInertiaTensorA * m_bodyA->m_angularMomentum;

        m_bodyB->m_velocity += m_jacobian[i].linearB * m_bodyB->m_invMass * lambda;
        m_bodyB->m_angularMomentum += m_jacobian[i].angularB * lambda;
        m_bodyB->m_angularVelocity = inverseInertiaTensorB * m_bodyB->m_angularMomentum;
    }
}

Joint::Joint(RigidBody* bodyA, const vec3& pointA, RigidBody* bodyB, const vec3& pointB)
: Constraint(1, bodyA, bodyB)
, m_pointA(pointA)
, m_pointB(pointB)
, m_anglimits(0)
{
    m_lowerBound[0] = -std::numeric_limits<float>::infinity();
    m_upperBound[0] = std::numeric_limits<float>::infinity();
}

Joint::Joint(RigidBody* bodyA, const vec3& pointA, RigidBody* bodyB, const vec3& pointB, const float angmin[3], const float angmax[3])
: Constraint(1 + initAngularLimits(angmin, angmax), bodyA, bodyB)
, m_pointA(pointA)
, m_pointB(pointB)
, m_customFrame(false)
{
    for (size_t i = 0; i < m_jacobian.size(); i++)
    {
        m_lowerBound[i] = -std::numeric_limits<float>::infinity();
        m_upperBound[i] = std::numeric_limits<float>::infinity();
    }
}

void Joint::setReferenceFrame(const mat3& frame)
{
    m_frame = frame;
    m_customFrame = true;
}

size_t Joint::initAngularLimits(const float angmin[3], const float angmax[3])
{
    size_t limits = 0;

    for (size_t i = 0; i < 3; i++)
    {
        if (angmin[i] > angmax[i]) continue;
        
        m_anglimits |= 1 << i;
        m_angmin[i] = angmin[i];
        m_angmax[i] = angmax[i];

        limits++;
    }

    return limits;
}

void Joint::evaluateLinear()
{
    vec3 pointAworld = m_bodyA->orientation() * m_pointA + m_bodyA->location();
    vec3 pointBworld = m_bodyB->orientation() * m_pointB + m_bodyB->location();

    vec3 axis = pointBworld - pointAworld;

    m_error[0] = axis.normalize();

    vec3 ra = pointAworld - m_bodyA->location();
    vec3 rb = pointBworld - m_bodyB->location();

    m_jacobian[0] = { -axis, -(ra ^ axis),
                       axis, rb ^ axis };
}

void Joint::evaluateAngular()
{
    mat3 matA = m_customFrame ? m_bodyA->orientation() * m_frame :
                                m_bodyA->orientation();

    const mat3 matB = m_bodyB->orientation();

    vec3 euler = ToEuler(matA.transpose() * matB);

    const vec3& x = matB[0]; //m_bodyB->orientation()[0];
    const vec3& z = matA[2]; //m_bodyA->orientation()[2];

    vec3 axisY = z ^ x;
    vec3 axisX = axisY ^ z;
    vec3 axisZ = x ^ axisY;

    size_t i = 1;

    if (m_anglimits & 1) 
    { 
        m_error[i] = euler.x < m_angmin[0] ? euler.x - m_angmin[0] :
                     euler.x > m_angmax[0] ? euler.x - m_angmax[0] :
                     0.0f;

        if (m_error[i] > math::pi) m_error[i] -= math::pi2;
        else if (m_error[i] < -math::pi) m_error[i] += math::pi2;
        
        m_jacobian[i] = { {}, -axisX,
                          {}, axisX };
        
        i++;
    }
    
    if (m_anglimits & 2) 
    { 
        m_error[i] = euler.y < m_angmin[1] ? euler.y - m_angmin[1] :
                     euler.y > m_angmax[1] ? euler.y - m_angmax[1] :
                     0.0f;

        if (m_error[i] > math::pi) m_error[i] -= math::pi2;
        else if (m_error[i] < -math::pi) m_error[i] += math::pi2;

        m_jacobian[i] = { {}, -axisY,
                          {}, axisY };

        i++;
    }
    
    if (m_anglimits & 4) 
    { 
        m_error[i] = euler.z < m_angmin[2] ? euler.z - m_angmin[2] :
                     euler.z > m_angmax[2] ? euler.z - m_angmax[2] :
                     0.0f;

        if (m_error[i] > math::pi) m_error[i] -= math::pi2;
        else if (m_error[i] < -math::pi) m_error[i] += math::pi2;

        m_jacobian[i] = { {}, -axisZ,
                          {}, axisZ };
    }
}

void Joint::evaluate()
{
    evaluateLinear();
    if (m_anglimits) evaluateAngular();
}

ConeConstraint::ConeConstraint(RigidBody* bodyA, RigidBody* bodyB, float angle)
: Constraint(1, bodyA, bodyB)
, m_angle(angle)
{
    m_lowerBound[0] = 0;
    m_upperBound[0] = std::numeric_limits<float>::infinity();
}

void ConeConstraint::evaluate()
{
    m_error[0] = m_bodyA->orientation()[1] * m_bodyB->orientation()[1] - cos(m_angle / 180.0f * math::pi);

    vec3 angular = m_bodyB->orientation()[1] ^ m_bodyA->orientation()[1];

    m_jacobian[0] = { {}, -angular,
                      {}, angular };
}

Hinge::Hinge(RigidBody* bodyA, const vec3& pointA, RigidBody* bodyB, const vec3& pointB, size_t axis)
: Constraint(3, bodyA, bodyB)
, m_pointA(pointA)
, m_pointB(pointB)
, m_i(axis)
, m_anglimit(false)
{
    switch (m_i)
    {
    case 0: m_j = 1; m_k = 2; break;
    case 1: m_j = 0; m_k = 2; break;
    case 2: m_j = 0; m_k = 1; break;
    }

    for (size_t i = 0; i < 3; i++)
    {
        m_lowerBound[i] = -std::numeric_limits<float>::infinity();
        m_upperBound[i] = std::numeric_limits<float>::infinity();
    }
}

Hinge::Hinge(RigidBody* bodyA, const vec3& pointA, RigidBody* bodyB, const vec3& pointB, size_t axis, float angmin, float angmax)
: Constraint(4, bodyA, bodyB)
, m_pointA(pointA)
, m_pointB(pointB)
, m_i(axis)
, m_anglimit(true)
, m_angmin(angmin)
, m_angmax(angmax)
{
    switch (m_i)
    {
    case 0: m_j = 1; m_k = 2; break;
    case 1: m_j = 0; m_k = 2; break;
    case 2: m_j = 0; m_k = 1; break;
    }

    for (size_t i = 0; i < 4; i++)
    {
        m_lowerBound[i] = -std::numeric_limits<float>::infinity();
        m_upperBound[i] = std::numeric_limits<float>::infinity();
    }
}

void Hinge::evaluate()
{
    // Position constraint
    {
        vec3 pointAworld = m_bodyA->orientation() * m_pointA + m_bodyA->location();
        vec3 pointBworld = m_bodyB->orientation() * m_pointB + m_bodyB->location();

        vec3 axis = pointBworld - pointAworld;

        m_error[0] = axis.normalize();

        vec3 ra = pointAworld - m_bodyA->location();
        vec3 rb = pointBworld - m_bodyB->location();

        m_jacobian[0] = { -axis, -(ra ^ axis),
                           axis, rb ^ axis };
    }

    // Revolution plain constraint 1
    {
        const vec3& a = m_bodyB->orientation()[m_i];
        const vec3& b = m_bodyA->orientation()[m_k];

        m_error[1] = a * b;
        vec3 angular = a ^ b;

        m_jacobian[1] = { {}, -angular,
                          {}, angular };
    }

    // Revolution plain constraint 1
    {
        const vec3& a = m_bodyB->orientation()[m_i];
        const vec3& b = m_bodyA->orientation()[m_j];

        m_error[2] = a * b;
        vec3 angular = a ^ b;

        m_jacobian[2] = { {}, -angular,
                          {}, angular };
    }

    // Angle constraint
    if (m_anglimit)
    {
        const vec3& ax = m_bodyA->orientation()[m_k];
        const vec3& ay = m_bodyA->orientation()[m_j];
        const vec3& bx = m_bodyB->orientation()[m_k];

        float ang = atan2(-ay * bx, ax * bx);

        const vec3& axis = m_bodyA->orientation()[m_i];

        m_error[3] = ang < m_angmin ? ang - m_angmin :
                     ang > m_angmax ? ang - m_angmax :
                     0.0f;

        if (m_error[3] > math::pi) m_error[3] -= math::pi2;
        else if (m_error[3] < -math::pi) m_error[3] += math::pi2;

        m_jacobian[3] = { {}, -axis,
                          {}, axis };
    }
}

} //namespace Physics