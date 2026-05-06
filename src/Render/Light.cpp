#include "Light.h"
#include "Render/SceneManager.h"

namespace Render
{

Light::Light()
: m_type(LightType::Omni)
, m_omniData{ vec3(0), 5, vec3(1), 2 }
, m_shadowType(LightShadow::None)
, m_view(SceneManager::GetInstance().getWorld(), View::ViewLeafs | View::ViewObjects | View::ViewRestrictDist)
{
    m_view.setDistance(affectedDistance());
}

Light::Light(const vec3& pos, const vec3& color, float radius, float falloff, LightShadow shadow)
: m_type(LightType::Omni)
, m_shadowType(shadow)
, m_view(SceneManager::GetInstance().getWorld(), View::ViewLeafs | View::ViewObjects | View::ViewRestrictDist)
{
    m_omniData.pos = pos;
    m_omniData.flux = color;
    m_omniData.radius = radius;
    m_omniData.falloff = falloff;
    m_omniData.shadowIdx = -1;

    m_view.setDistance(affectedDistance());
}

Light::Light(const vec3& pos, const vec3& dir, const vec3& color,
             float radius, float falloff,
             float outerAngle, float innerAngle, LightShadow shadow)
: m_type(LightType::Spot)
, m_shadowType(shadow)
, m_view(SceneManager::GetInstance().getWorld(), View::ViewLeafs | View::ViewObjects | View::ViewRestrictDist)
{
    vec3 sidedir = vec3::Orthogonal(dir);
    float length = radius + falloff;

    m_spotData.pos = pos;
    m_spotData.dir = dir;
    m_spotData.width = tanf(outerAngle * 0.5f / 180.0f * math::pi) * length;
    m_spotData.sidedir = sidedir;
    m_spotData.flux = color;
    m_spotData.radius = radius;
    m_spotData.falloff = falloff;
    m_spotData.outerAngle = cosf(outerAngle * 0.5f / 180.0f * math::pi);
    m_spotData.innerAngle = cosf(innerAngle * 0.5f / 180.0f * math::pi);
    m_spotData.shadowIdx = -1;

    mat3 mat;
    mat[2] = dir;
    mat[0] = sidedir;
    mat[1] = mat[0] ^ mat[2];

    m_spotData.projMat = mat4::Projection(outerAngle, 1.0f, 0.1f, radius + falloff) * mat.transpose() * mat4::Translate(-pos);

    m_view.setDistance(affectedDistance());
}

void Light::updateData()
{
    if (m_shadowType == LightShadow::Dynamic)
    {
        if (m_type == LightType::Omni)
            m_omniData.shadowIdx = m_shadowIdx;
        else
            m_spotData.shadowIdx = m_shadowIdx;
    }
    else if (m_shadowType == LightShadow::Static)
    {
        if (m_type == LightType::Omni)
            m_omniData.shadowIdx = m_cacheIdx + ShadowMapNum;
        else
            m_spotData.shadowIdx = m_cacheIdx + ShadowMapNum;
    }
}

void Light::calculateVisibility(uint64_t frame)
{
    //uint8_t flags = Scene::vis_restrict_dist;
    //if (m_shadowType == LightShadow::Dynamic) flags |= Scene::vis_dynamic;

    m_view.setFlags(View::ViewRestrictDist | View::ViewLeafs | View::ViewStatic);

    if (m_type == LightType::Omni)
        m_view.update(m_omniData.pos, affectedDistance(), frame);
    else
        m_view.update(m_spotData.pos, m_spotData.projMat, frame);

    m_view.addLightRefs(this);
    m_view.setFlags(View::ViewRestrictDist | View::ViewDynamic);
}

void Light::updateVisibility(uint64_t frame)
{
    if (m_type == LightType::Omni)
        m_view.update(m_omniData.pos, affectedDistance(), frame);
    else
        m_view.update(m_spotData.pos, m_spotData.projMat, frame);
}

} //namespace render