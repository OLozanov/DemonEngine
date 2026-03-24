#pragma once

#include "math/math3d.h"
#include "math/BBox.h"

#include <vector>

namespace Render
{

class Frustum
{
public:
    Frustum() = default;

    void update(const vec3& pos, const vec4& plane, const std::vector<vec3>& portal);
    void update(const vec3& pos, const vec3& dir, const vec3* verts, size_t num);
    //void update(const Camera& camera, float fovx, float fovy);
    void update(const mat4& mat);
    bool test(const vec3& pos, const vec3& bbox, const mat3& axis) const;
    bool test(const vec3& pos, const vec3& bbox) const;
    bool test(const BBox& bbox) const;
    bool test(const BBox& bbox, const mat4& mat) const;

    const size_t size() const { return m_planes.size(); }
    const std::vector<vec4>& planes() const { return m_planes; }
    const vec4& plane(size_t n) const { return m_planes[n]; }

private:
    std::vector<vec4> m_planes;
};

} // namespace Render
