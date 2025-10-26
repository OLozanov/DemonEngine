#include "Trigger.h"

namespace GameLogic
{

Trigger::Trigger(float size, const vec3& pos)
: m_pos(pos)
, m_bbox(size, size, size)
, m_triggered(false)
{
}

Trigger::Trigger(const vec3& bbox, const vec3& pos)
: m_pos(pos)
, m_bbox(bbox)
, m_triggered(false)
{
}

Trigger::Trigger(const BBox& bbox, const vec3& pos)
: m_triggered(false)
{
    vec3 mid = (bbox.max + bbox.min) * 0.5;
    m_bbox = bbox.max - mid + vec3{ 1.0, 0.0, 1.0 };
    m_pos = pos + mid;
}

void Trigger::test(const vec3& pos, const vec3& bbox)
{
    if (AABBTest(m_pos, m_bbox, pos, bbox))
    {
        if (!m_triggered) OnTrigger();

        m_triggered = true;
        return;
    }

    if (m_triggered)
    {
        m_triggered = false;
        OnRelease();
    }
}

} // namespace gamelogic