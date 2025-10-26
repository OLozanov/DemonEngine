#include "Physics/RigidBody.h"
#include "stdio.h"

#include <iostream>

namespace Physics
{

RigidBody::RigidBody(const vec3& pos,
            float mass,
            float bounce,
            float friction,
            unsigned long layers,
            bool rotation,
            bool updateHook)
: m_rest(false)
, m_rotation(rotation)
, m_updateHook(updateHook)
, m_mass(mass)
, m_invMass(1.0/mass)
, m_bounce(bounce)
, m_friction(friction)
, m_layers(layers)
, m_pos(pos)
, m_acceleration(0, -9.8, 0)
, m_angularVelocity({})
, m_bbox({1, 1, 1})
, m_adjacentBody(nullptr)
, m_collisionShape(nullptr)
, m_object(nullptr)
{
    if(m_bounce >= 0.9) m_bounce = 0.9;
    if(m_bounce < 0) m_bounce = 0;

    setInertia(BoxInertiaTensor(m_bbox, mass));
}

RigidBody::~RigidBody()
{
    delete m_collisionShape;
}

mat4 RigidBody::transformMat()
{
    return {{m_orientation[0], 0},
            {m_orientation[1], 0},
            {m_orientation[2], 0},
            m_pos};
}

mat3 RigidBody::inertiaTensor() const
{
    return TransformDiagonal(m_orientation, m_inertia);
    //return m_orientation * m_inertiaTensor * m_orientation.transpose();
}

mat3 RigidBody::inverseInertiaTensor() const
{
    return TransformDiagonal(m_orientation, m_invInertia);

    mat3 scaledmat = { m_orientation[0] * m_invInertia.x,
                       m_orientation[1] * m_invInertia.y, 
                       m_orientation[2] * m_invInertia.z };

    return scaledmat * m_orientation.transpose();
}

void RigidBody::setInertia(const vec3& inertia)
{
    m_inertia = inertia;
    m_invInertia = { 1.0f / inertia.x, 1.0f / inertia.y, 1.0f / inertia.z };
}

void RigidBody::applyImpulse(const vec3& impulse, const vec3& point)
{
    m_velocity += impulse*m_invMass;

    if(m_rotation)
    {
        m_angularMomentum += point^impulse;
        m_angularVelocity = inverseInertiaTensor() * m_angularMomentum; //angular velocity
    }

    m_rest = false;
}

void RigidBody::applyForce(const vec3& force)
{
    m_rest = false;
    m_force += force;
}

void RigidBody::applyForce(const vec3& force, const vec3& point)
{
    m_rest = false;
    m_force += force;
    m_torque += point^force;
}

void RigidBody::applyDrag(float drag, float angularDrag, float dt)
{
    m_velocity -= m_velocity*drag*(dt/ReferenceFrameTime);
    m_angularMomentum -= m_angularMomentum*angularDrag*(dt/ReferenceFrameTime);
}

void RigidBody::integrate(float dt)
{
    if (m_rest) return;

    m_velocity += (m_acceleration + m_force * m_invMass) * dt;
    m_pos += m_velocity * dt;

    if(m_rotation)
    {
        if(m_torque*m_torque > 0)
        {
            m_angularMomentum += m_torque*dt;
        }

        //mat3 inverseInertiaTensor_world = m_orientation * m_inverseInertiaTensor * m_orientation.transpose();
        m_angularVelocity = inverseInertiaTensor() * m_angularMomentum;

        vec3 axis = m_angularVelocity;
        float dangle = axis.normalize();

        if (dangle > AngularSpeedEps) m_orientation = mat3::Rotate(axis, dangle * dt)*m_orientation;

        //m_orientation += mat3::SkewSymmetric(m_angularVelocity * dt) * m_orientation;
        //m_orientation.orthogonalize();

        //Drag
        m_angularMomentum -= m_angularMomentum * 0.005 * (dt / ReferenceFrameTime);
        //m_angularMomentum *= pow(0.99, dt);

        //if (m_angularMomentum * m_angularMomentum > 10000.0f) m_angularMomentum = m_angularMomentum.normalized() * 100.0f;

        //if (m_velocity * m_velocity < 0.0005 && dangle < math::eps) m_rest = true;
    }

    m_force = {};
    m_torque = {};

    if (m_updateHook) onUpdate(dt);
}

void RigidBody::postUpdate(float dt)
{
    if (!m_rotation) return;
    if (m_rest) return;

    float speed = m_velocity.length();
    float angSpeed = m_angularVelocity.length();

    if (speed < LinearSpeedEps && angSpeed < AngularSpeedEps) m_rest = true;

    if (speed > 200) m_velocity *= pow(0.2f, dt);
    if (angSpeed > 200) m_angularMomentum *= pow(0.2f, dt);
}

} //namespace Physics