#include "Surface.h"

void Surface::setFlag(unsigned int flg, bool active)
{
    if (active) m_flags |= flg;
    else m_flags &= ~flg;
}

bool Surface::getFlags(unsigned int flg) const
{
    return (m_flags & flg) != 0;
}

void Surface::setMapMode(TextureMapping mapMode)
{
    m_mapMode = mapMode;
    
    if (mapMode == TextureMapping::TSpace) updateTCoord();
}

TextureMapping Surface::getMapMode() const
{ 
    return m_mapMode; 
}

void Surface::setTSpace(const vec3& s, const vec3& t)
{
    m_s = s;
    m_t = t;
}

void Surface::pickMove(const vec3& newpt)
{
    m_pos = newpt + m_pickdiff;
}

void Surface::pickMove2d(float x, float y, int i, int k)
{
    m_pos[i] = x + m_pickdiff[i];
    m_pos[k] = y + m_pickdiff[k];
}

void Surface::pickRotateX(float Cos, float Sin, const vec3& center)
{
    float y = Cos * m_pickdiff.y + Sin * m_pickdiff.z;
    float z = -Sin * m_pickdiff.y + Cos * m_pickdiff.z;

    m_pos.y = center.y + y;
    m_pos.z = center.z + z;
}

void Surface::pickRotateY(float Cos, float Sin, const vec3& center)
{
    float x = Cos * m_pickdiff.x - Sin * m_pickdiff.z;
    float z = Sin * m_pickdiff.x + Cos * m_pickdiff.z;

    m_pos.x = center.x + x;
    m_pos.z = center.z + z;
}

void Surface::pickRotateZ(float Cos, float Sin, const vec3& center)
{
    float x = Cos * m_pickdiff.x + Sin * m_pickdiff.y;
    float y = -Sin * m_pickdiff.x + Cos * m_pickdiff.y;

    m_pos.x = center.x + x;
    m_pos.y = center.y + y;
}

void Surface::pickScale(const vec3& center, const vec3& scale)
{
    vec3 scaledDiff = { m_pickdiff.x * scale.x, m_pickdiff.y * scale.y, m_pickdiff.z * scale.z };

    m_pos = center + scaledDiff;
    m_scale = scale;
}

void Surface::moveTextureS(float val)
{
    if (m_mapMode == TextureMapping::TSpace)
    {
        m_uv.x += val;
        updateTCoord();
    }
    else moveTexCoordS(val);
}

void Surface::moveTextureT(float val)
{
    if (m_mapMode == TextureMapping::TSpace)
    {
        m_uv.y += val;
        updateTCoord();
    }
    else moveTexCoordT(val);
}

void Surface::scaleTextureS(float val)
{
    if (m_mapMode == TextureMapping::TSpace)
    {
        m_s *= val;
        updateTCoord();
    }
    else scaleTexCoordS(val);
}

void Surface::scaleTextureT(float val)
{
    if (m_mapMode == TextureMapping::TSpace)
    {
        m_t *= val;
        updateTCoord();
    }
    else scaleTexCoordT(val);
}

void Surface::rotateTexture(float ang)
{
    if (m_mapMode == TextureMapping::TSpace)
    {
        float Cos = cos(ang / 180.0f * math::pi);
        float Sin = sin(ang / 180.0f * math::pi);

        vec3 XCos = m_s * Cos;
        vec3 YCos = m_t * Cos;
        vec3 XSin = m_s * Sin;
        vec3 YSin = m_t * (-Sin);

        m_s = XCos + YSin;
        m_t = XSin + YCos;

        updateTCoord();
    }
    else rotateTexCoord(ang);
}

void Surface::applyRotation()
{
    mat3 rotMat = mat3::Rotate(m_rot.x, m_rot.y, m_rot.z);

    m_s = rotMat * m_s;
    m_t = rotMat * m_t;

    applyTransform(rotMat);
    m_rot = { 0, 0, 0 };
}

void Surface::applyScale()
{
    m_s.x /= m_scale.x;
    m_s.y /= m_scale.y;
    m_s.z /= m_scale.z;

    m_t.x /= m_scale.x;
    m_t.y /= m_scale.y;
    m_t.z /= m_scale.z;

    applyScaleInternal();
    m_scale = { 1, 1, 1 };
}