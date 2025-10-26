#include "Collision.h"

namespace Collision
{

bool CollisionPolygon::isPointInside(const vec3& pt) const
{
    for (int i = 0; i < verts.size(); i++)
    {
        int k = (i == (verts.size() - 1)) ? 0 : i + 1;

        vec3 edge = verts[k] - verts[i];
        edge.normalize();

        vec3 enorm = edge ^ plane.xyz;
        float edst = -enorm * verts[i];

        float dist = enorm * pt + edst;

        if (dist > math::eps) return false;
    }

    return true;
}

bool CollisionPolygon::isInsidePerimeter(const vec3& pt) const
{
    for (int i = 0; i < verts.size(); i++)
    {
        int k = (i == (verts.size() - 1)) ? 0 : i + 1;

        vec3 edge = verts[k] - verts[i];
        vec3 dir = pt - verts[i];

        float dist = edge.x * dir.z - edge.z * dir.x;

        if (dist > math::eps) return false;
    }

    return true;
}

void CollisionPolygon::supportFunction(const vec3& axis, float& min, float& max) const
{
    min = verts[0] * axis;
    max = verts[0] * axis;

    for (int v = 1; v < verts.size(); v++)
    {
        float p = verts[v] * axis;

        if (p < min) min = p;
        if (p > max) max = p;
    }
}

bool CollisionPolygon::findBoxSepAxis(const vec3& pos, const vec3& bbox) const
{
    vec3 axis[3] = { {1, 0, 0},
                     {0, 1, 0},
                     {0, 0, 1} };

    float min;
    float max;

    for (int i = 0; i < verts.size(); i++)
    {
        int k;
        if (i == (verts.size() - 1)) k = 0;
        else k = i + 1;

        vec3 edge = verts[k] - verts[i];
        edge.normalize();

        vec3 snorm = edge ^ plane.xyz;
        snorm.normalize();

        supportFunction(snorm, min, max);

        max = snorm * verts[i];

        float reps = 0.002;

        if (fabs(1 - fabs(plane.x)) < 0.0001) reps = 0;
        if (fabs(1 - fabs(plane.y)) < 0.0001) reps = 0;
        if (fabs(1 - fabs(plane.z)) < 0.0001) reps = 0;

        float dist = bbox.x * fabs(snorm.x) + bbox.y * fabs(snorm.y) + bbox.z * fabs(snorm.z) + reps;
        float pdst = pos * snorm;

        //if(((pdst + dist) <= min) && ((pdst + dist) < max)) return true;
        //if(((pdst - dist) > min) && ((pdst - dist) >= max)) return true;

        if ((pdst + dist) <= min) return true;
        if ((pdst - dist) >= max) return true;

        for (int i = 0; i < 3; i++)
        {
            vec3 snorm = edge ^ axis[i];
            if (fabs(snorm.length()) < 0.0001) continue;

            snorm.normalize();

            float f = fabs(snorm * plane.xyz);
            if (fabs(1 - f) < 0.0001) continue;

            supportFunction(snorm, min, max);

            float dist = bbox.x * fabs(snorm.x) + bbox.y * fabs(snorm.y) + bbox.z * fabs(snorm.z) + reps;
            float pdst = pos * snorm;

            if ((pdst + dist) <= min) return true;
            if ((pdst - dist) >= max) return true;
        }
    }

    return false;
}

bool CollisionPolygon::findBoxSepAxis(const vec3& pos, const vec3& bbox, const mat3& axis) const
{
    float deps = 0.002;

    //Test box faces
    for (int i = 0; i < 3; i++)
    {
        const vec3& dir = axis[i];

        if (fabs(1 - fabs(dir * plane.xyz)) < math::eps) continue;

        float max;
        float min;

        supportFunction(dir, min, max);

        float dist = pos * dir;

        if (dist + bbox[i] + deps < min) return true;
        if (dist - bbox[i] - deps > max) return true;
    }

    //Test polygon edges
    for (int i = 0; i < verts.size(); i++)
    {
        int k = (i == (verts.size() - 1)) ? k = 0 : k = i + 1;

        vec3 edge = verts[k] - verts[i];
        edge.normalize();

        for (int a = 0; a < 3; a++)
        {
            vec3 dir = edge ^ axis[a];
            float n = dir.normalize();

            if (n < 0.001) continue;
            if (fabs(1 - fabs(dir * plane.xyz)) < math::eps) continue;

            float max;
            float min;

            supportFunction(dir, min, max);

            float sz = bbox.x * fabs(dir * axis[0]) +
                       bbox.y * fabs(dir * axis[1]) +
                       bbox.z * fabs(dir * axis[2]);

            float bdist = pos * dir;

            if (bdist + sz + deps < min) return true;
            if (bdist - sz - deps > max) return true;
        }
    }

    return false;
}

bool CollisionPolygon::findEdgeSepAxis(const vec3& pos, const vec3& bbox, const vec3& a, const vec3& b, const mat3& axis, const vec3& touchnorm, int aind) const
{
    constexpr float deps = 0.0001;

    vec3 edge = b - a;
    edge.normalize();

    for (int i = 0; i < 3; i++)
    {
        if (i == aind) continue;

        vec3 dir = edge ^ axis[i];
        float n = dir.normalize();

        if (n < 0.001) continue;
        if (fabs(1 - fabs(dir * touchnorm)) < deps) continue;

        float max;
        float min;

        supportFunction(dir, min, max);

        float sz = bbox.x * fabs(dir * axis[0]) +
            bbox.y * fabs(dir * axis[1]) +
            bbox.z * fabs(dir * axis[2]);

        float bdist = pos * dir;
        float edist = a * dir;

        if (bdist + sz < min + deps) return true;
        if (bdist - sz > max - deps) return true;
    }

    return false;
}

void supportFunction(const vec3& axis, const vec3* verts, unsigned long vnum, float& min, float& max)
{
    min = verts[0] * axis;
    max = verts[0] * axis;

    for (int v = 1; v < vnum; v++)
    {
        float p = verts[v] * axis;

        if (p < min) min = p;
        if (p > max) max = p;
    }
}

float intervalIntersection(float a, float b, float c, float d)
{
    float beg1, beg2;
    float end1, end2;

    if (a < b)
    {
        beg1 = a;
        end1 = b;
    }
    else
    {
        beg1 = b;
        end1 = a;
    }

    if (c < d)
    {
        beg2 = c;
        end2 = d;
    }
    else
    {
        beg2 = d;
        end2 = c;
    }

    float beg = (beg2 > beg1) ? beg2 : beg1;
    float end = (end2 < end1) ? end2 : end1;

    return (beg + end) * 0.5;
}

bool lineIntersection(const vec3& norm, float d, const vec3& a, const vec3& b, vec3& pt)
{
    vec3 dir = b - a;

    float d1 = norm * a + d;
    float d2 = norm * b + d;

    if ((d1 >= 0) && (d2 >= 0)) return false;
    if ((d1 < 0) && (d2 < 0)) return false;

    float frac = d1 / (d1 - d2);

    pt = a + dir * frac;
    return true;
}

bool isInsideBbox(const vec3& pt, const vec3& pos, const vec3& bbox)
{
    return fabs(pt.x - pos.x) < bbox.x + math::eps &&
        fabs(pt.y - pos.y) < bbox.y + math::eps &&
        fabs(pt.z - pos.z) < bbox.z + math::eps;
}

void rayOverlap(vec3 origin1, vec3 origin2, vec3 ray1, vec3 ray2, float& s, float& t)
{
    vec3 r = origin1 - origin2;

    float a = ray1 * ray1;
    float b = ray1 * ray2;
    float c = ray1 * r;
    float e = ray2 * ray2;
    float f = ray2 * r;

    float det = a * e - b * b;

    s = (b * f - c * e) / det;
    t = (a * f - b * c) / det;
}

bool rayClosestPoints(const vec3& origin1, const vec3& origin2,
                      const vec3& ray1, const vec3& ray2,
                      float& s, float& t)
{
    vec3 r = origin1 - origin2;

    float a = ray1 * ray1;
    float b = ray1 * ray2;
    float c = ray1 * r;
    float e = ray2 * ray2;
    float f = ray2 * r;

    float det = a * e - b * b;

    if (fabs(det) < math::eps) return false;

    s = (b * f - c * e) / det;
    t = (a * f - b * c) / det;

    return true;
}

} //namespace Collision