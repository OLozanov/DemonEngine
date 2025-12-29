#pragma once

#include "math/math3d.h"
#include "math/BBox.h"

namespace Render
{

class Frustum
{
public:
    Frustum() = default;

    void update(const mat4& mat);
    bool test(const vec3& pos, const vec3& bbox, const mat3& axis) const;
    bool test(const vec3& pos, const vec3& bbox) const;
    bool test(const BBox& bbox) const;

    const vec4* planes() { return m_planes; }

private:
    vec4 m_planes[6];
};

} // namespace Render
