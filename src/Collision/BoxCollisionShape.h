#pragma once

#include "Collision/CollisionShape.h"

namespace Collision
{

class BoxCollisionShape : public CollisionShape
{
    vec3 m_dimensions;
    vec3 m_center;

public:
    BoxCollisionShape(const mat3& orientation, const vec3& pos, const vec3& dimensions)
    : CollisionShape(orientation, pos)
    , m_dimensions(dimensions)
    , m_center(0)
    {
    }

    BoxCollisionShape(const mat3& orientation, const vec3& pos, const vec3& dimensions, const vec3& center)
    : CollisionShape(orientation, pos)
    , m_dimensions(dimensions)
    , m_center(center)
    {
    }

    ~BoxCollisionShape()
    {
    }

    void setDimensions(const vec3& dim) { m_dimensions = dim; }
    const vec3& getDimensions() const { return m_dimensions; }

    void setCenter(const vec3& center) { m_center = center; }
    const vec3& center() const { return m_center; }

    vec3 boxpos() const { return m_pos + m_orientation * m_center; }

    bool testCollision(const BoxCollisionShape * shape, ContactInfo& contactInfo) const override;
    bool testCollision(const PolygonalCollisionShape * shape, ContactInfo& contactInfo) const override;
    bool testCollision(const CapsuleCollisionShape * shape, ContactInfo& contactInfo) const override;

    bool traceRay(const vec3 & origin, const vec3 & ray, TraceRayInfo& tinfo) const override;
    bool traceBBox(TraceInfo& tinfo, const vec3& bbox, const vec3 begin, const vec3 end) const override;
    bool testHeight(const vec3& pos, const vec3& bbox, float& height, float& tilt) const override;

    bool testCollision(const CollisionShape * shape, ContactInfo& contactInfo) const override
    {
        return shape->testCollision(this, contactInfo);
    }
};

bool BBoxVsBBox(ContactInfo& contactInfo,
                const vec3& pos1, const mat3& rot1, const vec3& bbox1,
                const vec3& pos2, const mat3& rot2, const vec3& bbox2);

} //namespace Collision
