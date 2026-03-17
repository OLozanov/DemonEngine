#pragma once

#include "Render/World.h"
#include "Render/Frustum.h"
#include <vector>

namespace Render
{

class Light;
using LightList = std::vector<Light*>;
using FogVolumeList = std::vector<FogVolume*>;

class View
{
public:
    enum Flags
    {
        ViewLeafs = 1,
        ViewObjects = 2,
        ViewStatic = 4,
        ViewDynamic = 8,
        ViewInstanced = 16,
        ViewRestrictDist = 32,
    };

    enum DisplayType
    {
        DisplayRegular = 0,
        DisplayLayered = 1,
        DisplayEmissive = 2,
        DisplayTransparent = 3,
        DisplaySprites = 4,
        DisplayDebug = 5,
        DisplayListNum
    };

public:
    View(World& world, uint8_t flags = ViewLeafs | ViewStatic | ViewDynamic);

    void setFlags(uint8_t flags) { m_flags = flags; }
    void setDistance(float distance) { m_distance = distance; }

    void update(const vec3& pos, const mat4& viewMat, uint64_t frame);
    void update(const vec3& pos, float dist, uint64_t frame);
    void update(const vec3& dir, uint64_t frame);

    const DisplayList& displayList(size_t type) const { return m_displayList[type]; }
    const FogVolumeList& fogVolumes() const { return m_fogVolumes; }
    const InstancedList& instancedList() const { return m_instancedList; }

    void addLightRefs(Light* light);
    void enumLights(std::vector<Light*>& omni, std::vector<Light*>& spot, uint64_t frame);
    bool isSkyVisible() { return m_skyVisible; }
    bool isGlobalLit();

private:
    void addObjectData(DisplayObject* object);

    void markAll(const Frustum& frustum);
    void markAll(const vec3& pos);
    void markZoneObjects(const Frustum& frustum, Index zone);
    void markZoneObjects(const vec3& pos);
    void markZoneVis(const Zone& zone);

    void directionalZoneVisibility(const vec3& dir, Index zoneInd, Index pzoneInd, Index prt);
    void zoneVisibility(vec3 pos, const Frustum& frustum, Index zoneInd, Index pzoneInd);
    void zoneVisibility(vec3 pos, const Frustum& frustum, Index zoneInd);

    void updateVisibility(const vec3& pos, const Frustum& frustum, uint64_t frame);
    void updateVisibility(const vec3& pos, uint64_t frame);
    void directionalVisibility(const vec3& dir, uint64_t frame);

private:
    World& m_world;

    DisplayBlock m_global;
    DisplayBlock m_skyGeometry;
    DisplayBlock m_emissiveGeometry;
    DisplayBlock m_transparentGeometry;

    DisplayList m_displayList[DisplayListNum];
    InstancedList m_instancedList;
    FogVolumeList m_fogVolumes;

    std::vector<Index> m_visLeaves;

    uint8_t m_flags;

    float m_distance;

    uint64_t m_frame;
    Index m_zone;

    vec4 m_screenPlane;

    bool m_skyVisible;
};

} //namespace Render