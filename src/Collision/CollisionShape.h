#pragma once

#include "math/math3d.h"
#include "Collision/Collision.h"

namespace Collision
{

class BoxCollisionShape;
class PolygonalCollisionShape;
class CapsuleCollisionShape;

class CollisionShape
{
protected:
    const mat3& m_orientation;
    const vec3& m_pos;

public:
    CollisionShape(const mat3& orientation, const vec3& pos)
    : m_orientation(orientation)
    , m_pos(pos)
    {
    }

    virtual ~CollisionShape() {}

    const mat3& orientation() const { return m_orientation; }
    const vec3& pos() const { return m_pos; }

    virtual bool traceRay(const vec3 & origin, const vec3 & ray, TraceRayInfo& dist) const = 0;
    virtual bool traceBBox(TraceInfo& tinfo, const vec3& bbox, const vec3 begin, const vec3 end) const = 0;
    virtual bool testHeight(const vec3& pos, const vec3& bbox, float& height, float& tilt) const = 0;
    virtual bool testCollision(const CollisionShape * shape, ContactInfo& contactInfo) const = 0;

    virtual bool testCollision(const BoxCollisionShape * shape, ContactInfo& contactInfo) const = 0;
    virtual bool testCollision(const PolygonalCollisionShape * shape, ContactInfo& contactInfo) const = 0;
    virtual bool testCollision(const CapsuleCollisionShape * shape, ContactInfo& contactInfo) const = 0;
};

} //namespace Collision