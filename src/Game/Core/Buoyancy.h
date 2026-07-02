#pragma once

#include "Physics/Force.h"
#include "math/math3d.h"

#include <vector>

namespace Render
{
    class World;
}

namespace GameLogic
{

class BuoyancyForce : public Physics::ForceGenerator
{
    std::vector<vec3> m_applyPoints;
    const Render::World& m_world;

    float m_size;
    float m_volume;
    float m_density; // fluid density

public:

    BuoyancyForce(const Render::World& world, const vec3& bbox, float density = 12000);
    ~BuoyancyForce() {}

    void addPoint(const vec3& point);

    void update(Physics::RigidBody& body, float dt) override;
};

} // namespace Gamelogic
