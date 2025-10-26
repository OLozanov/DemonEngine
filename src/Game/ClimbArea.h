#pragma once

#include "math/math3d.h"

namespace GameLogic
{

// Ladders and similar stuff
class ClimbArea
{
public:
    ClimbArea(const vec3& pos, const vec2& norm, const vec3& size)
    : m_pos(pos)
    , m_norm(norm)
    , m_zd(m_pos.x * m_norm.x + m_pos.z * m_norm.y)
    , m_xd(m_pos.z * m_norm.x - m_pos.x * m_norm.y)
    , m_size(size)
    {
    }

    const vec3& pos() const { return m_pos; }
    const vec2& norm() const { return m_norm; }
    const vec3& size() const { return m_size; }

    bool test(const vec3& pos, const vec3& bbox) const
    {
        if (pos.y > m_pos.y + m_size.y) return false;
        if (pos.y < m_pos.y - m_size.y) return false;

        float zsize = m_size.z;

        if (pos.y > m_pos.y + m_size.y - bbox.y - 0.5) zsize += 0.2f;

        float r = fabs(bbox.x * m_norm.x) + fabs(bbox.z * m_norm.y);
        float dist = pos.x * m_norm.x + pos.z * m_norm.y - m_zd - r;

        if (fabs(dist) > zsize) return false;

        r = fabs(bbox.z * m_norm.x) + fabs(bbox.x * m_norm.y);
        dist = pos.z * m_norm.x - pos.x * m_norm.y - m_xd;

        if (fabs(dist) > m_size.x + r) return false;

        return true;
    }

private:
    vec3 m_pos;
    vec2 m_norm;
    vec3 m_size;

    float m_xd;
    float m_zd;
};

} // namespace gamelogic