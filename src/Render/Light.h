#pragma once

#include "math/math3d.h"
#include "Utils/LinkedList.h"
#include "Utils/MutualReference.h"

#include "Render/Scene.h"

namespace Render
{

enum class LightType
{
    Omni,
    Spot
};

enum class LightShadow : uint8_t
{
    None = 0,
    Static = 1,
    Dynamic = 2,
    Moving = 3
};

struct OmniLightData
{
    vec3 pos;
    float radius;
    vec3 flux;
    float falloff;
    uint32_t shadowIdx;
};

struct SpotLightData
{
    vec3 pos;
    float radius;
    vec3 flux;
    float falloff;
    vec3 dir;
    float outerAngle;
    float innerAngle;
    uint32_t shadowIdx;
    float padding[2];
    mat4 projMat;
};

class Light : public ListNode<Light>
{
public:
    static constexpr size_t InvalidShadowIndex = -1;

private:
    LightType m_type;
    LightShadow m_shadowType;

    uint32_t m_id = 0;

    Scene m_scene;

    OwnerRefList<Light> m_refs;
    uint64_t m_frame;

    size_t m_shadowIdx = InvalidShadowIndex;
    size_t m_cacheIdx = InvalidShadowIndex;

    bool m_staticUpdate;

    union
    {
        OmniLightData m_omniData;
        SpotLightData m_spotData;
    };

    static constexpr size_t ShadowMapNum = 8;

public:

    Light();
    Light(const vec3& pos, const vec3& flux, float radius, float falloff, LightShadow shadow = LightShadow::None);
    Light(const vec3& pos, const vec3& dir, const vec3& flux, 
          float radius, float falloff,
          float outerAngle, float innerAngle, LightShadow shadow = LightShadow::None);

    LightType type() { return m_type; }

    uint32_t id() const { return m_id; }
    void setId(uint32_t id) { m_id = id; }

    uint64_t frame() { return m_frame; }
    void setFrame(uint64_t frame) { m_frame = frame; }

    void clearReferences() { m_refs.clear(); }
    void addReference(RefList<Light>& list) { m_refs.append(list, this); }

    size_t shadowIndex() const { return m_shadowIdx; }
    size_t cacheIndex() const { return m_cacheIdx; }

    void setShadowIndex(size_t idx) { m_shadowIdx = idx; }
    void setCacheIndex(size_t idx) { m_cacheIdx = idx; }

    void updateData();

    bool needStaticUpdate() { return m_staticUpdate; }
    void setStaticUpdateFlag(bool set) { m_staticUpdate = set; }

    const void* data() const { return &m_omniData.pos; }

    const vec3& pos() const { return m_omniData.pos; }
    const vec3& flux() const { return m_omniData.flux; }
    float radius() const { return m_omniData.radius; }
    float falloff() const { return m_omniData.falloff; }

    const mat4& shadowMat() const { return m_spotData.projMat; }

    LightShadow shadowType() { return m_shadowType; }
    bool castShadow() const { return m_shadowType != LightShadow::None; }

    float affectedDistance() { return m_omniData.radius + m_omniData.falloff; }

    Scene& scene() { return m_scene; }
    void calculateVisibility(uint64_t frame);
    void updateVisibility(uint64_t frame);
};

} //namespace Render