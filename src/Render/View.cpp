#include "View.h"

#include "Render/Light.h"
#include "Render/Clipping.h"
#include <algorithm>

namespace Render
{

View::View(World& world, uint8_t flags)
: m_world(world)
, m_flags(flags)
{
    m_global.mat = &Render::World::GlobalMat();
    m_skyGeometry.mat = &Render::World::GlobalMat();
    m_emissiveGeometry.mat = &Render::World::GlobalMat();
    m_transparentGeometry.mat = &Render::World::GlobalMat();
}

void View::update(const vec3& pos, const mat4& viewMat, uint64_t frame)
{
    Frustum frustum;
    frustum.update(viewMat);

    updateVisibility(pos, frustum, frame);
}

void View::update(const vec3& pos, float dist, uint64_t frame)
{
    m_distance = dist;
    updateVisibility(pos, frame);
}

void View::update(const vec3& dir, uint64_t frame)
{
    directionalVisibility(dir, frame);
}

void View::addObjectData(DisplayObject* object)
{
    object->setFrame(m_frame);

    for (const DisplayBlock& block : object->displayData())
    {
        if (block.hide) continue;
    
        switch (block.type)
        {
        case DisplayBlock::display_regular: m_displayList[DisplayRegular].push_back(&block); break;
        case DisplayBlock::display_layered: m_displayList[DisplayLayered].push_back(&block); break;
        case DisplayBlock::display_emissive: m_displayList[DisplayEmissive].push_back(&block); break;
        case DisplayBlock::display_transparent: m_displayList[DisplayTransparent].push_back(&block); break;
        case DisplayBlock::display_sprite: if (block.displayData[0]->vertexnum) m_displayList[DisplaySprites].push_back(&block); break;
        case DisplayBlock::display_debug: m_displayList[DisplayDebug].push_back(&block); break;
        }
    }
    
    if (!(m_flags & ViewInstanced)) return;
    if (object->instanceData().empty()) return;

    float dist = Clipping::PlaneDist(m_screenPlane, object);

    if (dist > 120.0f) return;
    
    for (const InstanceData& data : object->instanceData()) m_instancedList.push_back(&data);
}

void View::markAll(const Frustum& frustum)
{
    Index lfid = 0;

    for (const Leaf& leaf : m_world.leafs())
    {
        m_visLeaves.push_back(lfid++);

        if (frustum.test(leaf.bbox))
        {
            for (const auto& displayData : leaf.regularGeometry) m_global.displayData.push_back(&displayData);
            for (const auto& displayData : leaf.skyGeometry) m_skyGeometry.displayData.push_back(&displayData);
            for (const auto& displayData : leaf.emissiveGeometry) m_emissiveGeometry.displayData.push_back(&displayData);
            for (const auto& displayData : leaf.transparentGeometry) m_transparentGeometry.displayData.push_back(&displayData);

            if (leaf.flags & LeafSky) m_skyVisible = true;
        }
    }

    for (const Zone& zone : m_world.zones())
    {
        for (DisplayObject* obj : zone.objects)
        {
            if (frustum.test(obj->bbox(), obj->mat()))
            {
                for (const DisplayBlock& block : obj->displayData())
                {
                    switch (block.type)
                    {
                    case DisplayBlock::display_regular: m_displayList[DisplayRegular].push_back(&block); break;
                    case DisplayBlock::display_layered: m_displayList[DisplayLayered].push_back(&block); break;
                    case DisplayBlock::display_emissive: m_displayList[DisplayEmissive].push_back(&block); break;
                    case DisplayBlock::display_transparent: m_displayList[DisplayTransparent].push_back(&block); break;
                    case DisplayBlock::display_sprite: if (block.displayData[0]->vertexnum) m_displayList[DisplaySprites].push_back(&block); break;
                    }
                }
            }

        }
    }
}

void View::markAll(const vec3& pos)
{
    Index lfid = 0;

    for (const Leaf& leaf : m_world.leafs())
    {
        m_visLeaves.push_back(lfid++);

        for (const auto& displayData : leaf.regularGeometry) m_global.displayData.push_back(&displayData);
        for (const auto& displayData : leaf.skyGeometry) m_global.displayData.push_back(&displayData);
    }

    for (const Zone& zone : m_world.zones())
    {
        for (DisplayObject* obj : zone.objects)
        {
            for (const DisplayBlock& block : obj->displayData())
            {
                switch (block.type)
                {
                case DisplayBlock::display_regular: m_displayList[DisplayRegular].push_back(&block); break;
                case DisplayBlock::display_emissive: m_displayList[DisplayEmissive].push_back(&block); break;
                case DisplayBlock::display_transparent: m_displayList[DisplayTransparent].push_back(&block); break;
                }
            }
        }
    }
}

void View::markZoneObjects(const Frustum& frustum, Index zind)
{
    Zone& zone = m_world.zone(zind);

    const auto& leafs = m_world.leafs();

    for (const auto& lfid : zone.leafs)
    {
        m_visLeaves.push_back(lfid);

        const Leaf& leaf = m_world.leaf(lfid);

        if (frustum.test(leaf.bbox))
        {
            for (const auto& displayData : leaf.regularGeometry) m_global.displayData.push_back(&displayData);
            for (const auto& displayData : leaf.skyGeometry) m_skyGeometry.displayData.push_back(&displayData);
            for (const auto& displayData : leaf.emissiveGeometry) m_emissiveGeometry.displayData.push_back(&displayData);
            for (const auto& displayData : leaf.transparentGeometry) m_transparentGeometry.displayData.push_back(&displayData);
        
            if (leaf.flags & LeafSky) m_skyVisible = true;
        }
    }

    for (DisplayObject* obj : zone.objects)
    {
        if (m_frame == obj->frame()) continue;

        bool viewStatic = m_flags & ViewStatic;
        bool viewDynamic = m_flags & ViewDynamic;

        if (!viewStatic && !obj->isDynamic()) continue;
        if (!viewDynamic && obj->isDynamic()) continue;

        if (frustum.test(obj->bbox(), obj->mat())) addObjectData(obj);
    }

    for (FogVolume* volume : zone.fogVolumes)
    {
        if (m_frame == volume->frame()) continue;
    
        if (frustum.test(volume->pos(), volume->size()))
        {
            volume->setFrame(m_frame);
            m_fogVolumes.push_back(volume);
        }
    }
}

void View::markZoneObjects(const vec3& pos)
{
    Zone& zone = m_world.zone(m_zone);

    const auto& leafs = m_world.leafs();

    if (m_flags & ViewLeafs)
    {
        for (const auto& lfid : zone.leafs)
        {
            const Leaf& leaf = m_world.leaf(lfid);

            vec3 mid = (leaf.bbox.min + leaf.bbox.max) * 0.5;
            vec3 bbpos = mid;

            vec3 box = leaf.bbox.max - mid;

            if ((m_flags & ViewRestrictDist) &&
                !(AABBTest(bbpos, box, pos, vec3(m_distance, m_distance, m_distance)))) continue;

            m_visLeaves.push_back(lfid);

            for (const auto& displayData : leaf.regularGeometry) m_global.displayData.push_back(&displayData);
            for (const auto& displayData : leaf.skyGeometry) m_global.displayData.push_back(&displayData);
        }
    }

    for (DisplayObject* obj : zone.objects)
    {
        if (m_frame == obj->frame()) continue;

        bool viewStatic = m_flags & ViewStatic;
        bool viewDynamic = m_flags & ViewDynamic;

        if (!viewStatic && !obj->isDynamic()) continue;
        if (!viewDynamic && obj->isDynamic()) continue;

        if (m_flags & ViewRestrictDist)
        {
            vec3 bbpos;
            vec3 box;

            obj->worldBBox(bbpos, box);

            if (!AABBTest(bbpos, box, pos, vec3(m_distance, m_distance, m_distance))) continue;
        }

        obj->setFrame(m_frame);

        for (const DisplayBlock& block : obj->displayData())
        {
            switch (block.type)
            {
            case DisplayBlock::display_regular: m_displayList[DisplayRegular].push_back(&block); break;
            case DisplayBlock::display_emissive: m_displayList[DisplayEmissive].push_back(&block); break;
            case DisplayBlock::display_transparent: m_displayList[DisplayTransparent].push_back(&block); break;
            }
        }
    }
}

void View::markZoneVis(const Zone& zone)
{
    for (const auto& lfid : zone.leafs)
    {
        Leaf& leaf = m_world.leaf(lfid);

        if (m_frame == leaf.frame) continue;
        leaf.frame = m_frame;
        m_visLeaves.push_back(lfid);

        for (const auto& displayData : leaf.regularGeometry) m_global.displayData.push_back(&displayData);
    }

    for (DisplayObject* obj : zone.objects)
    {
        if (m_frame == obj->frame()) continue;

        obj->setFrame(m_frame);

        for (const DisplayBlock& block : obj->displayData())
        {
            if (block.type == DisplayBlock::display_regular) m_displayList[DisplayRegular].push_back(&block);
        }
    }
}

void View::zoneVisibility(vec3 pos, const Frustum& frustum, Index zoneInd, Index pzoneInd)
{
    Zone& zone = m_world.zone(zoneInd);

    for (Index pind : zone.portals)
    {
        Portal& portal = m_world.portal(pind);

        Index opzoneInd = portal.zone[0];
        if (opzoneInd == zoneInd) opzoneInd = portal.zone[1];

        if (pzoneInd != InvalidIndex)
        {
            if (opzoneInd == pzoneInd) continue;

            //Split portal
            if (!Clipping::PortalVisFrustum(frustum, portal)) continue;

        } else portal.bid = -1;

        Zone& opzone = m_world.zone(opzoneInd);

        Frustum pfrustum;
        pfrustum.update(pos, portal.plane, portal.vertices());

        //Check leaf visibility
        if (m_flags & ViewLeafs)
        {
            for (Index lid : opzone.leafs)
            {
                Leaf& leaf = m_world.leaf(lid);

                vec3 mid = (leaf.bbox.min + leaf.bbox.max) * 0.5;
                vec3 bbpos = mid;

                vec3 box = leaf.bbox.max - mid;

                if ((m_flags & ViewRestrictDist) &&
                    !(AABBTest(bbpos, box, pos, vec3(m_distance, m_distance, m_distance)))) continue;

                if (pfrustum.test(leaf.bbox))
                {
                    if (leaf.frame != m_frame)
                    {
                        leaf.frame = m_frame;
                        m_visLeaves.push_back(lid);
                        //AddReflectionSurf(leaf);

                        for (const auto& displayData : leaf.regularGeometry) m_global.displayData.push_back(&displayData);
                        for (const auto& displayData : leaf.regularGeometry) m_skyGeometry.displayData.push_back(&displayData);

                        if (leaf.flags & LeafSky) m_skyVisible = true;
                    }
                }
            }
        }

        //Object visibility
        for (DisplayObject* obj : opzone.objects)
        {
            if (m_frame == obj->frame()) continue;

            bool viewStatic = m_flags & ViewStatic;
            bool viewDynamic = m_flags & ViewDynamic;

            if (!viewStatic && !obj->isDynamic()) continue;
            if (!viewDynamic && obj->isDynamic()) continue;

            if (m_flags & ViewRestrictDist)
            {
                vec3 bbpos;
                vec3 box;

                obj->worldBBox(bbpos, box);

                if(!(AABBTest(bbpos, box, pos, vec3(m_distance, m_distance, m_distance)))) continue;
            }

            if (pfrustum.test(obj->bbox(), obj->mat())) addObjectData(obj);
        }

        zoneVisibility(pos, pfrustum, opzoneInd, zoneInd);
    }
}

void View::zoneVisibility(vec3 pos, const Frustum& frustum, Index zoneInd)
{
    Zone& zone = m_world.zone(zoneInd);

    for (Index pind : zone.portals)
    {
        Portal& portal = m_world.portal(pind);

        Index opzoneInd = portal.zone[0];
        if (opzoneInd == zoneInd) opzoneInd = portal.zone[1];

        if (opzoneInd == m_zone) continue;

        // If eye pos is at portal plane use frustrum for culling
        float pdist = portal.plane.xyz * pos + portal.plane.w;
        if (fabs(pdist) < math::eps)
        {
            markZoneObjects(frustum, opzoneInd);
            zoneVisibility(pos, frustum, opzoneInd);

            continue;
        }

        //Split portal
        if (!Clipping::PortalVisFrustum(frustum, portal)) continue;
  
        Zone& opzone = m_world.zone(opzoneInd);

        Frustum pfrustum;
        pfrustum.update(pos, portal.plane, portal.vertices());

        //Check leaf visibility
        if (m_flags & ViewLeafs)
        {
            for (Index lid : opzone.leafs)
            {
                Leaf& leaf = m_world.leaf(lid);

                vec3 mid = (leaf.bbox.min + leaf.bbox.max) * 0.5;
                vec3 bbpos = mid;

                vec3 box = leaf.bbox.max - mid;

                if ((m_flags & ViewRestrictDist) &&
                    !(AABBTest(bbpos, box, pos, vec3(m_distance, m_distance, m_distance)))) continue;

                if (pfrustum.test(leaf.bbox))
                {
                    if (leaf.frame != m_frame)
                    {
                        leaf.frame = m_frame;
                        m_visLeaves.push_back(lid);
                        //AddReflectionSurf(leaf);

                        for (const auto& displayData : leaf.regularGeometry) m_global.displayData.push_back(&displayData);
                        for (const auto& displayData : leaf.skyGeometry) m_skyGeometry.displayData.push_back(&displayData);
                        for (const auto& displayData : leaf.emissiveGeometry) m_emissiveGeometry.displayData.push_back(&displayData);
                        for (const auto& displayData : leaf.transparentGeometry) m_transparentGeometry.displayData.push_back(&displayData);

                        if (leaf.flags & LeafSky) m_skyVisible = true;
                    }
                }
            }
        }

        //Object visibility
        for (DisplayObject* obj : opzone.objects)
        {
            if (m_frame == obj->frame()) continue;

            if (pfrustum.test(obj->bbox(), obj->mat())) addObjectData(obj);
        }

        // Fog volumes visibility
        for (FogVolume* volume : opzone.fogVolumes)
        {
            if (m_frame == volume->frame()) continue;

            if (pfrustum.test(volume->pos(), volume->size()))
            {
                volume->setFrame(m_frame);

                m_fogVolumes.push_back(volume);
            }
        }

        zoneVisibility(pos, pfrustum, opzoneInd, zoneInd);
    }
}

void View::updateVisibility(const vec3& pos, const Frustum& frustum, uint64_t frame)
{
    m_frame = frame;
    m_screenPlane = frustum.plane(0);

    m_skyVisible = false;

    if (m_flags & ViewLeafs) m_visLeaves.clear();
    m_displayList[DisplayRegular].clear();
    m_displayList[DisplayLayered].clear();
    m_displayList[DisplayEmissive].clear();
    m_displayList[DisplayTransparent].clear();
    m_displayList[DisplaySprites].clear();
    m_displayList[DisplayDebug].clear();

    m_instancedList.clear();

    m_fogVolumes.clear();

    m_global.vertexData = m_world.vertexData();
    m_global.displayData.clear();

    m_skyGeometry.vertexData = m_world.vertexData();
    m_skyGeometry.displayData.clear();

    m_emissiveGeometry.vertexData = m_world.vertexData();
    m_emissiveGeometry.displayData.clear();

    m_transparentGeometry.vertexData = m_world.vertexData();
    m_transparentGeometry.displayData.clear();

    m_displayList[DisplayRegular].push_back(&m_global);

    Index leaf = m_world.tracePos(pos);

    if (leaf == InvalidIndex)
    {
        m_zone = InvalidIndex;
        markAll(frustum);
        return;
    }
    
    m_zone = m_world.leaf(leaf).zone;
    markZoneObjects(frustum, m_zone);

    zoneVisibility(pos, frustum, m_zone);

    if (!m_emissiveGeometry.displayData.empty()) m_displayList[DisplayEmissive].push_back(&m_emissiveGeometry);
    if (!m_transparentGeometry.displayData.empty()) m_displayList[DisplayTransparent].push_back(&m_transparentGeometry);

    if (!m_fogVolumes.empty())
    {
        std::sort(m_fogVolumes.begin(), m_fogVolumes.end(), [&pos](FogVolume * a, FogVolume * b) {

            float adist = (a->pos() - pos).length();
            float bdist = (b->pos() - pos).length();

            return adist > bdist;
        });
    }
}

void View::updateVisibility(const vec3& pos, uint64_t frame)
{
    m_frame = frame;

    if(m_flags & ViewLeafs) m_visLeaves.clear();
    m_displayList[DisplayRegular].clear();
    m_displayList[DisplayEmissive].clear();
    m_displayList[DisplayTransparent].clear();

    m_global.vertexData = m_world.vertexData();
    m_global.displayData.clear();

    m_displayList[DisplayRegular].push_back(&m_global);

    Index leaf = m_world.tracePos(pos);

    if (leaf == InvalidIndex)
    {
        m_zone = InvalidIndex;
        return;
    }

    m_zone = m_world.leaf(leaf).zone;
    markZoneObjects(pos);

    zoneVisibility(pos, {}, m_zone, InvalidIndex);
}

void View::directionalZoneVisibility(const vec3& dir, Index zoneInd, Index pzoneInd, Index prt)
{
    Zone& zone = m_world.zone(zoneInd);

    for (Index pind : zone.portals)
    {
        Portal& portal = m_world.portal(pind);

        Index opzoneInd = portal.zone[0];
        if (opzoneInd == zoneInd) opzoneInd = portal.zone[1];

        if (pzoneInd != InvalidIndex)
        {
            if (opzoneInd == pzoneInd) continue;

            //Split portal
            Portal& clipPortal = m_world.portal(prt);
            
            if (!Clipping::PortalVisDir(dir, clipPortal, portal)) continue;

        } else portal.bid = -1;

        Zone& opzone = m_world.zone(opzoneInd);
        markZoneVis(opzone);

        //Check leaf visibility
        /*for (Index lid : opzone.leafs)
        {
            Leaf& leaf = m_world.leaf(lid);

            vec3 mid = (leaf.bbox.min + leaf.bbox.max) * 0.5;
            vec3 bbpos = mid;

            vec3 box = leaf.bbox.max - mid;

            if (clipping::LeafVisDir(dir, portal, leaf.bbox))
            {
                if (leaf.frame != m_frame)
                {
                    leaf.frame = m_frame;
                    m_visLeaves.push_back(lid);

                    for (const auto& displayData : leaf.regularGeometry) m_global.displayData.push_back(&displayData);
                }
            }
        }

        //Object visibility
        for (DisplayObject* obj : opzone.objects)
        {
            if (clipping::ObjVisDir(dir, portal, obj))
            {
                if (obj->frame != m_frame)
                {
                    obj->frame = m_frame;

                    for (const DisplayBlock& block : obj->displayData())
                    {
                        if (block.type == DisplayBlock::display_regular) m_regularList.push_back(&block);
                    }
                }
            }
        }*/

        directionalZoneVisibility(dir, opzoneInd, zoneInd, pind);
    }
}

void View::directionalVisibility(const vec3& dir, uint64_t frame)
{
    m_frame = frame;

    m_visLeaves.clear();

    m_displayList[DisplayRegular].clear();
    m_global.displayData.clear();

    m_global.vertexData = m_world.vertexData();
    m_displayList[DisplayRegular].push_back(&m_global);

    Index zoneInd = 0;

    for (const Zone& zone : m_world.zones())
    {
        for (const auto& lfid : zone.leafs)
        {
            Leaf& leaf = m_world.leaf(lfid);

            if (leaf.flags & LeafSky)
            {
                //m_visZones.push_back(zoneInd);

                markZoneVis(zone);
                directionalZoneVisibility(dir, zoneInd, InvalidIndex, InvalidIndex);
                break;
            }
        }
        
        zoneInd++;
    }

    for (unsigned long id : m_visLeaves) m_world.leaf(id).globalLit = true;
}

void View::addLightRefs(Light* light)
{
    for (Index lfid : m_visLeaves)
    {
        Leaf& leaf = m_world.leaf(lfid);
        light->addReference(leaf.lights);
    }
}

void View::enumLights(std::vector<Light*>& omni, std::vector<Light*>& spot, uint64_t frame)
{
    omni.clear();
    spot.clear();

    for (Index lfid : m_visLeaves)
    {
        Leaf& leaf = m_world.leaf(lfid);

        for (Light* light : leaf.lights)
        {
            if (light->frame() != frame)
            {
                light->setFrame(frame);

                if (light->type() == LightType::Omni)
                    omni.push_back(light);
                else
                    spot.push_back(light);
            }
        }
    }
}

bool View::isGlobalLit()
{
    for (unsigned long id : m_visLeaves)
    {
        const Leaf& leaf = m_world.leaf(id);
        if (leaf.globalLit) return true;
    }

    return false;
}

} //namespace Render