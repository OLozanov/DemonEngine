#include "Geometry.h"

float RayIntersect(const vec3& origin1, const vec3& origin2, const vec3& ray1, const vec3& ray2, float& s, float& t)
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

    vec3 pt1 = origin1 + ray1 * s;
    vec3 pt2 = origin2 + ray2 * t;

    vec3 diff = pt2 - pt1;

    return diff.length();
}

inline PointType ClassifyPoint(const vec4& plane, const vec3& vert)
{
    float dist = vert * plane.xyz + plane.w;

    if (fabs(dist) < math::eps) return PointType::Plane;

    if (dist > 0) return PointType::Front;
    else return PointType::Back;
}

PolyType ClassifyPolygon(const vec4& plane, const VertexList& verts)
{
    int back = 0;
    int front = 0;
    int coplanar = 0;

    for (const Vertex& vert : verts)
    {
        float dist = vert.position * plane.xyz + plane.w;

        if (fabs(dist) < math::eps) coplanar++;
        else
        {
            if (dist > 0) front++;
            if (dist < 0) back++;
        }
    }

    if (back && !front) return PolyType::Back;
    if (front && !back) return PolyType::Front;
    if (back && front) return PolyType::Split;

    return PolyType::Plane;
}

PolyType ClassifyPolygon(const vec4& plane, const std::vector<vec3>& verts)
{
    int back = 0;
    int front = 0;
    int coplanar = 0;

    for (const vec3& vert : verts)
    {
        float dist = vert * plane.xyz + plane.w;

        if (fabs(dist) < math::eps) coplanar++;
        else
        {
            if (dist > 0) front++;
            if (dist < 0) back++;
        }
    }

    if (back && !front) return PolyType::Back;
    if (front && !back) return PolyType::Front;
    if (back && front) return PolyType::Split;

    return PolyType::Plane;
}

Vertex SplitEdge(const vec4& plane, const Vertex& vert1, const Vertex& vert2)
{
    Vertex nvert;

    float dist = vert1.position * plane.xyz + plane.w;

    vec3 edge = vert2.position - vert1.position;
    float elen = edge.normalize();
    float Cos = -(edge * plane.xyz);

    float len = dist / Cos;

    nvert.position = vert1.position + edge * len;

    float rat = len / elen;

    //Texture coord
    vec2 tedge = vert2.tcoord - vert1.tcoord;

    len = tedge.normalize();
    len *= rat;

    nvert.tcoord = vert1.tcoord + tedge * len;

    //tangent space
    vec3 dnorm = vert2.normal - vert1.normal;
    vec3 dtang = vert2.tangent - vert1.tangent;
    vec3 dbinorm = vert2.binormal - vert2.binormal;

    len = dnorm.normalize();

    dnorm *= len * rat;
    nvert.normal = vert1.normal + dnorm;

    len = dtang.normalize();

    dtang *= len * rat;
    nvert.tangent = vert1.tangent + dtang;

    len = dbinorm.normalize();

    dbinorm *= len * rat;
    nvert.binormal = vert1.binormal + dbinorm;

    nvert.normal.normalize();
    nvert.tangent.normalize();
    nvert.binormal.normalize();

    return nvert;
}

vec3 SplitEdge(const vec4& plane, const vec3& vert1, const vec3& vert2)
{
    float dist = vert1 * plane.xyz + plane.w;

    vec3 edge = vert2 - vert1;
    float elen = edge.normalize();
    float Cos = -(edge * plane.xyz);

    float len = dist / Cos;

    return vert1 + edge * len;

    float rat = len / elen;
}

void SplitPoly(const vec4& plane, const VertexList& verts, VertexList& left, VertexList& right)
{
    for (int i = 0; i < verts.size(); i++)
    {
        const Vertex& verta = verts[i];
        const Vertex& vertb = i < verts.size() - 1 ? verts[i + 1] : verts[0];

        PointType typea = ClassifyPoint(plane, verta.position);
        PointType typeb = ClassifyPoint(plane, vertb.position);

        if (typea == PointType::Plane)
        {
            left.push_back(verta);
            right.push_back(verta);

            continue;
        }

        if (typea == PointType::Back)
        {
            left.push_back(verta);

            //Edge intersection
            if (typeb == PointType::Front)
            {
                Vertex vert = SplitEdge(plane, verta, vertb);

                left.push_back(vert);
                right.push_back(vert);
            }
        }
        else
        {
            right.push_back(verta);

            //Edge intersection
            if (typeb == PointType::Back)
            {
                Vertex vert = SplitEdge(plane, vertb, verta);

                left.push_back(vert);
                right.push_back(vert);
            }
        }
    }
}

