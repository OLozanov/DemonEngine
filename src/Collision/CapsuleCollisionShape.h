#pragma once

#include "Collision/CollisionShape.h"
#include "Collision/Collision.h"

namespace Collision
{

// Capsule with zero length is equivalent to sphere
class CapsuleCollisionShape : public CollisionShape
{
    float m_radius;
    float m_length;

    //float lineClosestPoint(const vec3& origin, const vec3& ray)

    bool testCollision(const BoxCollisionShape * shape, ContactInfo& contactInfo) const override;
    bool testCollision(const PolygonalCollisionShape * shape, ContactInfo& contactInfo) const override;
    bool testCollision(const CapsuleCollisionShape * shape, ContactInfo& contactInfo) const override;

public:
    CapsuleCollisionShape(const mat3& orientation, const vec3& pos)
    : CollisionShape(orientation, pos)
    , m_radius(0.5f)
    , m_length(1.0f)
    {
    }

    CapsuleCollisionShape(const mat3& orientation, const vec3& pos, float radius, float length = 0.0f)
    : CollisionShape(orientation, pos)
    , m_radius(radius)
    , m_length(length)
    {
    }

    ~CapsuleCollisionShape()
    {
    }

    float radius() const { return m_radius; }
    float length() const { return m_length; }

    bool traceRay(const vec3 & origin, const vec3 & ray, TraceRayInfo& tifo) const override;
    bool traceBBox(TraceInfo& tinfo, const vec3& bbox, const vec3 begin, const vec3 end) const override;
    bool testHeight(const vec3& pos, const vec3& bbox, float& height, float& tilt) const override;

    bool testCollision(const CollisionShape * shape, ContactInfo& contactInfo) const override
    {
        return shape->testCollision(this, contactInfo);
    }
};

} //namespace Collision
