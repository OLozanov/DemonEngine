#pragma once

#include "Render/Camera.h"
#include "math/math3d.h"
#include "math/BBox.h"

namespace Render
{

class Frustum
{
public:
    Frustum() = default;

    void update(const vec3& pos, const vec3& dir, const vec3* verts, size_t num);
    void update(const Camera& camera, float fovx, float fovy);
    void update(const mat4& mat);
    bool test(const vec3& pos, const vec3& bbox, const mat3& axis) const;
    bool test(const vec3& pos, const vec3& bbox) const;
    bool test(const BBox& bbox) const;
    bool test(const BBox& bbox, const mat4& mat) const;

    const vec4* planes() const { return m_planes; }
    const vec4& plane(size_t n) const { return m_planes[n]; }

private:
    vec4 m_planes[6];
};

} // namespace Render
