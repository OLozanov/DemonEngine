#pragma once

#include "Collision/CollisionShape.h"
#include "Collision/Collision.h"

namespace Collision
{

class PolygonalCollisionShape : public CollisionShape
{
    unsigned long m_pnum;
    const CollisionPolygon * m_polygons;

    static void findContactPoint(ContactInfo& cinfo,
                            const CollisionPolygon& polygon,
                            const vec3& pos, const mat3& axis, const vec3 & bbox);

    static void findIntersections(vec3& accum, int& n,
                                const CollisionPolygon& polygonA,
                                const vec3& pos, const mat3& axis,
                                const CollisionPolygon& polygonB);

    static bool testPolygon(ContactInfo& cinfo,
                            const CollisionPolygon& polygonA,
                            const vec3& pos, const mat3& axis,
                            const CollisionPolygon& polygonB);

    static bool testPolygon(ContactInfo& cinfo,
                            const CollisionPolygon& polygon,
                            const vec3& pos, const mat3& axis, const vec3 & bbox);

    static bool testPolygon(ContactInfo& cinfo,
                            const CollisionPolygon& polygon,
                            const vec3& pos, const mat3& axis,
                            float radius);

    static bool testPolygon(ContactInfo& cinfo,
                            const CollisionPolygon& polygon,
                            const vec3& pt1, const vec3& pt2,
                            const mat3& axis, float radius);

    static bool tracePolygon(const CollisionPolygon& polygon, const vec3 & origin, const vec3 & ray, float & dist);

    static bool tracePolygon(TraceInfo& tinfo,
                            const CollisionPolygon& polygon,
                            const vec3& bbox,
                            const mat3& axis,
                            const vec3& begin, const vec3& end);

    static bool testPolyHeight(const CollisionPolygon& polygon,
                               const vec3& pos,
                               const vec3& bbox,
                               float& dist);

    bool testCollision(const BoxCollisionShape * shape, ContactInfo& contactInfo) const override;
    bool testCollision(const PolygonalCollisionShape * shape, ContactInfo& contactInfo) const override;
    bool testCollision(const CapsuleCollisionShape * shape, ContactInfo& contactInfo) const override;

    bool testPolygonal(ContactInfo & cinfo, const vec3 & pos, const mat3& mat,
                       const CollisionPolygon * polygons, unsigned long pnum) const;

public:
    PolygonalCollisionShape(const mat3& orientation, const vec3& pos)
    : CollisionShape(orientation, pos)
    , m_pnum(0)
    , m_polygons(nullptr)
    {
    }

    PolygonalCollisionShape(const mat3& orientation, const vec3& pos, unsigned long num, const CollisionPolygon * polygons)
    : CollisionShape(orientation, pos)
    , m_pnum(num)
    , m_polygons(polygons)
    {
    }

    ~PolygonalCollisionShape()
    {
    }

    bool testBBox(ContactInfo & cinfo, const vec3 & pos, const mat3& mat, const vec3 & bbox) const;
    bool testSphere(ContactInfo & cinfo, const CapsuleCollisionShape * sphere) const;
    bool testCapsule(ContactInfo& cinfo, const CapsuleCollisionShape* capsule) const;

    bool traceRay(const vec3 & origin, const vec3 & ray, TraceRayInfo& tinfo) const override;
    bool traceBBox(TraceInfo& tinfo, const vec3& bbox, const vec3 begin, const vec3 end) const;
    bool testHeight(const vec3& pos, const vec3& bbox, float& height, float& tilt) const override;

    bool testCollision(const CollisionShape * shape, ContactInfo& contactInfo) const override
    {
        return shape->testCollision(this, contactInfo);
    }
};

} //namespace Collision
