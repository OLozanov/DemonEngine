#pragma once

#include "Render/World.h"
#include <vector>

namespace Render
{

class Light;
using LightList = std::vector<Light*>;
using FogVolumeList = std::vector<FogVolume*>;

class Scene
{
    World& m_world;

    DisplayBlock m_global;
    DisplayBlock m_skyGeometry;
    DisplayBlock m_emissiveGeometry;
    DisplayBlock m_transparentGeometry;

    DisplayList m_regularList;
    DisplayList m_layeredList;
    DisplayList m_emissiveList;
    DisplayList m_transparentList;
    DisplayList m_spriteList;
    DisplayList m_debugList;

    InstancedList m_instancedList;

    FogVolumeList m_fogVolumes;

    std::vector<Index> m_visLeaves;

    uint8_t m_flags;

    float m_distance;

    uint64_t m_frame;
    Index m_zone;

    vec4 m_screenPlane;

    bool m_skyVisible;

public:
    enum Flags
    {
        vis_leafs = 1,
        vis_objects = 2,
        vis_static = 4,
        vis_dynamic = 8,
        vis_instanced = 16,
        vis_restrict_dist = 32,
    };

public:
    Scene(World& world, uint8_t flags = vis_leafs | vis_static | vis_dynamic);

    void setFlags(uint8_t flags) { m_flags = flags; }
    void setDistance(float distance) { m_distance = distance; }

    void calculateVisibility(const vec3& pos, const vec4& screenPlane, const vec3* frustum, uint64_t frame);
    void calculateVisibility(const vec3& pos, uint64_t frame);

    void globalLightVisibility(const vec3& dir, uint64_t frame);

    const DisplayList& displayList() const { return m_regularList; }
    const DisplayList& displayListLayered() const { return m_layeredList; }
    const DisplayList& displayListEmissive() const { return m_emissiveList; }
    const DisplayList& displayListTransparent() const { return m_transparentList; }
    const DisplayList& displayListSprites() const { return m_spriteList; }
    const DisplayList& displayListDebug() const { return m_debugList; }
    const DisplayBlock& skyBlock() const { return m_skyGeometry; }

    const FogVolumeList& fogVolumes() const { return m_fogVolumes; }

    const InstancedList& instancedList() const { return m_instancedList; }

    void addLightRefs(Light* light);
    void enumLights(std::vector<Light*>& omni, std::vector<Light*>& spot, uint64_t frame);
    bool isSkyVisible() { return m_skyVisible; }
    bool isGlobalLit();

private:
    void addObjectData(DisplayObject* object);

    void markAll(const vec3& pos, const vec4& screenPlane, const vec3* frustum);
    void markAll(const vec3& pos);
    void markZoneObjects(const vec3& pos, const vec4& screenPlane, const vec3* frustum, Index zone);
    void markZoneObjects(const vec3& pos);
    void markZoneVis(const Zone& zone);

    void globalLightZoneVisibility(const vec3& dir, Index zoneInd, Index pzoneInd, Index prt);
    void zoneVisibility(vec3 pos, Index zoneInd, Index pzoneInd, Index prt);
    void zoneVisibility(vec3 pos, const vec4& screenPlane, const vec3* frustum, Index zoneInd);
};

} //namespace render