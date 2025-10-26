#include "DirectionalLight.h"

namespace Render
{

DirectionalLight::DirectionalLight()
: m_shadowMat(LodNum)
{
    m_projMat[0] = mat4::OrthoProjection(-25, 25, -25, 25, -500, 500);
    m_projMat[1] = mat4::OrthoProjection(-75, 75, -75, 75, -500, 500);
    m_projMat[2] = mat4::OrthoProjection(-225, 225, -225, 225, -500, 500);
}

void DirectionalLight::setup(const vec3& direction, const vec3& color)
{
    static const vec3 worldX = { 1, 0, 0 };
    static const vec3 worldZ = { 0, 1, 0 };

    m_direction = direction;
    m_color = color;

    m_direction.normalize();

    m_yAxis = worldZ - m_direction * (m_direction * worldZ);

    if (m_yAxis.length() < math::eps)
    {
        m_xAxis = worldX - m_direction * (m_direction * worldX);
        m_yAxis = m_direction ^ m_xAxis;
    
    } else m_xAxis = m_yAxis ^ m_direction;

    m_xAxis.normalize();
    m_yAxis.normalize();

    m_baseViewMat = { {m_xAxis.x, m_yAxis.x, m_direction.x, 0},
                        {m_xAxis.y, m_yAxis.y, m_direction.y, 0},
                        {m_xAxis.z, m_yAxis.z, m_direction.z, 0},
                        {0, 0, 0, 1} };

    for (int i = 0; i < LodNum; i++) m_shadowMat[i] = m_projMat[i] * m_baseViewMat;

    m_data[0] = -m_direction;
    m_data[1] = m_color;
}

void DirectionalLight::setPosition(const vec3& pos)
{
    m_viewMat = m_baseViewMat * mat4::Translate(-pos);

    for (int i = 0; i < LodNum; i++)
    {
        m_shadowMat[i] = m_projMat[i] * m_viewMat;

        constexpr int shadFactor = ShadowResolution / 2;

        m_shadowMat[i][3][0] = ((int)(m_shadowMat[i][3][0] * shadFactor)) / (float)shadFactor;
        m_shadowMat[i][3][1] = ((int)(m_shadowMat[i][3][1] * shadFactor)) / (float)shadFactor;
    }
}

} //namespace render