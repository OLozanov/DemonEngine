#pragma once

#include "Physics/Force.h"
#include "Physics/RigidBody.h"
#include "math/math3d.h"

namespace Physics
{

class LinearForceGenerator : public ForceGenerator
{
    vec3 m_force;
public:

    LinearForceGenerator() {}
    LinearForceGenerator(const vec3& force) : m_force(force) {}

    ~LinearForceGenerator() {}

    void setForce(const vec3& force) { m_force = force; }

    void update(RigidBody& body, float dt) override
    {
        body.applyForce(m_force);
    }
};

} //namespace Physics
