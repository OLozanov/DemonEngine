#include "Collision/CapsuleCollisionShape.h"
#include "Collision/PolygonalCollisionShape.h"
#include "Collision/BoxCollisionShape.h"

namespace Collision
{

bool CapsuleCollisionShape::testCollision(const BoxCollisionShape * shape, ContactInfo& contactInfo) const
{
    bool result = shape->testCollision(this, contactInfo);

    contactInfo.norm = -contactInfo.norm;

    return result;
}

bool CapsuleCollisionShape::testCollision(const PolygonalCollisionShape * shape, ContactInfo& contactInfo) const
{
    if (m_length > 0.0f)
        return shape->testCapsule(contactInfo, this);
    else
        return shape->testSphere(contactInfo, this);
}

bool CapsuleCollisionShape::testCollision(const CapsuleCollisionShape * shape, ContactInfo& contactInfo) const
{
    float radius = shape->m_radius + m_radius;

    if (shape->m_length == 0.0f && m_length == 0.0f)
    {
        vec3 norm = shape->m_pos - m_pos;

        float dist = norm.normalize();
        if (dist > radius) return false;

        contactInfo.norm = norm;
        contactInfo.dist = radius - dist;
        contactInfo.point = (shape->m_pos - norm * shape->m_radius + m_pos + norm * m_radius) * 0.5;
    
        return true;
    }

    const vec3& dir1 = shape->m_orientation[1];
    const vec3& dir2 = shape->m_orientation[1];

    float s;
    float t;

    if (rayClosestPoints(shape->m_pos, m_pos, dir1, dir2, s, t))
    {
        if (s > -shape->m_length * 0.5f && s < shape->m_length * 0.5f &&
            t > -m_length * 0.5f && t < m_length * 0.5f)
        {
            vec3 pt1 = shape->m_pos + dir1 * s;
            vec3 pt2 = m_pos + dir2 * t;

            vec3 norm = pt2 - pt1;
            float dist = norm.normalize();

            if (dist > radius) return false;

            contactInfo.norm = norm;
            contactInfo.dist = radius - dist;
            contactInfo.point = (pt1 + pt2) * 0.5f;

            return true;
        }
        else // closest point outside range
        {
            bool result = false;

            float mindist;

            vec3 a1 = shape->m_pos - dir1 * shape->m_length * 0.5f;
            vec3 b1 = shape->m_pos + dir1 * shape->m_length * 0.5f;

            vec3 a2 = m_pos - dir2 * m_length * 0.5f;
            vec3 b2 = m_pos + dir2 * m_length * 0.5f;

            auto testSegmentDist = [&](const vec3& p,
                const vec3& a, const vec3& b,
                const vec3& pos,
                const vec3& dir,
                float min, float max)
            {
                float dist;
                vec3 norm;
                vec3 pt;

                float proj = (p - pos) * dir;

                if (proj < min)
                {
                    pt = (p + a) * 0.5f;
                    norm = p - a;
                    dist = norm.normalize();
                }
                else if (proj > max)
                {
                    pt = (p + b) * 0.5f;
                    norm = p - b;
                    dist = norm.normalize();
                }
                else
                {
                    vec3 projpt = pos + dir * proj;

                    pt = (p + projpt) * 0.5f;
                    norm = p - projpt;
                    dist = norm.normalize();
                }

                if (!result || dist < mindist)
                {
                    mindist = dist;
                    result = true;

                    contactInfo.norm = norm;
                    contactInfo.dist = radius - dist;
                    contactInfo.point = pt;
                }
            };

            testSegmentDist(a2, a1, b1, shape->m_pos, dir1, -shape->m_length * 0.5f, shape->m_length * 0.5f);
            testSegmentDist(b2, a1, b1, shape->m_pos, dir1, -shape->m_length * 0.5f, shape->m_length * 0.5f);

            testSegmentDist(a1, a2, b2, m_pos, dir2, -m_length * 0.5f, m_length * 0.5f);
            testSegmentDist(b1, a2, b2, m_pos, dir2, -m_length * 0.5f, m_length * 0.5f);

            if (result && mindist < radius)
                return true;
            else
                return false;
        }
    }
    else // axis are colinear: check ovelapping intervals
    {
        vec3 apt1 = shape->m_pos - dir1 * shape->m_length * 0.5f;
        vec3 bpt1 = shape->m_pos + dir1 * shape->m_length * 0.5f;

        vec3 apt2 = m_pos - dir2 * m_length * 0.5f;
        vec3 bpt2 = m_pos + dir2 * m_length * 0.5f;

        vec3 norm = shape->m_pos - m_pos;
        norm -= dir1 * (norm * dir1);

        float dist = norm.normalize();
    
        if (dist > radius) return false;

        float a1 = -shape->m_length * 0.5f;
        float b1 = shape->m_length * 0.5f;

        float a2 = (apt2 - shape->m_pos) * dir1;
        float b2 = (bpt2 - shape->m_pos) * dir1;

        if (a2 > b1)
        {
            vec3 norm = apt2 - bpt1;
            float dist = norm.normalize();
        
            if (dist < radius)
            {
                contactInfo.norm = norm;
                contactInfo.dist = radius - dist;
                contactInfo.point = (apt2 + bpt1) * 0.5f;

                return true;
            }
            else
                return false;
        }

        if (a1 > b2)
        {
            vec3 norm = apt1 - bpt2;
            float dist = norm.normalize();

            if (dist < radius)
            {
                contactInfo.norm = norm;
                contactInfo.dist = radius - dist;
                contactInfo.point = (apt1 + bpt2) * 0.5f;

                return true;
            }
            else
                return false;
        }

        float a = std::max(a1, a2);
        float b = std::min(b1, b2);

        float mid = (a + b) * 0.5f;

        contactInfo.norm = norm;
        contactInfo.dist = dist;
        contactInfo.point = shape->m_pos + dir1 * mid + norm * dist * 0.5f;

        return true;
    }

    return false;
}

bool CapsuleCollisionShape::traceRay(const vec3 & origin, const vec3 & ray, TraceRayInfo& tinfo) const
{
    vec3 dir = m_pos - origin;
    float len2 = dir*dir;
    float rad2 = m_radius*m_radius;

    if(len2 < rad2) return false;

    float rayproj = dir*ray;
    float rdist2 = len2 - rayproj*rayproj;

    float idist = sqrt(rad2-rdist2);

    tinfo.dist = rayproj - idist;
    tinfo.norm = ((origin + ray * tinfo.dist) - m_pos).normalized();

    return true;
}

bool CapsuleCollisionShape::traceBBox(TraceInfo& tinfo, const vec3& bbox, const vec3 begin, const vec3 end) const
{
    return false;
}

bool CapsuleCollisionShape::testHeight(const vec3& pos, const vec3& bbox, float& height, float& tilt) const
{
    return false;
}

} //namespace Collision