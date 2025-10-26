#include "Collision/PolygonalCollisionShape.h"
#include "Collision/BoxCollisionShape.h"
#include "Collision/CapsuleCollisionShape.h"

//#include "stdio.h"

namespace Collision
{

void bboxPlaneContact(const vec3 & norm, float d, const mat3& axis, const vec3 & bbox, const vec3& pos, vec3& point)
{
    float x = norm*axis[0];
    float y = norm*axis[1];
    float z = norm*axis[2];

    float r = bbox.x*fabs(x) + bbox.y*fabs(y) + bbox.z*fabs(z);

    point = {};

    if(fabs(x) > math::eps) point += axis[0]*((x < 0) ? bbox.x : -bbox.x);
    if(fabs(y) > math::eps) point += axis[1]*((y < 0) ? bbox.y : -bbox.y);
    if(fabs(z) > math::eps) point += axis[2]*((z < 0) ? bbox.z : -bbox.z);

    float dist = norm*pos + d;
    float frac = dist/r;

    point *= frac;
    point += pos;
}

void PolygonalCollisionShape::findContactPoint(ContactInfo & cinfo,
                                        const CollisionPolygon& polygon,
                                        const vec3 & pos, const mat3& axis, const vec3 & bbox)
{
    vec3 accum = {};
    int n = 0;

    static const int xcoord[] = {-1, 1, 1, -1};
    static const int ycoord[] = {-1, -1, 1, 1};

    //Check box edges
    for(int i = 0; i < 4; i++)
    {
        int k = (i == 3) ? 0 : i + 1;

        vec3 a = pos + axis*vec3(bbox.x*xcoord[i], bbox.y*ycoord[i], bbox.z);
        vec3 b = pos + axis*vec3(bbox.x*xcoord[k], bbox.y*ycoord[k], bbox.z);

        vec3 pt;

        if(lineIntersection(polygon.plane.xyz, polygon.plane.w, a, b, pt) && polygon.isPointInside(pt))
        {
            accum += pt;
            n++;
        }

        a = pos + axis*vec3(bbox.x*xcoord[i], bbox.y*ycoord[i], -bbox.z);
        b = pos + axis*vec3(bbox.x*xcoord[k], bbox.y*ycoord[k], -bbox.z);

        if(lineIntersection(polygon.plane.xyz, polygon.plane.w, a, b, pt) && polygon.isPointInside(pt))
        {
            accum += pt;
            n++;
        }

        a = pos + axis*vec3(bbox.x*xcoord[i], bbox.y*ycoord[i], -bbox.z);
        b = pos + axis*vec3(bbox.x*xcoord[i], bbox.y*ycoord[i], bbox.z);

        if(lineIntersection(polygon.plane.xyz, polygon.plane.w, a, b, pt) && polygon.isPointInside(pt))
        {
            accum += pt;
            n++;
        }
    }

    //Check polygon edges
    vec3 boxpos = pos*axis;

    for(int i = 0; i < polygon.verts.size(); i++)
    {
        int k = (i == (polygon.verts.size()-1)) ? k = 0 : k = i + 1;

        const vec3& a = polygon.verts[i];
        const vec3& b = polygon.verts[k];

        vec3 edge = b - a;
        float len = edge.normalize();

        vec3 boxa = a*axis;
        vec3 boxb = b*axis;
        vec3 bedge = edge*axis;

        if(isInsideBbox(boxa, boxpos, bbox))
        {
            accum += a;
            n++;
        }

        if(isInsideBbox(boxb, boxpos, bbox))
        {
            accum += b;
            n++;
        }

        for(int f = 0; f < 3; f++)
        {
            if(fabs(bedge[f]) < math::eps) continue;

            float slope = 1.0/bedge[f];

            float adst = (boxpos[f] - boxa[f]);
            float d1 = (adst - bbox[f])*slope;
            float d2 = (adst + bbox[f])*slope;

            if(d1 > 0 && d1 < len)
            {
                vec3 intersect = boxa + bedge*d1;

                if(isInsideBbox(intersect, boxpos, bbox))
                {
                    accum += axis*intersect;
                    n++;
                }
            }

            if(d2 > 0 && d2 < len)
            {
                vec3 intersect = boxa + bedge*d2;

                if(isInsideBbox(intersect, boxpos, bbox))
                {
                    accum += axis*intersect;
                    n++;
                }
            }
        }
    }

    if(n == 0) n = 1;

    cinfo.point = accum*(1.0/n);
}

void PolygonalCollisionShape::findIntersections(vec3& accum, int& n,
                                            const CollisionPolygon& polygonA,
                                            const vec3 & pos, const mat3& axis,
                                            const CollisionPolygon& polygonB)
{
    //Intersections of polygon A by polygon B
    for(int i = 0; i < polygonB.verts.size(); i++)
    {
        int k = (i == (polygonB.verts.size()-1)) ? 0 : i + 1;

        vec3 a = axis*polygonB.verts[i] + pos;
        vec3 b = axis*polygonB.verts[k] + pos;

        vec3 pt;

        if(lineIntersection(polygonA.plane.xyz, polygonA.plane.w, a, b, pt) && polygonA.isPointInside(pt))
        {
            accum += pt;
            n++;
        }
    }

    //Intersections of polygon B by polygon A
    for(int i = 0; i < polygonA.verts.size(); i++)
    {
        int k = (i == (polygonA.verts.size()-1)) ? 0 : i + 1;

        vec3 a = (polygonA.verts[i] - pos)*axis;
        vec3 b = (polygonA.verts[k] - pos)*axis;

        vec3 pt;

        if(lineIntersection(polygonB.plane.xyz, polygonB.plane.w, a, b, pt) && polygonB.isPointInside(pt))
        {
            accum += axis*pt + pos;
            n++;
        }
    }
}

bool PolygonalCollisionShape::testPolygon(ContactInfo & cinfo,
                                        const CollisionPolygon& polygonA,
                                        const vec3 & pos, const mat3& axis,
                                        const CollisionPolygon& polygonB)
{
    float min, max;

    //Polygon A normal
    vec3 dir = polygonA.plane.xyz*axis;
    float dist = polygonA.plane.xyz*pos + polygonA.plane.w;

    polygonB.supportFunction(dir, min, max);

    if(dist + min > 0) return false;
    if(dist + max < 0) return false;

    float penetration = fabs(dist + min);

    cinfo.dist = penetration;
    cinfo.norm = polygonA.plane.xyz;

    //Polygon B normal
    dir = axis*polygonB.plane.xyz;
    dist = dir*-pos + polygonB.plane.w;

    polygonA.supportFunction(dir, min, max);

    if(dist + min > 0) return false;
    if(dist + max < 0) return false;

    penetration = fabs(dist + min);

    if(penetration < cinfo.dist)
    {
        cinfo.dist = penetration;
        cinfo.norm = -dir;
    }

    //Edges
    for(int i = 0; i < polygonA.verts.size(); i++)
    {
        int k = (i == (polygonA.verts.size()-1)) ? 0 : i + 1;

        vec3 edgeA = polygonA.verts[k] - polygonA.verts[i];
        edgeA.normalize();

        for(int l = 0; l < polygonB.verts.size(); l++)
        {
            int m = (l == (polygonB.verts.size()-1)) ? 0 : l + 1;

            vec3 edgeB = axis*(polygonB.verts[m] - polygonB.verts[l]);
            edgeB.normalize();

            vec3 dir = edgeA^edgeB;
            float edgeSin = dir.normalize();

            if(edgeSin < 0.001) continue;

            float a, b, c, d;

            polygonA.supportFunction(dir, a, b);
            polygonB.supportFunction(dir*axis, c, d);

            float dst = dir*pos;

            c += dst;
            d += dst;

            if(c > b) return false;

            penetration = b - c;

            if(penetration < cinfo.dist)
            {
                cinfo.dist = penetration;
                cinfo.norm = dir;
            }

            if(a > d) return false;

            penetration = -(a - d);

            if(penetration < cinfo.dist)
            {
                cinfo.dist = penetration;
                cinfo.norm = -dir;
            }
        }
    }
}

bool PolygonalCollisionShape::testPolygonal(ContactInfo& cinfo,
                                            const vec3& pos,
                                            const mat3& mat,
                                            const CollisionPolygon * polygons,
                                            unsigned long pnum) const
{
    bool result = false;
    vec3 contactPt = {};
    int contactNum = 0;

    vec3 relpos = (pos - m_pos)*m_orientation;
    mat3 axis = {mat[0]*m_orientation, mat[1]*m_orientation, mat[2]*m_orientation};

    for(int i = 0; i < m_pnum; i++)
    {
        for(int k = 0; k < pnum; k++)
        {
            ContactInfo plgContactInfo;

            if(testPolygon(plgContactInfo, m_polygons[i], relpos, axis, polygons[k]))
            {
                findIntersections(contactPt, contactNum, m_polygons[i], relpos, axis, polygons[k]);

                if(!result || plgContactInfo.dist > cinfo.dist)
                {
                    result = true;

                    cinfo.norm = plgContactInfo.norm;
                    cinfo.dist = plgContactInfo.dist;
                }
            }
        }
    }

    if(result)
    {
        if(contactNum == 0) contactNum = 1;

        cinfo.norm = m_orientation*cinfo.norm;
        cinfo.point = m_pos +  m_orientation*(contactPt*(1.0/contactNum));
    }

    return result;
}

bool PolygonalCollisionShape::testPolygon(ContactInfo & cinfo,
                                        const CollisionPolygon& polygon,
                                        const vec3 & pos, const mat3& axis, const vec3 & bbox)
{
    //Test polygon plane
    float r = bbox.x*fabs(polygon.plane.xyz*axis[0]) +
            bbox.y*fabs(polygon.plane.xyz*axis[1]) +
            bbox.z*fabs(polygon.plane.xyz*axis[2]);

    float dist = polygon.plane.xyz*pos + polygon.plane.w;

    if(fabs(dist) > r) return false;

    float normPenetration = r - dist;

    cinfo.norm = polygon.plane.xyz;
    cinfo.dist = normPenetration;

    //Test box faces
    for(int i = 0; i < 3; i++)
    {
        const vec3& dir = axis[i];

        float max;
        float min;

        polygon.supportFunction(dir, min, max);

        float dist = pos*dir;

        if(dist + bbox[i] < min) return false;

        float penetration = dist + bbox[i] - min;

        if(penetration < cinfo.dist)
        {
            cinfo.dist = penetration;
            cinfo.norm = -dir;
        }

        if(dist - bbox[i] > max) return false;

        penetration = max - (dist - bbox[i]);

        if(penetration < cinfo.dist)
        {
            cinfo.dist = penetration;
            cinfo.norm = dir;
        }
    }

    //Test polygon edges
    for(int i = 0; i < polygon.verts.size(); i++)
    {
        int k = (i == (polygon.verts.size()-1)) ? k = 0 : k = i + 1;

        vec3 edge = polygon.verts[k] - polygon.verts[i];
        edge.normalize();

        for(int a = 0; a < 3; a++)
        {
            vec3 dir = edge^axis[a];
            float n = dir.normalize();

            if(n < 0.001) continue;

            float max;
            float min;

            polygon.supportFunction(dir, min, max);

            float sz = bbox.x*fabs(dir*axis[0]) +
                    bbox.y*fabs(dir*axis[1]) +
                    bbox.z*fabs(dir*axis[2]);

            float bdist = pos*dir;

            if(bdist + sz < min) return false;

            float penetration = bdist + sz - min;

            if(penetration < cinfo.dist)
            {
                cinfo.dist = penetration;
                cinfo.norm = -dir;
            }

            if(bdist - sz > max) return false;

            penetration = max - (bdist - sz);

            if(penetration < cinfo.dist)
            {
                cinfo.dist = penetration;
                cinfo.norm = dir;
            }
        }
    }

    return true;
}

bool PolygonalCollisionShape::testBBox(ContactInfo & cinfo, const vec3 & pos, const mat3& mat, const vec3 & bbox) const
{
    bool result = false;
    int poly;

    vec3 relpos = (pos - m_pos)*m_orientation;
    mat3 axis = {mat[0]*m_orientation, mat[1]*m_orientation, mat[2]*m_orientation};

    for(int p = 0; p < m_pnum; p++)
    {
        ContactInfo plgContactInfo;

        if(testPolygon(plgContactInfo, m_polygons[p], relpos, axis, bbox))
        {
            if(!result || plgContactInfo.dist > cinfo.dist)
            {
                result = true;
                poly = p;

                cinfo.norm = plgContactInfo.norm;
                cinfo.dist = plgContactInfo.dist;
            }
        }
    }

    if(result)
    {
        findContactPoint(cinfo, m_polygons[poly], relpos, axis, bbox);

        cinfo.norm = m_orientation*cinfo.norm;
        cinfo.point = m_pos + m_orientation*cinfo.point;
    }

    return result;
}

bool PolygonalCollisionShape::testPolygon(ContactInfo& cinfo,
                                        const CollisionPolygon& polygon,
                                        const vec3& pos, const mat3& axis,
                                        float radius)
{
    float dist = polygon.plane.xyz*pos + polygon.plane.w;

    if(fabs(dist) > radius) return false;

    float normPenetration = radius - dist;

    cinfo.norm = polygon.plane.xyz;
    cinfo.dist = normPenetration;

    //Test polygon edges
    for(int i = 0; i < polygon.verts.size(); i++)
    {
        int k = (i == (polygon.verts.size()-1)) ? 0 : i + 1;

        vec3 edge = polygon.verts[k] - polygon.verts[i];
        float edgelen = edge.normalize();

        vec3 relpos = pos - polygon.verts[i];
        float edgeproj = relpos*edge;

        vec3 tang = edge^polygon.plane.xyz;
        float tdist = tang*relpos;

        if(tdist > radius) return false;
        if(tdist < 0) continue;

        if(edgeproj < 0)
        {
            vec3 norm = relpos;
            float dist = norm.normalize();

            if(dist > radius) return false;

            float penetration = radius - dist;

            if(penetration < cinfo.dist)
            {
                cinfo.norm = norm;
                cinfo.dist = penetration;
            }

            continue;
        }

        if(edgeproj > edgelen)
        {
            vec3 norm = pos - polygon.verts[k];
            float dist = norm.normalize();

            if(dist > radius) return false;

            float penetration = radius - dist;

            if(penetration < cinfo.dist)
            {
                cinfo.norm = norm;
                cinfo.dist = penetration;
            }

            continue;
        }

        vec3 norm = relpos - edge*edgeproj;
        float dist = norm.normalize();

        if(dist > radius) return false;

        float penetration = radius - dist;

        if(penetration < cinfo.dist)
        {
            cinfo.norm = norm;
            cinfo.dist = penetration;
        }
    }

    return true;
}

bool PolygonalCollisionShape::testPolygon(ContactInfo& cinfo,
                                          const CollisionPolygon& polygon,
                                          const vec3& pt1, const vec3& pt2,
                                          const mat3& axis, float radius)
{
    float dst1 = polygon.plane.xyz * pt1 + polygon.plane.w;
    float dst2 = polygon.plane.xyz * pt2 + polygon.plane.w;

    float dist = std::min(dst1, dst2);

    if (fabs(dist) > radius) return false;

    float penetration = radius - dist;

    cinfo.norm = polygon.plane.xyz;
    cinfo.dist = penetration;
    cinfo.point = (dst1 < dst2 ? pt1 : pt2) - polygon.plane.xyz * radius;

    return true;
}

bool PolygonalCollisionShape::testSphere(ContactInfo & cinfo, const CapsuleCollisionShape * sphere) const
{
    bool result = false;

    const vec3& pos = sphere->pos();
    const mat3& mat = sphere->orientation();

    vec3 relpos = (pos - m_pos)*m_orientation;
    mat3 axis = {mat[0]*m_orientation, mat[1]*m_orientation, mat[2]*m_orientation};

    float radius = sphere->radius();

    for(int p = 0; p < m_pnum; p++)
    {
        ContactInfo plgContactInfo;

        if(testPolygon(plgContactInfo, m_polygons[p], relpos, axis, radius))
        {
            if(!result || plgContactInfo.dist > cinfo.dist)
            {
                result = true;

                cinfo.norm = plgContactInfo.norm;
                cinfo.dist = plgContactInfo.dist;
                cinfo.point = plgContactInfo.point;
            }
        }
    }

    if(result)
    {
        cinfo.norm = m_orientation*cinfo.norm;
        cinfo.point = pos - cinfo.norm;
    }

    return result;
}

bool PolygonalCollisionShape::testCapsule(ContactInfo& cinfo, const CapsuleCollisionShape* capsule) const
{
    bool result = false;

    const vec3& pos = capsule->pos();
    const mat3& mat = capsule->orientation();

    vec3 relpos = (pos - m_pos) * m_orientation;
    mat3 axis = { mat[0] * m_orientation, mat[1] * m_orientation, mat[2] * m_orientation };

    float radius = capsule->radius();
    float length = capsule->length();

    vec3 pt1 = relpos - axis[1] * length * 0.5f;
    vec3 pt2 = relpos + axis[1] * length * 0.5f;

    for (int p = 0; p < m_pnum; p++)
    {
        ContactInfo plgContactInfo;

        if (testPolygon(plgContactInfo, m_polygons[p], pt1, pt2, axis, radius))
        {
            if (!result || plgContactInfo.dist > cinfo.dist)
            {
                result = true;

                cinfo.norm = plgContactInfo.norm;
                cinfo.dist = plgContactInfo.dist;
            }
        }
    }

    if (result)
    {
        cinfo.norm = m_orientation * cinfo.norm;
        cinfo.point = pos - cinfo.norm;
    }

    return result;
}

bool PolygonalCollisionShape::testCollision(const BoxCollisionShape * shape, ContactInfo& contactInfo) const
{
    return testBBox(contactInfo, shape->pos(), shape->orientation(), shape->getDimensions());
}

bool PolygonalCollisionShape::testCollision(const PolygonalCollisionShape * shape, ContactInfo& contactInfo) const
{
    return testPolygonal(contactInfo, shape->m_pos, shape->m_orientation, shape->m_polygons, shape->m_pnum);
}

bool PolygonalCollisionShape::testCollision(const CapsuleCollisionShape * shape, ContactInfo& contactInfo) const
{
    if (shape->length() > 0.0f)
        return testCapsule(contactInfo, shape);
    else
        return testSphere(contactInfo, shape);
}

bool PolygonalCollisionShape::tracePolygon(const CollisionPolygon& polygon,
                                        const vec3 & origin,
                                        const vec3 & ray,
                                        float & dist)
{
    float ndist = polygon.plane.xyz*origin + polygon.plane.w;

    if(ndist < 0) return false;

    float cos = ray*polygon.plane.xyz;

    if(cos > -math::eps) return false;

    dist = -(ndist/cos);
    vec3 intersection = origin + ray*dist;

    for(int i = 0; i < polygon.verts.size(); i++)
    {
        int k = (i == polygon.verts.size()-1) ? 0 : i + 1;

        vec3 edge = polygon.verts[k] - polygon.verts[i];
        edge.normalize();

        vec3 norm = edge^polygon.plane.xyz;
        float d = -norm*polygon.verts[i];

        float ndist = intersection*norm + d;

        if(ndist > 0) return false;
    }

    return true;
}

bool PolygonalCollisionShape::tracePolygon(TraceInfo& tinfo,
                                            const CollisionPolygon& polygon,
                                            const vec3& bbox,
                                            const mat3& axis,
                                            const vec3& begin, const vec3& end)
{
    constexpr float deps = 0.00001;

    //Test polygon plane
    float r = bbox.x*fabs(polygon.plane.xyz*axis[0]) +
            bbox.y*fabs(polygon.plane.xyz*axis[1]) +
            bbox.z*fabs(polygon.plane.xyz*axis[2]);

    float d1 = polygon.plane.xyz*begin + polygon.plane.w - r;
    float d2 = polygon.plane.xyz*end + polygon.plane.w - r;

    if(d1 < -r*2) return false;
    if((d1 >= -deps) && (d2 >= -deps)) return false;

    if(d1 >= -deps && d2 < -deps)
    {
        float frac = d1/(d1 - d2);

        vec3 point = begin + (end - begin)*frac;

        if(frac < tinfo.fraction && !polygon.findBoxSepAxis(point, bbox, axis))
        {
            tinfo.fraction = frac;
            tinfo.norm = polygon.plane.xyz;
            tinfo.dend = d2;
        }
    }

    bool testOnly = tinfo.fraction < 1.0;

    //Test box faces
    for(int i = 0; i < 3; i++)
    {
        const vec3& dir = axis[i];

        if(fabs(1 - fabs(dir*polygon.plane.xyz)) < deps) continue;

        float max;
        float min;

        polygon.supportFunction(dir, min, max);

        float bdist = begin*dir;
        float edist = end*dir;

        float d1 = bdist + bbox[i];
        float d2 = edist + bbox[i];

        if(d1 <= min+deps && d2 <= min+deps) return false;

        if(d1 <= min+deps && d2 > min+deps && !testOnly)
        {
            float frac = (min - d1)/(d2 - d1);
            vec3 point = begin + (end - begin)*frac;

            if(frac < tinfo.fraction)
            {
                tinfo.fraction = frac;
                tinfo.norm = -dir;
                tinfo.dend = min - d2;
            }
        }

        d1 = bdist - bbox[i];
        d2 = edist - bbox[i];

        if(d1 >= max-deps && d2 >= max-deps) return false;

        if(d1 >= max-deps && d2 < max-deps && !testOnly)
        {
            float frac = (d1 - max)/(d1 - d2);
            vec3 point = begin + (end - begin)*frac;

            if(frac < tinfo.fraction)
            {
                tinfo.fraction = frac;
                tinfo.norm = dir;
                tinfo.dend = d2 - max;
            }
        }
    }

    vec3 movedir = end - begin;
    float movedist = movedir.normalize();

    //Test polygon edges
    for(int i = 0; i < polygon.verts.size(); i++)
    {
        int k = (i == (polygon.verts.size()-1)) ? 0 : i + 1;

        vec3 edge = polygon.verts[k] - polygon.verts[i];
        edge.normalize();

        for(int a = 0; a < 3; a++)
        {
            vec3 dir = edge^axis[a];
            float n = dir.normalize();

            if(n < 0.001) continue;
            if(fabs(1 - fabs(dir*polygon.plane.xyz)) < deps) continue;

            float max;
            float min;

            polygon.supportFunction(dir, min, max);

            float sz = bbox.x*fabs(dir*axis[0]) +
                    bbox.y*fabs(dir*axis[1]) +
                    bbox.z*fabs(dir*axis[2]);

            float bdist = begin*dir;
            float edist = end*dir;

            float d1 = bdist + sz;
            float d2 = edist + sz;

            if(d1 <= min+deps && d2 <= min+deps) return false;

            if(d1 <= min+deps && d2 > min+deps && !testOnly)
            {
                float frac = (min - d1)/(d2 - d1);
                vec3 point = begin + (end - begin)*frac;

                if(frac < tinfo.fraction && !polygon.findEdgeSepAxis(point, bbox, polygon.verts[i], polygon.verts[k], axis, dir, a))
                {
                    tinfo.fraction = frac;
                    tinfo.norm = -dir;
                    tinfo.dend = min - d2;
                }
            }

            d1 = bdist - sz;
            d2 = edist - sz;

            if(d1 >= max-deps && d2 >= max-deps) return false;

            if(d1 >= max-deps && d2 < max-deps && !testOnly)
            {
                float frac = (d1 - max)/(d1 - d2);
                vec3 point = begin + (end - begin)*frac;

                if(frac < tinfo.fraction && !polygon.findEdgeSepAxis(point, bbox, polygon.verts[i], polygon.verts[k], axis, dir, a))
                {
                    tinfo.fraction = frac;
                    tinfo.norm = dir;
                    tinfo.dend = d2 - max;
                }
            }
        }

        if(!testOnly)
        {
            vec3 dir = edge^movedir;
            float n = dir.normalize();

            if(n < 0.001) continue;
            //if(fabs(dir*polygon.norm) < deps) continue;

            float sz = bbox.x*fabs(dir*axis[0]) +
                    bbox.y*fabs(dir*axis[1]) +
                    bbox.z*fabs(dir*axis[2]);

            float dist = end*dir;

            float max;
            float min;

            polygon.supportFunction(dir, min, max);

            if(dist + sz <= min+deps) return false;
            if(dist - sz >= max-deps) return false;
        }
    }

    return true;
}

bool PolygonalCollisionShape::traceBBox(TraceInfo& tinfo, const vec3& bbox, const vec3 begin, const vec3 end) const
{
    bool result = false;
    int poly;

    vec3 rbegin = (begin - m_pos)*m_orientation;
    vec3 rend = (end - m_pos)*m_orientation;
    mat3 axis = m_orientation.transpose();

    for(int p = 0; p < m_pnum; p++)
    {
        TraceInfo plgTraceInfo;
        plgTraceInfo.fraction = 1.0;

        if(tracePolygon(plgTraceInfo, m_polygons[p], bbox, axis, rbegin, rend))
        {
            if(plgTraceInfo.fraction < tinfo.fraction)
            {
                result = true;
                tinfo = plgTraceInfo;
            }
        }
    }

    if(result) tinfo.norm = m_orientation*tinfo.norm;

    return result;
}

bool PolygonalCollisionShape::traceRay(const vec3 & origin, const vec3 & ray, TraceRayInfo& tinfo) const
{
    bool result = false;

    vec3 local_origin = (origin - m_pos)*m_orientation;
    vec3 local_ray = ray*m_orientation;

    tinfo.dist = std::numeric_limits<float>::infinity();
    size_t pid = -1;

    for(size_t p = 0; p < m_pnum; p++)
    {
        float plgdist;

        if(tracePolygon(m_polygons[p], local_origin, local_ray, plgdist))
        {
            if (plgdist < tinfo.dist)
            {
                tinfo.dist = plgdist;
                pid = p;
            }
            
            result = true;
        }
    }

    if (result)
    {
        tinfo.norm = m_orientation * m_polygons[pid].plane.xyz;
        tinfo.material = m_polygons[pid].material;
    }

    return result;
}

bool PolygonalCollisionShape::testPolyHeight(const CollisionPolygon& poly,
                                             const vec3& pos, 
                                             const vec3& bbox, 
                                             float& vdist)
{
    static constexpr vec3 axis[3] = { {1.0f, 0.0f, 0.0f},
                                      {0.0f, 1.0f, 0.0f},
                                      {0.0f, 0.0f, 1.0f} };

    if (poly.plane.y < 0.5) return false;

    vec3 boxpos[4] = { {pos.x - bbox.x, 0, pos.z - bbox.z},
                       {pos.x + bbox.x, 0, pos.z - bbox.z},
                       {pos.x - bbox.x, 0, pos.z + bbox.z},
                       {pos.x + bbox.x, 0, pos.z + bbox.z} };

    bool binside[4] = { true, true, true, true };

    vdist = -std::numeric_limits<float>::infinity();

    //Test box faces
    for (int i = 0; i < 3; i++)
    {
        if (i == 1) continue;

        const vec3& dir = axis[i];

        if (fabs(1 - fabs(dir * poly.plane.xyz)) < math::eps) continue;

        float max;
        float min;

        poly.supportFunction(dir, min, max);

        float dist = pos * dir;

        if (dist + bbox[i] < min) return false;
        if (dist - bbox[i] > max) return false;
    }

    const std::vector<vec3>& verts = poly.verts;

    // heighst vert
    int vmax = 0;

    for (int i = 1; i < verts.size(); i++)
    {
        if (verts[i].y > verts[vmax].y) vmax = i;
    }

    if ((verts[vmax].x >= pos.x - bbox.x && verts[vmax].x <= pos.x + bbox.x) &&
        (verts[vmax].z >= pos.z - bbox.z && verts[vmax].z <= pos.z + bbox.z))
    {
        vdist = verts[vmax].y;
        return true;
    }

    const vec3& updir = axis[1];

    //Test polygon edges
    for (int i = 0; i < verts.size(); i++)
    {
        int k = (i == (verts.size() - 1)) ? k = 0 : k = i + 1;

        vec3 edge = verts[k] - verts[i];
        float elength = edge.normalize();

        vec3 sdir = edge ^ updir;

        float d = -verts[i] * sdir;

        float r = bbox.x * fabs(sdir.x) + bbox.y * fabs(sdir.y) + bbox.z * fabs(sdir.z);
        float dist = sdir * pos + d;

        for (int l = 0; l < 4; l++)
        {
            float dist = sdir * boxpos[l] + d;
            if (dist > 0) binside[l] = false;
        }

        if (dist > r) return false;

        if (dist < -r) continue;

        bool etouch = false;

        const vec3& pnorm = poly.plane.xyz;

        vec3 tang = sdir - pnorm * (sdir * pnorm);
        vec3 xtang = vec3(1.0f, 0.0f, 0.0f) - pnorm * pnorm.x;
        vec3 ztang = vec3(0.0f, 0.0f, 1.0f) - pnorm * pnorm.z;

        if (fabs(edge.x) > math::eps)
        {
            float x1 = pos.x + bbox.x - verts[i].x;
            float x2 = pos.x - bbox.x - verts[i].x;

            float y1 = verts[i].x < verts[k].x ? verts[i].y : verts[k].y;
            float y2 = verts[i].x < verts[k].x ? verts[k].y : verts[i].y;

            float x = y1 < y2 ? x1 : x2;

            float len = x / edge.x;
            float z = verts[i].z + len * edge.z;

            if ((len > 0 && len < elength) &&
                (z >= pos.z - bbox.z && z <= pos.z + bbox.z))
            {
                float height = verts[i].y + len * edge.y;
                if (height > vdist) vdist = height;
            }
        }

        if (fabs(edge.z) > math::eps)
        {
            float z1 = pos.z + bbox.z - verts[i].z;
            float z2 = pos.z - bbox.z - verts[i].z;

            float y1 = verts[i].z < verts[k].z ? verts[i].y : verts[k].y;
            float y2 = verts[i].z < verts[k].z ? verts[k].y : verts[i].y;

            float z = y1 < y2 ? z1 : z2;

            float len = z / edge.z;
            float x = verts[i].x + len * edge.x;

            if ((len > 0 && len < elength) &&
                (x >= pos.x - bbox.x && x <= pos.x + bbox.x))
            {
                float height = verts[i].y + len * edge.y;
                if (height > vdist) vdist = height;
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        if (!binside[i]) continue;

        float dist = poly.plane.xyz * boxpos[i] + poly.plane.w;
        float height = -dist / poly.plane.y;

        if (height > vdist) vdist = height;
    }

    return true;
}

bool PolygonalCollisionShape::testHeight(const vec3& pos, const vec3& bbox, float& height, float& tilt) const
{
    bool intersect = false;

    vec3 relpos = pos - m_pos;

    float vdist = 0;

    for(int p = 0; p < m_pnum; p++)
    {
        const CollisionPolygon& poly = m_polygons[p];

        float dist;

        if (testPolyHeight(m_polygons[p], relpos, bbox, dist))
        {
            if (fabs(relpos.y - dist) < bbox.y * 4)
            {
                if (!intersect || dist > vdist)
                {
                    intersect = true;
                    vdist = dist;
                    height = vdist + m_pos.y;
                    tilt = m_polygons[p].plane.y;
                }
            }
        }
    }

    return intersect;
}

} //namespace Collision