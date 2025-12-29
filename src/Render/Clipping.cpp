#include "Render/Clipping.h"
#include "Collision/Collision.h"

namespace Render
{

namespace Clipping
{

bool FrustumTest(vec3 pos, const vec3* verts, const vec4& plane, vec3 box, vec3 bpos, const vec3* axis)
{
    constexpr size_t vnum = 4;

    float r = fabs(axis[0] * plane.xyz) * box.x + fabs(axis[1] * plane.xyz) * box.y + fabs(axis[2] * plane.xyz) * box.z;

    float d1 = plane.xyz * pos + plane.w;
    float d2 = plane.xyz * bpos + plane.w;

    //if ((d1 >= 0) && (d2 - r >= 0)) return false;
    //if ((d1 <= 0) && (d2 + r <= 0)) return false;

    if (d2 + r < 0) return false;

    for (int v = 0; v < vnum; v++)
    {
        int i1, i2;

        i1 = v;
        i2 = ((v + 1) == vnum) ? 0 : v + 1;

        vec3 edge1 = verts[i1] - pos;
        vec3 edge2 = verts[i2] - pos;

        vec3 norm = edge1 ^ edge2;
        norm.normalize();

        if (d1 < 0) norm = -norm;

        float dist = -pos * norm;

        float r = fabs(axis[0] * norm) * box.x + fabs(axis[1] * norm) * box.y + fabs(axis[2] * norm) * box.z;
        float d = norm * bpos + dist - r;

        if (d > 0) return false;
    }

    return true;
}

bool FrustumAABBVis(const vec3& pos, const vec3* frustum, const vec4& plane, const vec3& bbpos, const vec3& size)
{
    static const vec3 axis[3] = { {1, 0, 0},
                                  {0, 1, 0},
                                  {0, 0, 1} };

    return FrustumTest(pos, frustum, plane, size, bbpos, axis);
}

bool FrustumLeafVis(const vec3& pos, const vec3* frustum, const vec4& plane, const BBox& bbox)
{
    vec3 mid = (bbox.min + bbox.max) * 0.5;
    vec3 bbpos = mid;

    vec3 box = bbox.max - mid;

    static const vec3 axis[3] = { {1, 0, 0},
                                  {0, 1, 0},
                                  {0, 0, 1} };

    return FrustumTest(pos, frustum, plane, box, bbpos, axis);
}

bool FrustumObjVis(const vec3& pos, const vec3* frustum, const vec4& plane, const DisplayObject& obj)
{
    const BBox& bbox = obj.bbox();
    const mat4& mat = obj.mat();

    vec3 opos = mat[3];

    const mat3 rot = mat;

    vec3 mid = (bbox.min + bbox.max) * 0.5;
    vec3 bbpos = opos + rot * mid;

    vec3 box = bbox.max - mid;

    return FrustumTest(pos, frustum, plane, box, bbpos, &rot[0]);
}

bool FrustumAABBVis(const Frustum& frustum, const vec3& bbpos, const vec3& size)
{
    return frustum.test(size, bbpos);
}

bool FrustumLeafVis(const Frustum& frustum, const vec4& plane, const BBox& bbox)
{
    vec3 mid = (bbox.min + bbox.max) * 0.5;
    vec3 bbpos = mid;

    vec3 box = bbox.max - mid;

    static const vec3 axis[3] = { {1, 0, 0},
                                  {0, 1, 0},
                                  {0, 0, 1} };

    return frustum.test(box, bbpos);
}

bool FrustumObjVis(const Frustum& frustum, const vec4& plane, const DisplayObject& obj)
{
    const BBox& bbox = obj.bbox();
    const mat4& mat = obj.mat();

    vec3 opos = mat[3];

    const mat3 rot = mat;

    vec3 mid = (bbox.min + bbox.max) * 0.5;
    vec3 bbpos = opos + rot * mid;

    vec3 box = bbox.max - mid;

    return frustum.test(box, bbpos, rot);
}

float PlaneDist(const vec4& plane, const DisplayObject* object)
{
    const BBox& bbox = object->bbox();
    const mat4& mat = object->mat();

    vec3 opos = mat[3];

    const mat3 rot = mat;

    vec3 mid = (bbox.min + bbox.max) * 0.5;
    vec3 bpos = opos + rot * mid;

    vec3 box = bbox.max - mid;

    float r = fabs(rot[0] * plane.xyz) * box.x + fabs(rot[1] * plane.xyz) * box.y + fabs(rot[2] * plane.xyz) * box.z;
    float dist = plane.xyz * bpos + plane.w;

    return dist - r;
}

bool OBBVis(vec3 pos, const Portal& portal, vec3 box, vec3 bpos, vec3* axis)
{
    unsigned long vnum;
    const vec3* verts;

    if (portal.bid == -1)
    {
        vnum = portal.verts.size();
        verts = portal.verts.data();
    }
    else
    {
        vnum = portal.vbuff[portal.bid].size();
        verts = portal.vbuff[portal.bid].data();
    }

    float r = fabs(axis[0] * portal.plane.xyz) * box.x + fabs(axis[1] * portal.plane.xyz) * box.y + fabs(axis[2] * portal.plane.xyz) * box.z;

    float d1 = portal.plane.xyz * pos + portal.plane.w;
    float d2 = portal.plane.xyz * bpos + portal.plane.w;

    //all leaves on this side of portal are not considered
    if ((d1 >= 0) && (d2 - r >= 0)) return false;
    if ((d1 <= 0) && (d2 + r <= 0)) return false;

    for (int v = 0; v < vnum; v++)
    {
        int i1, i2;

        i1 = v;
        if ((v + 1) == vnum) i2 = 0;
        else i2 = v + 1;

        vec3 edge1 = verts[i1] - pos;
        vec3 edge2 = verts[i2] - pos;

        vec3 norm = edge1 ^ edge2;
        norm.normalize();

        if (d1 < 0) norm = -norm;

        float dist = -pos * norm;

        float r = fabs(axis[0] * norm) * box.x + fabs(axis[1] * norm) * box.y + fabs(axis[2] * norm) * box.z;
        float d = norm * bpos + dist - r;

        if (d > 0) return false;
    }

    return true;
}

bool AABBVis(const vec3& pos, const Portal& portal, const vec3& box, const vec3& bbpos)
{
    vec3 axis[3] = { {1, 0, 0},
                     {0, 1, 0},
                     {0, 0, 1} };

    return OBBVis(pos, portal, box, bbpos, axis);
}

bool OBBVisDir(const vec3& dir, const vec3* verts, unsigned long vnum, const vec3& box, const vec3& bpos, const vec3* axis)
{
    for (int v = 0; v < vnum; v++)
    {
        int i1 = v;
        int i2 = ((v + 1) == vnum) ? 0 : v + 1;

        vec3 edge = verts[i2] - verts[i1];

        vec3 norm = edge ^ dir;
        norm.normalize();

        float dist = -verts[i1] * norm;

        float r = fabs(axis[0] * norm) * box.x + fabs(axis[1] * norm) * box.y + fabs(axis[2] * norm) * box.z;
        float d = norm * bpos + dist - r;

        if (d > 0) return false;
    }

    for (int a = 0; a < 3; a++)
    {
        vec3 norm = axis[a] ^ dir;
        float len = norm.normalize();

        if (len < math::eps) continue;

        float min, max;
        Collision::supportFunction(norm, verts, vnum, min, max);

        float dist = bpos * norm;
        float r = fabs(axis[0] * norm) * box.x + fabs(axis[1] * norm) * box.y + fabs(axis[2] * norm) * box.z;

        if (dist + r < min) return false;
        if (dist - r > max) return false;
    }

    return true;
}

bool OBBVisDir(const vec3& dir, const Portal& portal, const vec3& box, const vec3& bpos, const vec3* axis)
{
    unsigned long vnum;
    const vec3* verts;

    if (portal.bid == -1)
    {
        vnum = portal.verts.size();
        verts = portal.verts.data();
    }
    else
    {
        vnum = portal.vbuff[portal.bid].size();
        verts = portal.vbuff[portal.bid].data();
    }

    float r = fabs(axis[0] * portal.plane.xyz) * box.x + fabs(axis[1] * portal.plane.xyz) * box.y + fabs(axis[2] * portal.plane.xyz) * box.z;

    float d1 = portal.plane.xyz * dir;
    float d2 = portal.plane.xyz * bpos + portal.plane.w;

    if (fabs(d1) < math::eps) return false;

    //do not consider leaves on this side of portal
    if ((d1 < 0) && (d2 - r >= 0)) return false;
    if ((d1 > 0) && (d2 + r < 0)) return false;

    for (int v = 0; v < vnum; v++)
    {
        int i1 = v;
        int i2 = ((v + 1) == vnum) ? 0 : v + 1;

        vec3 edge = verts[i2] - verts[i1];

        vec3 norm = edge ^ dir;
        norm.normalize();

        if (d1 < 0) norm = -norm;

        float dist = -verts[i1] * norm;

        float r = fabs(axis[0] * norm) * box.x + fabs(axis[1] * norm) * box.y + fabs(axis[2] * norm) * box.z;
        float d = norm * bpos + dist - r;

        if (d > 0) return false;
    }

    for (int a = 0; a < 3; a++)
    {
        vec3 norm = axis[a] ^ dir;
        float len = norm.normalize();

        if (len < math::eps) continue;

        float min, max;
        Collision::supportFunction(norm, verts, vnum, min, max);

        float dist = bpos * norm;
        float r = fabs(axis[0] * norm) * box.x + fabs(axis[1] * norm) * box.y + fabs(axis[2] * norm) * box.z;

        if (dist + r < min) return false;
        if (dist - r > max) return false;
    }

    return true;
}

bool LeafVis(const vec3& pos, const Portal& portal, const BBox& bbox)
{
    vec3 mid = (bbox.min + bbox.max) * 0.5;
    vec3 bbpos = mid;

    vec3 box = bbox.max - mid;

    vec3 axis[3] = { {1, 0, 0},
                    {0, 1, 0},
                    {0, 0, 1} };

    return OBBVis(pos, portal, box, bbpos, axis);
}

bool LeafVisDir(const vec3& dir, const vec3* verts, unsigned long vnum, const BBox& bbox)
{
    vec3 mid = (bbox.min + bbox.max) * 0.5;
    vec3 bbpos = mid;

    vec3 box = bbox.max - mid;

    const vec3 axis[3] = { {1, 0, 0},
                            {0, 1, 0},
                            {0, 0, 1} };

    return OBBVisDir(dir, verts, vnum, box, bbpos, axis);
}

bool LeafVisDir(const vec3& dir, const Portal& portal, const BBox& bbox)
{
    vec3 mid = (bbox.min + bbox.max) * 0.5;
    vec3 bbpos = mid;

    vec3 box = bbox.max - mid;

    const vec3 axis[3] = { {1, 0, 0},
                            {0, 1, 0},
                            {0, 0, 1} };

    return OBBVisDir(dir, portal, box, bbpos, axis);
}

bool ObjVis(const vec3& pos, const Portal& portal, const DisplayObject* obj)
{
    vec3 opos = obj->mat()[3];

    const BBox& bbox = obj->bbox();
    mat3 rot = obj->mat();

    vec3 mid = (bbox.min + bbox.max) * 0.5;
    vec3 bbpos = opos + rot * mid;

    vec3 box = bbox.max - mid;

    return OBBVis(pos, portal, box, bbpos, &rot[0]);
}

bool ObjVisDir(const vec3& dir, const vec3* verts, unsigned long vnum, const DisplayObject* obj)
{
    vec3 opos = obj->mat()[3];

    const BBox& bbox = obj->bbox();
    mat3 rot = obj->mat();

    vec3 mid = (bbox.min + bbox.max) * 0.5;
    vec3 bbpos = opos + rot * mid;

    vec3 box = bbox.max - mid;

    return OBBVisDir(dir, verts, vnum, box, bbpos, &rot[0]);
}

bool ObjVisDir(const vec3& dir, const Portal& portal, const DisplayObject* obj)
{
    vec3 opos = obj->mat()[3];

    const BBox& bbox = obj->bbox();
    mat3 rot = obj->mat();

    vec3 mid = (bbox.min + bbox.max) * 0.5;
    vec3 bbpos = opos + rot * mid;

    vec3 box = bbox.max - mid;

    return OBBVisDir(dir, portal, box, bbpos, &rot[0]);
}

void PortalSplit(const vec3& norm, float D, std::vector<vec3>& vin, std::vector<vec3>& vout)
{
    vout.clear();

    for (int v = 0; v < vin.size(); v++)
    {
        int e1 = v;
        int e2 = ((v + 1) == vin.size()) ? 0 : v + 1;

        float d1 = vin[e1] * norm + D;
        float d2 = vin[e2] * norm + D;

        bool split = true;

        if ((d1 > 0) && (d2 > 0)) split = false;
        if ((d1 < 0) && (d2 < 0)) split = false;

        vec3 midpt;

        if (split)
        {
            float frac = d1 / (d1 - d2);

            vec3 dir = vin[e2] - vin[e1];
            float len = dir.normalize();
            len = frac * len;

            dir = dir * len;
            midpt = vin[e1] + dir;

            vout.push_back(midpt);
        }

        if (d2 < -math::eps) vout.push_back(vin[e2]);
    }
}

bool PortalVis(const vec3& pos, Portal& portal, Portal& tport)
{
    float d1 = tport.plane.xyz * pos + tport.plane.w;
    float d2 = tport.plane.xyz * portal.center + tport.plane.w;

    if (fabs(d1) < math::eps) return false;

    // if pos is behind tested portal
    if ((d1 < 0.0f) && (d2 >= 0.0f)) return false;
    if ((d1 >= 0.0f) && (d2 < 0.0f)) return false;

    std::vector<vec3>& verts = (portal.bid == -1) ? portal.verts : portal.vbuff[portal.bid];

    if (verts.size() < 3) return false;

    tport.bid = -1;

    float d = portal.plane.xyz * pos + portal.plane.w;

    for (int v = 0; v < verts.size(); v++)
    {
        int i1 = v;
        int i2 = ((v + 1) == verts.size()) ? 0 : v + 1;

        if (d < 0)
        {
            int tmp = i2;
            i2 = i1;
            i1 = tmp;
        }

        vec3 edge1 = verts[i1] - pos;
        vec3 edge2 = verts[i2] - pos;

        vec3 norm = edge1 ^ edge2;
        norm.normalize();

        float dist = -pos * norm;

        std::vector<vec3>& vin = (tport.bid == -1) ? tport.verts : tport.vbuff[tport.bid];
        std::vector<vec3>& vout = (tport.bid == 0) ? tport.vbuff[1] : tport.vbuff[0];

        PortalSplit(norm, dist, vin, vout);

        tport.bid = (tport.bid == 0) ? 1 : 0;

        if (vout.size() <= 2) return false;
    }

    return true;
}

bool PortalVisDir(const vec3& dir, Portal& portal, Portal& tport)
{
    std::vector<vec3>& verts = (portal.bid == -1) ? portal.verts : portal.vbuff[portal.bid];

    if (verts.size() < 3) return false;

    tport.bid = -1;

    float d = portal.plane.xyz * dir;

    for (int v = 0; v < verts.size(); v++)
    {
        int i1 = v;
        int i2 = ((v + 1) == verts.size()) ? 0 : v + 1;

        vec3 edge = verts[i2] - verts[i1];

        vec3 norm = edge ^ dir;
        norm.normalize();

        if (d < 0) norm = -norm;

        float dist = -verts[i1] * norm;

        std::vector<vec3>& vin = (tport.bid == -1) ? tport.verts : tport.vbuff[tport.bid];
        std::vector<vec3>& vout = (tport.bid == 0) ? tport.vbuff[1] : tport.vbuff[0];

        PortalSplit(norm, dist, vin, vout);

        tport.bid = (tport.bid == 0) ? 1 : 0;

        if (vout.size() <= 2) return false;
    }

    return true;
}

bool PortalVisFrustum(const vec3& pos, const vec3* frustum, const vec4& plane, Portal& tport)
{
    constexpr size_t vnum = 4;

    tport.bid = -1;

    //float d = tport.plane.xyz * pos + tport.plane.w;
    //if (fabs(d) < math::eps) return false;

    for (int v = 0; v < vnum; v++)
    {
        int i1 = v;
        int i2 = ((v + 1) == vnum) ? 0 : v + 1;

        /*if (d < 0)
        {
            int tmp = i2;
            i2 = i1;
            i1 = tmp;
        }*/

        vec3 edge1 = frustum[i1] - pos;
        vec3 edge2 = frustum[i2] - pos;

        vec3 norm = edge1 ^ edge2;
        norm.normalize();

        float dist = -pos * norm;

        std::vector<vec3>& vin = (tport.bid == -1) ? tport.verts : tport.vbuff[tport.bid];
        std::vector<vec3>& vout = (tport.bid == 0) ? tport.vbuff[1] : tport.vbuff[0];

        PortalSplit(norm, dist, vin, vout);

        tport.bid = (tport.bid == 0) ? 1 : 0;

        if (vout.size() <= 2) return false;
    }


    return true;
}

} //namespace clipping

} //namespace render