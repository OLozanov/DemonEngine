#include "Collision/BoxCollisionShape.h"
#include "Collision/PolygonalCollisionShape.h"
#include "Collision/CapsuleCollisionShape.h"

namespace Collision
{

void findContactPoint(ContactInfo& cinfo,
                const vec3& pos1, const mat3& axis1, const vec3& bbox1,
                const vec3& pos2, const mat3& axis2, const vec3& bbox2)
{
    vec3 pos = pos2 - pos1;

    vec3 accum = {};
    int n = 0;

    static const int xcoord[] = {-1, 1, 1, -1};
    static const int ycoord[] = {-1, -1, 1, 1};

    for(int i = 0; i < 4; i++)
    {
        vec3 a = axis2*vec3(bbox2.x*xcoord[i], bbox2.y*ycoord[i], bbox2.z) + pos;
        vec3 b = axis2*vec3(bbox2.x*xcoord[i], bbox2.y*ycoord[i], -bbox2.z) + pos;

        vec3 boxa = a*axis1;
        vec3 boxb = b*axis1;

        if(isInsideBbox(boxa, {}, bbox1))
        {
            accum += a;
            n++;
        }

        if(isInsideBbox(boxb, {}, bbox1))
        {
            accum += b;
            n++;
        }
    }

    for(int i = 0; i < 4; i++)
    {
        vec3 a = axis1*vec3(bbox1.x*xcoord[i], bbox1.y*ycoord[i], bbox1.z);
        vec3 b = axis1*vec3(bbox1.x*xcoord[i], bbox1.y*ycoord[i], -bbox1.z);

        vec3 boxa = a*axis2;
        vec3 boxb = b*axis2;

        vec3 relpos = pos*axis2;

        if(isInsideBbox(boxa, relpos, bbox2))
        {
            accum += a;
            n++;
        }

        if(isInsideBbox(boxb, relpos, bbox2))
        {
            accum += b;
            n++;
        }
    }

    cinfo.point = accum*(1.0/n);
}

bool BBoxVsBBox(ContactInfo& cinfo,
                const vec3& pos1, const mat3& rot1, const vec3& bbox1,
                const vec3& pos2, const mat3& rot2, const vec3& bbox2)
{
    vec3 pos = pos2 - pos1;

    for(int i = 0; i < 3; i++)
    {
        const vec3& dir = rot1[i];

        float x = dir*rot2[0];
        float y = dir*rot2[1];
        float z = dir*rot2[2];

        float sz1 = bbox1[i];
        float sz2 = bbox2.x*fabs(x) + bbox2.y*fabs(y) + bbox2.z*fabs(z);

        float dist = dir*pos;
        float radius = sz1 + sz2;

        if(fabs(dist) >= radius) return false;

        float penetration = radius - fabs(dist);

        if(i == 0 || penetration < cinfo.dist)
        {
            vec3 point = {};

            float sign = (dist < 0) ? -1 : 1;

            if(fabs(x) > math::eps) point.x = ((x < 0) ? bbox2.x : -bbox2.x)*sign;
            else point.x = intervalIntersection(-pos.x-bbox1.x, -pos.x+bbox1.x, -bbox2.x, bbox2.x);

            if(fabs(y) > math::eps) point.y = ((y < 0) ? bbox2.y : -bbox2.y)*sign;
            else point.y = intervalIntersection(-pos.y-bbox1.y, -pos.y+bbox1.y, -bbox2.y, bbox2.y);

            if(fabs(z) > math::eps) point.z = ((z < 0) ? bbox2.z : -bbox2.z)*sign;
            else point.z = intervalIntersection(-pos.z-bbox1.z, -pos.z+bbox1.z, -bbox2.z, bbox2.z);

            cinfo.dist = penetration;
            cinfo.norm = (dist >= 0) ? -rot1[i] : rot1[i];
            cinfo.point = pos2 + rot2*point;
        }
    }

    for(int i = 0; i < 3; i++)
    {
        const vec3& dir = rot2[i];

        float x = dir*rot1[0];
        float y = dir*rot1[1];
        float z = dir*rot1[2];

        float sz1 = bbox2[i];
        float sz2 = bbox1.x*fabs(x) + bbox1.y*fabs(y) + bbox1.z*fabs(z);

        float dist = dir*pos;
        float radius = sz1 + sz2;

        if(fabs(dist) >= radius) return false;

        float penetration = radius - fabs(dist);

        if(penetration < cinfo.dist)
        {
            vec3 point = {};

            float sign = (dist < 0) ? 1 : -1;

            if(fabs(x) > math::eps) point.x = ((x < 0) ? bbox1.x : -bbox1.x)*sign;
            else point.x = intervalIntersection(pos.x-bbox2.x, pos.x+bbox2.x, -bbox1.x, bbox1.x);

            if(fabs(y) > math::eps) point.y = ((y < 0) ? bbox1.y : -bbox1.y)*sign;
            else point.y = intervalIntersection(pos.y-bbox2.y, pos.y+bbox2.y, -bbox1.y, bbox1.y);

            if(fabs(z) > math::eps) point.z = ((z < 0) ? bbox1.z : -bbox1.z)*sign;
            else point.z = intervalIntersection(pos.z-bbox2.z, pos.z+bbox2.z, -bbox1.z, bbox1.z);

            cinfo.dist = penetration;
            cinfo.norm = (dist >= 0) ? -rot2[i] : rot2[i];
            cinfo.point = pos1 + rot1*point;
        }
    }

    for(int i = 0; i < 3; i++)
    {
        for(int k = 0; k < 3; k++)
        {
            vec3 dir = rot1[i]^rot2[k];
            float len = dir.normalize();

            if(fabs(len) < math::eps) continue;

            float sz1 = bbox1.x*fabs(dir*rot1[0]) + bbox1.y*fabs(dir*rot1[1]) + bbox1.z*fabs(dir*rot1[2]);
            float sz2 = bbox2.x*fabs(dir*rot2[0]) + bbox2.y*fabs(dir*rot2[1]) + bbox2.z*fabs(dir*rot2[2]);

            float dist = dir*pos;
            float radius = sz1 + sz2;

            if(fabs(dist) >= radius) return false;

            float penetration = radius - fabs(dist);

            if(penetration < cinfo.dist)
            {
                float s;
                float t;

                float sign = (dist >= 0) ? 1 : -1;

                rayOverlap(pos1, pos2, rot1[i], rot2[k], s, t);

                vec3 pt1 = pos1;
                vec3 pt2 = pos2;

                for(int l = 0; l < 3; l++)
                {
                    pt1 += rot1[l] * ((l == i) ? s : bbox1[l] * sgn(rot1[l]*dir) * sign);
                    pt2 += rot2[l] * ((l == k) ? t : bbox2[l] * sgn(rot2[l]*dir) * -sign);
                }

                cinfo.point = (pt1 + pt2)*0.5;

                cinfo.dist = penetration;
                cinfo.norm = (dist >= 0) ? -dir : dir;
            }
        }
    }

    return true;
}

bool BoxCollisionShape::testCollision(const BoxCollisionShape * shape, ContactInfo& contactInfo) const
{
    return BBoxVsBBox(contactInfo, shape->m_pos, shape->m_orientation, shape->m_dimensions,
                        m_pos, m_orientation, m_dimensions);
}

bool BoxCollisionShape::testCollision(const PolygonalCollisionShape * shape, ContactInfo& contactInfo) const
{
    bool result = shape->testBBox(contactInfo, m_pos, m_orientation, m_dimensions);

    contactInfo.norm = -contactInfo.norm;

    return result;
}

bool BoxCollisionShape::testCollision(const CapsuleCollisionShape * shape, ContactInfo& contactInfo) const
{
    vec3 relpos = (shape->pos() - m_pos)*m_orientation;

    for(int i = 0; i < 3; i++)
    {
        float dist = fabs(relpos[i]);
        float radius = shape->radius() + m_dimensions[i];

        if(dist > radius) return false;

        float penetration = radius - dist;

        if(i == 0 || penetration < contactInfo.dist)
        {
            contactInfo.norm = relpos[i] > 0 ? m_orientation[i] : -m_orientation[i];
            contactInfo.dist = radius - dist;
        }
    }

    contactInfo.point = shape->pos();
    if(shape->radius() > contactInfo.dist) contactInfo.point -= contactInfo.norm*(shape->radius() - contactInfo.dist);

    return true;
}

bool BoxCollisionShape::traceRay(const vec3 & origin, const vec3 & ray, TraceRayInfo& tinfo) const
{
    vec3 lorigin = (origin - m_pos) * m_orientation;
    vec3 lray = ray * m_orientation;

    float tmin = 0;
    float tmax = std::numeric_limits<float>::infinity();

    size_t axis = 0;
    float dir = 1;

    for (size_t i = 0; i < 3; i++)
    {
        if (fabs(lray[i]) < math::eps)
        {
            if ((lorigin[i] < -m_dimensions[i]) || (lorigin[i] > m_dimensions[i])) return false;
            else continue;
        }

        float dst1 = (lorigin[i] - m_dimensions[i]) / -lray[i];
        float dst2 = (lorigin[i] + m_dimensions[i]) / -lray[i];

        float dmin;
        float dmax;

        float adir = 1;

        if (dst1 < dst2)
        {
            dmin = dst1;
            dmax = dst2;
        }
        else
        {
            dmin = dst2;
            dmax = dst1;

            adir = -1;
        }

        if (dmax < 0) return false;			// (dmin < dmax) condition probably covers this
        if (dmin < 0) dmin = 0;

        if (dmin > tmin) { tmin = dmin; axis = i; dir = adir; }
        if (dmax < tmax) tmax = dmax;

        if (tmin > tmax) return false;
    }

    tinfo.dist = tmin;
    tinfo.norm = m_orientation[axis] * dir;

    return true;
}

bool BoxCollisionShape::traceBBox(TraceInfo& tinfo, const vec3& bbox, const vec3 begin, const vec3 end) const
{
    constexpr float deps = 0.0001;

    vec3 rbegin = begin - m_pos;
    vec3 rend = end - m_pos;

    for(int i = 0; i < 3; i++)
    {
        const vec3& dir = m_orientation[i];

        float min = -m_dimensions[i];
        float max = m_dimensions[i];

        float r = bbox.x*fabs(dir.x) + bbox.y*fabs(dir.y) + bbox.z*fabs(dir.z);

        float bdist = rbegin*dir;
        float edist = rend*dir;

        float d1 = bdist + r;
        float d2 = edist + r;

        if(d1 <= min+deps && d2 <= min+deps) return false;

        if(d1 <= min+deps && d2 > min+deps)
        {
            if(fabs(min - d1) < 0.001) d1 = min;

            float frac = (min - d1)/(d2 - d1);

            if(frac < tinfo.fraction)
            {
                tinfo.fraction = frac;
                tinfo.norm = -dir;
                tinfo.dend = min - d2;
            }
        }

        d1 = bdist - r;
        d2 = edist - r;

        if(d1 >= max-deps && d2 >= max-deps) return false;

        if(d1 >= max-deps && d2 < max-deps)
        {
            if(fabs(d1 - max) < 0.001) d1 = max;

            float frac = (d1 - max)/(d1 - d2);

            if(frac < tinfo.fraction)
            {
                tinfo.fraction = frac;
                tinfo.norm = dir;
                tinfo.dend = d2 - max;
            }
        }
    }

    for(int i = 0; i < 3; i++)
    {
        vec3 dir = {};
        dir[i] = 1.0;

        float min = -bbox[i];
        float max = bbox[i];

        float r = m_dimensions.x*fabs(m_orientation[0]*dir) +
                m_dimensions.y*fabs(m_orientation[1]*dir) +
                m_dimensions.z*fabs(m_orientation[2]*dir);

        float bdist = -rbegin*dir;
        float edist = -rend*dir;

        float d1 = bdist + r;
        float d2 = edist + r;

        if(d1 <= min+deps && d2 <= min+deps) return false;

        if(d1 <= min+deps && d2 > min+deps)
        {
            if(fabs(min - d1) < 0.001) d1 = min;

            float frac = (min - d1)/(d2 - d1);

            if(frac < tinfo.fraction)
            {
                tinfo.fraction = frac;
                tinfo.norm = dir;
                tinfo.dend = min - d2;
            }
        }

        d1 = bdist - r;
        d2 = edist - r;

        if(d1 >= max-deps && d2 >= max-deps) return false;

        if(d1 >= max-deps && d2 < max-deps)
        {
            if(fabs(d1 - max) < 0.001) d1 = max;

            float frac = (d1 - max)/(d1 - d2);

            if(frac < tinfo.fraction)
            {
                tinfo.fraction = frac;
                tinfo.norm = -dir;
                tinfo.dend = d2 - max;
            }
        }
    }

    for(int i = 0; i < 3; i++)
    {
        for(int k = 0; k < 3; k++)
        {
            vec3 axis = {};
            axis[k] = 1.0;

            vec3 dir = m_orientation[i]^axis;
            float len = dir.normalize();

            if(fabs(len) < math::eps) continue;

            float sz1 = m_dimensions.x*fabs(m_orientation[0]*dir) +
                        m_dimensions.y*fabs(m_orientation[1]*dir) +
                        m_dimensions.z*fabs(m_orientation[2]*dir);

            float sz2 = bbox.x*fabs(dir.x) + bbox.y*fabs(dir.y) + bbox.z*fabs(dir.z);

            float min = -sz1;
            float max = sz1;

            float bdist = rbegin*dir;
            float edist = rend*dir;

            float d1 = bdist + sz2;
            float d2 = edist + sz2;

            if(d1 <= min+deps && d2 <= min+deps) return false;

            if(d1 <= min+deps && d2 > min+deps)
            {
                if(fabs(min - d1) < 0.001) d1 = min;

                float frac = (min - d1)/(d2 - d1);

                if(frac < tinfo.fraction)
                {
                    tinfo.fraction = frac;
                    tinfo.norm = -dir;
                    tinfo.dend = min - d2;
                }
            }

            d1 = bdist - sz2;
            d2 = edist - sz2;

            if(d1 >= max-deps && d2 > max-deps) return false;

            if(d1 >= max-deps && d2 < max-deps)
            {
                if(fabs(d1 - max) < 0.001) d1 = max;

                float frac = (d1 - max)/(d1 - d2);

                if(frac < tinfo.fraction)
                {
                    tinfo.fraction = frac;
                    tinfo.norm = dir;
                    tinfo.dend = d2 - max;
                }
            }
        }
    }

    return true;
}

bool BoxCollisionShape::testHeight(const vec3& pos, const vec3& bbox, float& height, float& tilt) const
{
    return false;
}

} //namespace Collision
