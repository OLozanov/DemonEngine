#pragma once

#include "math/math3d.h"
#include <vector>

#include "D3D/ConstantBuffer.h"

namespace Render
{

// Global directional light like sun and so on.
class DirectionalLight
{
    static constexpr int ShadowResolution = 2048;
    static constexpr int LodNum = 3;

    vec3 m_direction;
    vec3 m_color;

    vec3 m_xAxis;
    vec3 m_yAxis;

    mat4 m_projMat[LodNum];

    mat4 m_baseViewMat;
    mat4 m_viewMat;

    vec4 m_data[2];

    ConstantBuffer<mat4> m_shadowMat;

public:

    DirectionalLight();

    void setup(const vec3& direction, const vec3& color);

    const void* data() const { return &m_data; }

    const vec3& direction() { return m_direction; }
    const vec3& color() { return m_color; }
    const mat4& viewMat() { return m_viewMat; }
    const ConstantBuffer<mat4>& shadowMatrices() { return m_shadowMat; }

    void setPosition(const vec3& pos);
};

} //namespace render