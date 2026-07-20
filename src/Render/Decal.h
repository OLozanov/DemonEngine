#pragma once
#include "Utils/LinkedList.h"
#include "Utils/MutualReference.h"
#include "Resources/Resources.h"
#include "math/math3d.h"

namespace Render
{

struct DecalData
{
    vec4 x;
    vec4 y;
    vec4 z;
    vec4 pos;
    vec3 size;
    MaterialId matid;
};

class Decal : public ListNode<Decal>
{
private:
    DecalData m_data;
    mat3 m_orientation;

    MaterialPtr m_material;

    OwnerRefSet<Decal> m_refs;
    uint64_t m_frame;
public:

    Decal(const vec3& pos,
          const mat3& orientation,
          const vec3& size,
          Material* material)
          : m_material(material)
          , m_orientation(orientation)
          , m_data(orientation[0], orientation[1], orientation[2], pos, 
            size, 
            material->id)
    {
    }

    const vec3& pos() const { return m_data.pos.xyz; }
    const mat3& orientation() const { return m_orientation; }
    const vec3& size() const { return m_data.size; }

    const DecalData& data() const { return m_data; }

    uint64_t frame() { return m_frame; }
    void setFrame(uint64_t frame) { m_frame = frame; }

    void clearReferences() { m_refs.clear(); }
    void addReference(RefList<Decal>& list) { m_refs.append(list, this); }
};

} //namespace Render
