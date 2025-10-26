#pragma once

#include "math/math3d.h"
#include <vector>

namespace Collision
{

using MaterialId = uint32_t;

struct ContactInfo
{
    vec3 point;
    vec3 norm;
    float dist;
};

struct TraceInfo
{
    float fraction;
    vec3 norm;
    float dist;			//penetration distance
    float dend;
};

struct TraceRayInfo
{
    float dist;
    vec3 norm;
    uint32_t material;
    uint32_t layers;
    void* object;
};

struct CollisionPolygon
{
    std::vector<vec3> verts;
    vec4 plane;
    
    MaterialId material;

    bool isPointInside(const vec3& pt) const;
    bool isInsidePerimeter(const vec3& pt) const;
    bool findBoxSepAxis(const vec3& pos, const vec3& bbox) const;
    bool findBoxSepAxis(const vec3& pos, const vec3& bbox, const mat3& axis) const;
    bool findEdgeSepAxis(const vec3& pos, const vec3& bbox, const vec3& a, const vec3& b, const mat3& axis, const vec3& touchnorm, int aind) const;
    void supportFunction(const vec3& axis, float& min, float& max) const;
};

void supportFunction(const vec3& axis, const vec3* verts, unsigned long vnum, float& min, float& max);

float intervalIntersection(float a, float b, float c, float d);
bool lineIntersection(const vec3& norm, float d, const vec3& a, const vec3& b, vec3& pt);
bool isInsideBbox(const vec3& pt, const vec3& pos, const vec3& bbox);

bool rayClosestPoints(const vec3& origin1, const vec3& origin2,
                      const vec3& ray1, const vec3& ray2,
                      float& s, float& t);

void rayOverlap(vec3 origin1, vec3 origin2, vec3 ray1, vec3 ray2, float& s, float& t);

} //namespace Collision