void SplitPoly(const vec4& plane, const std::vector<vec3>& verts, std::vector<vec3>& left, std::vector<vec3>& right)
{
    for (int i = 0; i < verts.size(); i++)
    {
        const vec3& verta = verts[i];
        const vec3& vertb = i < verts.size() - 1 ? verts[i + 1] : verts[0];

        PointType typea = ClassifyPoint(plane, verta);
        PointType typeb = ClassifyPoint(plane, vertb);

        if (typea == PointType::Plane)
        {
            left.push_back(verta);
            right.push_back(verta);

            continue;
        }

        if (typea == PointType::Back)
        {
            left.push_back(verta);

            //Edge intersection
            if (typeb == PointType::Front)
            {
                vec3 vert = SplitEdge(plane, verta, vertb);

                left.push_back(vert);
                right.push_back(vert);
            }
        }
        else
        {
            right.push_back(verta);

            //Edge intersection
            if (typeb == PointType::Back)
            {
                vec3 vert = SplitEdge(plane, vertb, verta);

                left.push_back(vert);
                right.push_back(vert);
            }
        }
    }
}

// Moller-Trambore algorithm
bool TriangleIntersect(const vec3& v0, const vec3& v1, const vec3& v2,
                       const vec3& origin, const vec3& ray, float& dist)
{
    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;

    vec3 h = ray ^ edge2;
    float a = edge1 * h;

    if (fabs(a) < math::eps) return false; // ray is parallel to triangle

    float f = 1.0 / a;
    vec3 s = origin - v0;
    float u = f * (s * h);

    if (u < 0.0f || u > 1.0f) return false;

    vec3 q = s ^ edge1;
    float v = f * (ray * q);

    if (v < 0.0f || u + v > 1.0f) return false;

    float t = f * (edge2 * q);

    if (t > math::eps) // ray intersection
    {
        dist = t;
        return true;
    }

    return false;
}

bool PolyIntersect(const EditPolygon& poly, const vec3& origin, const vec3& ray, float& dst, bool frontCull)
{
    float dist = origin * poly.plane.xyz + poly.plane.w;
    float Cos = ray * poly.plane.xyz;

    if (!frontCull && dist < 0) return false;
    if (frontCull && dist > 0) return false;
    if (fabs(Cos) < math::eps) return false;

    dst = -dist / Cos;
    vec3 pt = origin + ray * dst;

    if (pt * ray < origin * ray) return false; //Backward polygons

    for (int i = 0; i < poly.vertices.size(); i++)
    {
        int k = (i == poly.vertices.size() - 1) ? 0 : i + 1;

        const vec3& pta = poly.vertices[i].position;
        const vec3& ptb = poly.vertices[k].position;

        vec3 edge = ptb - pta;

        vec3 enorm = poly.plane.xyz ^ edge;
        enorm.normalize();

        float dist = pt * enorm - pta * enorm;

        if (dist < 0) return false;
    }

    return true;
}

void TriangleTangentSpace(const vec3& a,
                          const vec3& b,
                          const vec3& c,
                          const vec2& ta,
                          const vec2& tb,
                          const vec2& tc,
                          vec3& s,
                          vec3& t)
{
    vec3 l1 = b - a;
    vec3 l2 = c - a;

    vec3 norm = l1 ^ l2;
    norm.normalize();

    vec2 tl1 = tb - ta;
    vec2 tl2 = tc - ta;

    float det = tl1.x * tl2.y - tl1.y * tl2.x;

    if (fabs(det) < math::eps)
    {
        s = l1;
        t = l2;
    }
    else
    {
        float sx = (tl2.y * l1.x - tl1.y * l2.x) / det;
        float sy = (tl2.y * l1.y - tl1.y * l2.y) / det;
        float sz = (tl2.y * l1.z - tl1.y * l2.z) / det;

        float tx = (-tl2.x * l1.x + tl1.x * l2.x) / det;
        float ty = (-tl2.x * l1.y + tl1.x * l2.y) / det;
        float tz = (-tl2.x * l1.z + tl1.x * l2.z) / det;

        s = { sx, sy, sz };
        t = { tx, ty, tz };
    }

    s.normalize();
    t.normalize();

    //invert tangents if need
    //vec3 cp = s ^ t;
    //if (cp * norm < 0) s = -s;
}