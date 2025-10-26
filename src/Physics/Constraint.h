#pragma once

#include "Utils/LinkedList.h"
#include "math/math3d.h"

#include <vector>

namespace Physics
{

class PhysicsManager;
class RigidBody;

struct JacobianRow
{
    vec3 linearA;
    vec3 angularA;
    vec3 linearB;
    vec3 angularB;
};

class Constraint : public ListNode<Constraint>
{
protected:
    Constraint(size_t rows, RigidBody* bodyA, RigidBody* bodyB, bool collision = false);

    float effectiveMass(const JacobianRow& jacobian, const mat3& inverseInertiaTensor);
    float effectiveMass(const JacobianRow& jacobian, const mat3& inverseInertiaTensorA, const mat3& inverseInertiaTensorB);

    void resolveSingle(float dt);

public:
    virtual ~Constraint() {}

    virtual void evaluate() = 0;
    void resolve(float dt);

    //const Jacobian& jacobian() { return m_jacobian; }

protected:
    RigidBody* m_bodyA;
    RigidBody* m_bodyB;

    std::vector<JacobianRow> m_jacobian;    // C' = Jv
    std::vector<float> m_error;             // Constraint error/bias

    std::vector<float> m_friction;

    std::vector<float> m_lowerBound;
    std::vector<float> m_upperBound;

    friend class PhysicsManager;
};

class Joint : public Constraint
{
public:
    Joint(RigidBody* bodyA, const vec3& pointA, RigidBody* bodyB, const vec3& pointB);
    Joint(RigidBody* bodyA, const vec3& pointA, RigidBody* bodyB, const vec3& pointB, const float angmin[3], const float angmax[3]);

    void setReferenceFrame(const mat3& frame);

    void evaluate() override;

private:
    size_t initAngularLimits(const float angmin[3], const float angmax[3]);

    void evaluateLinear();
    void evaluateAngular();

private:
    vec3 m_pointA;
    vec3 m_pointB;

    bool m_customFrame;
    mat3 m_frame;

    // Angular limits
    uint8_t m_anglimits;

    float m_angmin[3];
    float m_angmax[3];
};

class ConeConstraint : public Constraint
{
public:
    ConeConstraint(RigidBody* bodyA, RigidBody* bodyB, float angle);

    void evaluate() override;
private:
    float m_angle;
};

class Hinge : public Constraint
{
public:
    Hinge(RigidBody* bodyA, const vec3& pointA, RigidBody* bodyB, const vec3& pointB, size_t axis);
    Hinge(RigidBody* bodyA, const vec3& pointA, RigidBody* bodyB, const vec3& pointB, size_t axis, float angmin, float angmax);

    void evaluate() override;

private:
    vec3 m_pointA;
    vec3 m_pointB;

    size_t m_i;
    size_t m_j;
    size_t m_k;

    bool m_anglimit;

    float m_angmin;
    float m_angmax;
};

} //namespace Physics