#pragma once

#include "math/math3d.h"
#include "Utils/LinkedList.h"
#include "Render/DisplayData.h"

#include "Utils/MutualReference.h"

#include <vector>

namespace Render
{

class DisplayObject : public ListNode<DisplayObject>
{
protected:
    BBox m_bbox;
    mat4 m_mat;

    bool m_dynamic = false;

    std::vector<DisplayBlock> m_displayData;
    std::vector<InstanceData> m_instanceData;

private:
    OwnerRefList<DisplayObject> m_refs;
    uint64_t m_frame;

public:
    DisplayObject() {}
    virtual ~DisplayObject() {}

    const std::vector<DisplayBlock>& displayData() const { return m_displayData; }
    const std::vector<InstanceData>& instanceData() const { return m_instanceData; }

    bool isDynamic() const { return m_dynamic; }

    const BBox& bbox() const { return m_bbox; }
    const mat4& mat() const { return m_mat; }
    void setMat(const mat4& mat) { m_mat = mat; }

    void worldBBox(vec3& bbpos, vec3& box)
    {
        vec3 opos = m_mat[3];

        mat3 rot = m_mat;

        vec3 mid = (m_bbox.min + m_bbox.max) * 0.5;
        bbpos = opos + rot * mid;

        box = m_bbox.max - mid;
    }

    uint64_t frame() { return m_frame; }
    void setFrame(uint64_t frame) { m_frame = frame; }

    void clearReferences() { m_refs.clear(); }
    void addReference(RefList<DisplayObject>& list) { m_refs.append(list, this); }

    void hideSubmesh(int n, bool hide = true) { m_displayData[n].hide = hide; }
};

} //namespace Render