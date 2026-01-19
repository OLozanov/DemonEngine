#pragma once

#include "math/math3d.h"
#include "Utils/LinkedList.h"
#include "Physics/Inertia.h"
#include "Collision/CollisionShape.h"

namespace Physics
{

class PhysicsManager;
class Constraint;

using ObjectRef = void*;

class RigidBody : public ListNode<RigidBody>
{
protected:
    static constexpr float ReferenceFrameTime = 0.025;

    const float m_restLinearVelocity = 0.035f; //0.018f;
    const float m_restAngularVelocity = 0.035f;

    bool m_rest;

    const bool m_rotation;
    const bool m_updateHook;

    const float m_mass;
    const float m_invMass;

    // Rotational inertia. 
    // Diagonal of inertia tensor in principal space (which object space coincides with)
    vec3 m_inertia;
    vec3 m_invInertia;

    float m_bounce;
    float m_friction;

    unsigned long m_layers;

    vec3 m_pos;
    vec3 m_velocity;
    vec3 m_acceleration;

    vec3 m_angularMomentum;
    vec3 m_angularVelocity;

    mat3 m_orientation;

    vec3 m_force;
    vec3 m_torque;

    vec3 m_bbox;

    RigidBody* m_adjacentBody;
    Collision::CollisionShape * m_collisionShape;

    ObjectRef m_object;

    void setInertia(const vec3& inertia);

    virtual void onCollide(const vec3& normal, float impulse) = 0;
    virtual void onUpdate(float dt) {}

public:
    RigidBody(const vec3& pos,
            float mass,
            float bounce = 0.2,
            float friction = 0.5,
            unsigned long layers = 1,
            bool rotation = true,
            bool updateHook = false,
            float restLinearVelocity = 0.018f,
            float restAngularVelocity = 0.035f);

    virtual ~RigidBody();

    bool isAtRest() { return m_rest; }

    const vec3& boundingBox() { return m_bbox; }

    const vec3& location() { return m_pos; }
    const mat3& orientation() { return m_orientation; }
    mat4 transformMat();

    float mass() const { return m_mass; }
    float inverseMass() const { return m_invMass; }

    mat3 inertiaTensor() const;
    mat3 inverseInertiaTensor() const;

    const vec3& velocity() const { return m_velocity; }
    void setVelocity(const vec3& velocity) { m_velocity = velocity; }

    const vec3& angularVelocity() const { return m_angularVelocity; }

    void applyImpulse(const vec3& impulse);
    void applyImpulse(const vec3& impulse, const vec3& point);
    void applyForce(const vec3& force);
    void applyForce(const vec3& force, const vec3& point);

    void applyDrag(float drag, float angularDrag, float dt);

    void integrate(float dt);
    void postUpdate(float dt);

    friend class PhysicsManager;
    friend class Constraint;
};

} //namespace Physics
