#include "GameObject.h"
#include "Utils/FileStream.h"

#include <algorithm>

namespace GameLogic
{

LinkedList<Hitable> Hitable::ObjectList;
LinkedList<Pickable> Pickable::ObjectList;

Hitable::Hitable()
{
    ObjectList.append(this);
}

Hitable::~Hitable()
{
    if (!ObjectList.empty()) ObjectList.remove(this);
}

void Hitable::ClearObjects()
{
    ObjectList.clear();
}

// Workaround for Player in fact
void Hitable::AddObject(Hitable* object)
{
    ObjectList.append(object);
}

void Hitable::Impact(const vec3& center, uint32_t power)
{
    for (Hitable* object : ObjectList)
    {
        vec3 objCenter = object->center();
        vec3 dir = center - objCenter;
        float dist = dir.normalize();

        float effectiveDist = power * DistFactor;

        if (dist < effectiveDist)
        {
            float attenuation = std::max(0.0f, effectiveDist - dist) / effectiveDist;
            object->hit(objCenter, -dir, power * PowerFactor * attenuation);
        }
    }
}

Pickable::Pickable(float size, const vec3& pos)
: m_pos(pos)
, m_bbox(size, size, size)
, m_triggered(false)
{
    ObjectList.append(this);
}

Pickable::Pickable(const vec3& bbox, const vec3& pos)
: m_pos(pos)
, m_bbox(bbox)
, m_triggered(false)
{
    ObjectList.append(this);
}

Pickable::~Pickable()
{
    if (!ObjectList.empty()) ObjectList.remove(this);
}

void Pickable::test(const vec3& pos, const vec3& bbox)
{
    if (AABBTest(m_pos, m_bbox, pos, bbox))
    {
        if (!m_triggered) OnPickup();

        m_triggered = true;
        return;
    }

    if (m_triggered) m_triggered = false;
}

void Pickable::ClearObjects()
{
    ObjectList.clear();
}

void Pickable::TestObjects(const vec3& pos, const vec3& bbox)
{
    for (Pickable* object : ObjectList) object->test(pos, bbox);
}

void GameObject::save(FileStream& file)
{
    file << m_id;
}

void GameObject::restore(FileStream& file)
{
    file >> m_id;
}

} // namespace gamelogic