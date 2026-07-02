#include "Buoyancy.h"
#include "Render/World.h"
#include "Physics/RigidBody.h"

namespace GameLogic
{

BuoyancyForce::BuoyancyForce(const Render::World& world, const vec3& bbox, float density)
: m_world(world)
, m_density(density)
{
    m_size = std::min(bbox.x, std::min(bbox.y, bbox.z));
    m_volume = m_size > 0.02f ? bbox.x * bbox.y * bbox.z / 8.0f : 0.01f;

    static const vec3 points[] = { {0.5, 0.5, 0.5},
                                   {-0.5, 0.5, 0.5},
                                   {0.5, 0.5, -0.5},
                                   {-0.5, 0.5, -0.5},
                                   {0.5, -0.5, 0.5},
                                   {-0.5, -0.5, 0.5},
                                   {0.5, -0.5, -0.5},
                                   {-0.5, -0.5, -0.5} };

    m_applyPoints.reserve(8);

    for (size_t i = 0; i < 8; i++) 
        m_applyPoints.emplace_back(points[i].x * bbox.x, points[i].y * bbox.y, points[i].z * bbox.z);

}

void BuoyancyForce::update(Physics::RigidBody& body, float dt)
{
    mat4 bodyMat = body.transformMat();

    bool isSubmerged = false;

    if(body.isAtRest()) return;

    for(const vec3& applyPoint : m_applyPoints)
    {
        vec3 point = bodyMat * applyPoint;
        vec3 probe = point - vec3(0, m_size * 0.5f, 0);

        float submergeDepth = m_world.getSubmergeDepth(probe);

        if(submergeDepth <= 0) continue;

        isSubmerged = true;

        float buoyancy = m_volume * m_density * 9.8f;
        float force;

        if(submergeDepth > m_size) force = m_size * buoyancy;
        else force = (m_size - submergeDepth) * buoyancy;

        body.applyForce(vec3(0, force, 0), point - body.location());
    }

    if(isSubmerged) body.applyDrag(0.02, 0.01, dt);
}

void BuoyancyForce::addPoint(const vec3& point)
{
    m_applyPoints.push_back(point);
}

} // namespace Gamelogic
