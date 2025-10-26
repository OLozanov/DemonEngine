#pragma once

class vec3;

struct BBox
{
    vec3 min;
    vec3 max;
};

bool AABBDynTest(const vec3& mbox, const vec3& sbox, const vec3& begin, const vec3& end, const vec3& pos); //Moving AABB vs static AABB intersection test
bool AABBTest(const vec3& pos1, const vec3& bbox1, const vec3& pos2, const vec3& bbox2);

bool BBoxIntersect(const vec3& pos1, const BBox& bbox1, const vec3& pos2, const BBox& bbox2);

bool BBoxRayTest(const vec3& origin, const vec3& ray, const vec3& pos, const vec3& bbox, const mat3& axis, float& dist);

BBox RecalcBBox(const BBox& bbox, const mat3& mat);