#pragma once

#include "Utils/LinkedList.h"
#include "Utils/MutualReference.h"
#include "math/math3d.h"
#include <numeric>

namespace Render
{

struct FogData
{
    vec3 pos;
    float pad;
    vec3 size;
    float density;
    vec3 color;
    uint32_t flags;

    FogData(const vec3& pos,
            const vec3& size,
            const vec3& color,
            float density,
            uint8_t type,
            uint8_t lighting)
    : pos(pos)
    , size(size)
    , color(color)
    , density(density)
    , flags(lighting << 1 | type)
    {
    }
};

class FogVolume : public ListNode<FogVolume>
{
private:
    FogData m_data;

    OwnerRefSet<FogVolume> m_refs;
    uint64_t m_frame;
public:
    FogVolume(const vec3& pos,
              const vec3& size,
              const vec3& color,
              float density,
              uint8_t type,
              bool lighting)
    : m_data(pos, size, { powf(color.x, 2.2), powf(color.y, 2.2), powf(color.z, 2.2) }, density * 0.1, type, lighting)
    {
    }

    const vec3& pos() const { return m_data.pos; }
    const vec3& size() const { return m_data.size; }
    const vec3& color() const { return m_data.color; }

    const FogData& data() const { return m_data; }

    uint64_t frame() { return m_frame; }
    void setFrame(uint64_t frame) { m_frame = frame; }

    void clearReferences() { m_refs.clear(); }
    void addReference(RefList<FogVolume>& list) { m_refs.append(list, this); }
};

} //namespace Render