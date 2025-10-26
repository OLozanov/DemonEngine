#include "math3d.h"
#include <cfloat>

#include <limits>

bool AABBDynTest(const vec3& mbox, const vec3& sbox, const vec3& begin, const vec3& end, const vec3& pos)
{
    for (int a = 0; a < 3; a++)
    {
        constexpr float deps = 0.02;

        float min = begin[a] < end[a] ? begin[a] : end[a];
        float max = begin[a] < end[a] ? end[a] : begin[a];

        float mbox_min = min - mbox[a];
        float mbox_max = max + mbox[a];

        float sbox_min = pos[a] - sbox[a] - deps;
        float sbox_max = pos[a] + sbox[a] + deps;

        if (mbox_max < sbox_min) return false;
        if (mbox_min > sbox_max) return false;
    }

    return true;
}

bool AABBTest(const vec3& pos1, const vec3& bbox1, const vec3& pos2, const vec3& bbox2)
{
    for (int a = 0; a < 3; a++)
        if (fabs(pos2[a] - pos1[a]) > bbox1[a] + bbox2[a]) return false;

    return true;
}

bool BBoxIntersect(const vec3& pos1, const BBox& bbox1, const vec3& pos2, const BBox& bbox2)
{
    for (int a = 0; a < 3; a++)
    {
        if (pos1[a] + bbox1.max[a] < pos2[a] + bbox2.min[a]) return false;
        if (pos2[a] + bbox2.max[a] < pos1[a] + bbox1.min[a]) return false;
    }

    return true;
}

bool BBoxRayTest(const vec3& origin, const vec3& ray, const vec3& pos, const vec3& bbox, const mat3& axis, float& dist)
{
    float tmin = 0;
    float tmax = std::numeric_limits<float>::infinity();

    for (int i = 0; i < 3; i++)
    {
        float Cos = -(ray * axis[i]);

        float d = pos * axis[i];
        float dst = origin * axis[i] - d;

        if (fabs(Cos) < 0.001)
        {
            if ((dst < -bbox[i]) || (dst > bbox[i])) return false;
            else continue;
        }

        float dst1 = (dst - bbox[i]) / Cos;
        float dst2 = (dst + bbox[i]) / Cos;

        float dmin;
        float dmax;

        if (dst1 < dst2)
        {
            dmin = dst1;
            dmax = dst2;
        }
        else
        {
            dmin = dst2;
            dmax = dst1;
        }

        if (dmax < 0) return false;			// (dmin < dmax) condition probably covers this
        if (dmin < 0) dmin = 0;

        if (dmin > tmin) tmin = dmin;
        if (dmax < tmax) tmax = dmax;

        if (tmin > tmax) return false;
    }

    //pt = origin + ray * tmin;		//For debug purposes

    dist = tmin;

    return true;
}

BBox RecalcBBox(const BBox& bbox, const mat3& mat)
{
    vec3 max = { 0, 0, 0 };
    vec3 min = { 0, 0, 0 };

    vec3 verts[8];

    verts[0] = mat * bbox.min;
    verts[1] = mat * vec3(bbox.max.x, bbox.min.y, bbox.min.z);
    verts[2] = mat * vec3(bbox.max.x, bbox.min.y, bbox.max.z);
    verts[3] = mat * vec3(bbox.min.x, bbox.min.y, bbox.max.z);

    verts[4] = mat * vec3(bbox.min.x, bbox.max.y, bbox.min.z);
    verts[5] = mat * vec3(bbox.max.x, bbox.max.y, bbox.min.z);
    verts[6] = mat * bbox.max;
    verts[7] = mat * vec3(bbox.min.x, bbox.max.y, bbox.max.z);

    for (int i = 0; i < 8; i++)
    {
        for (int k = 0; k < 3; k++)
        {
            if (verts[i][k] > max[k]) max[k] = verts[i][k];
            if (verts[i][k] < min[k]) min[k] = verts[i][k];
        }
    }

    return { min, max };
}