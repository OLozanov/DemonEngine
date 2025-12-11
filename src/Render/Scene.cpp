#include "Scene.h"
#include "Render/Light.h"
#include "Render/Clipping.h"
#include <algorithm>

namespace Render
{

Scene::Scene(World& world, uint8_t flags)
: m_world(world)
, m_flags(flags)
{
    m_global.mat = &Render::World::GlobalMat();
    m_skyGeometry.mat = &Render::World::GlobalMat();
    m_emissiveGeometry.mat = &Render::World::GlobalMat();
    m_transparentGeometry.mat = &Render::World::GlobalMat();
}

void Scene::markAll(const vec3& pos, const vec4& screenPlane, const vec3* frustum)
{
    Index lfid = 0;

    for (const Leaf& leaf : m_world.leafs())
    {
        m_visLeaves.push_back(lfid++);

        if (Clipping::FrustumLeafVis(pos, frustum, screenPlane, leaf.bbox))
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
            if (Clipping::FrustumObjVis(pos, frustum, screenPlane, *obj))
            {
                for (const DisplayBlock& block : obj->displayData())
                {
                    switch (block.type)
                    {
                    case DisplayBlock::display_regular: m_regularList.push_back(&block); break;
                    case DisplayBlock::display_layered: m_layeredList.push_back(&block); break;
                    case DisplayBlock::display_emissive: m_emissiveList.push_back(&block); break;
                    case DisplayBlock::display_transparent: m_transparentList.push_back(&block); break;
                    case DisplayBlock::display_sprite: if (block.displayData[0]->vertexnum) m_spriteList.push_back(&block); break;
                    }
                }
            }

        }
    }
}

void Scene::markAll(const vec3& pos)
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
                case DisplayBlock::display_regular: m_regularList.push_back(&block); break;
                case DisplayBlock::display_emissive: m_emissiveList.push_back(&block); break;
                case DisplayBlock::display_transparent: m_transparentList.push_back(&block); break;
                }
            }
        }
    }
}

void Scene::markZoneObjects(const vec3& pos, const vec4& screenPlane, const vec3* frustum, Index zind)
{
    Zone& zone = m_world.zone(zind);

    const auto& leafs = m_world.leafs();

    for (const auto& lfid : zone.leafs)
    {
        m_visLeaves.push_back(lfid);

        const Leaf& leaf = m_world.leaf(lfid);

        if (Clipping::FrustumLeafVis(pos, frustum, screenPlane, leaf.bbox))
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

        obj->setFrame(m_frame);

        if (Clipping::FrustumObjVis(pos, frustum, screenPlane, *obj))
        {
            for (const DisplayBlock& block : obj->displayData())
            {
                if (block.hide) continue;

                switch (block.type)
                {
                case DisplayBlock::display_regular: m_regularList.push_back(&block); break;
                case DisplayBlock::display_layered: m_layeredList.push_back(&block); break;
                case DisplayBlock::display_emissive: m_emissiveList.push_back(&block); break;
                case DisplayBlock::display_transparent: m_transparentList.push_back(&block); break;
                case DisplayBlock::display_sprite: if (block.displayData[0]->vertexnum) m_spriteList.push_back(&block); break;
                case DisplayBlock::display_debug: m_debugList.push_back(&block); break;
                }
            }

            if (obj->instanceData().empty()) continue;

            for (const InstanceData& data : obj->instanceData()) m_instancedList.push_back(&data);
        }
    }

    for (FogVolume* volume : zone.fogVolumes)
    {
        if (m_frame == volume->frame()) continue;
    
        if (Clipping::FrustumAABBVis(pos, frustum, screenPlane, volume->pos(), volume->size()))
        {
            volume->setFrame(m_frame);
            m_fogVolumes.push_back(volume);
        }
    }
}

void Scene::markZoneObjects(const vec3& pos)
{
    Zone& zone = m_world.zone(m_zone);

    const auto& leafs = m_world.leafs();

    if (m_flags & vis_leafs)
    {
        for (const auto& lfid : zone.leafs)
        {
            const Leaf& leaf = m_world.leaf(lfid);

            vec3 mid = (leaf.bbox.min + leaf.bbox.max) * 0.5;
            vec3 bbpos = mid;

            vec3 box = leaf.bbox.max - mid;

            if ((m_flags & vis_restrict_dist) &&
                !(AABBTest(bbpos, box, pos, vec3(m_distance, m_distance, m_distance)))) continue;

            m_visLeaves.push_back(lfid);

            for (const auto& displayData : leaf.regularGeometry) m_global.displayData.push_back(&displayData);
            for (const auto& displayData : leaf.skyGeometry) m_global.displayData.push_back(&displayData);
        }
    }

    for (DisplayObject* obj : zone.objects)
    {
        if (m_frame == obj->frame()) continue;

        bool visStatic = m_flags & vis_static;
        bool visDynamic = m_flags & vis_dynamic;

        if (!visStatic && !obj->isDynamic()) continue;
        if (!visDynamic && obj->isDynamic()) continue;

        if (m_flags & vis_restrict_dist)
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
            case DisplayBlock::display_regular: m_regularList.push_back(&block); break;
            case DisplayBlock::display_emissive: m_emissiveList.push_back(&block); break;
            case DisplayBlock::display_transparent: m_transparentList.push_back(&block); break;
            }
        }
    }
}

void Scene::markZoneVis(const Zone& zone)
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
            if (block.type == DisplayBlock::display_regular) m_regularList.push_back(&block);
        }
    }
}

void Scene::zoneVisibility(vec3 pos, Index zoneInd, Index pzoneInd, Index prt)
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

            Portal& clipPortal = m_world.portal(prt);

            //Split portal
            if (!Clipping::PortalVis(pos, clipPortal, portal)) continue;

        } else portal.bid = -1;

        Zone& opzone = m_world.zone(opzoneInd);

        //Check leaf visibility
        if (m_flags & vis_leafs)
        {
            for (Index lid : opzone.leafs)
            {
                Leaf& leaf = m_world.leaf(lid);

                vec3 mid = (leaf.bbox.min + leaf.bbox.max) * 0.5;
                vec3 bbpos = mid;

                vec3 box = leaf.bbox.max - mid;

                if ((m_flags & vis_restrict_dist) &&
                    !(AABBTest(bbpos, box, pos, vec3(m_distance, m_distance, m_distance)))) continue;

                if (Clipping::LeafVis(pos, portal, leaf.bbox))
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

            bool visStatic = m_flags & vis_static;
            bool visDynamic = m_flags & vis_dynamic;

            if (!visStatic && !obj->isDynamic()) continue;
            if (!visDynamic && obj->isDynamic()) continue;

            if (m_flags & vis_restrict_dist)
            {
                vec3 bbpos;
                vec3 box;

                obj->worldBBox(bbpos, box);

                if(!(AABBTest(bbpos, box, pos, vec3(m_distance, m_distance, m_distance)))) continue;
            }

            if (Clipping::ObjVis(pos, portal, obj))
            {
                if (obj->frame() != m_frame)
                {
                    obj->setFrame(m_frame);

                    for (const DisplayBlock& block : obj->displayData())
                    {
                        switch (block.type)
                        {
                        case DisplayBlock::display_regular: m_regularList.push_back(&block); break;
                        case DisplayBlock::display_layered: m_layeredList.push_back(&block); break;
                        case DisplayBlock::display_emissive: m_emissiveList.push_back(&block); break;
                        case DisplayBlock::display_transparent: m_transparentList.push_back(&block); break;
                        }
                    }

                    if (obj->instanceData().empty()) continue;

                    for (const InstanceData& data : obj->instanceData()) m_instancedList.push_back(&data);
                }
            }
        }

        zoneVisibility(pos, opzoneInd, zoneInd, pind);
    }
}

void Scene::zoneVisibility(vec3 pos, const vec4& screenPlane, const vec3* frustum, Index zoneInd)
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
            markZoneObjects(pos, screenPlane, frustum, opzoneInd);
            zoneVisibility(pos, screenPlane, frustum, opzoneInd);

            return;
        }

        //Split portal
        if (!Clipping::PortalVisFrustum(pos, frustum, screenPlane, portal)) continue;
  
        Zone& opzone = m_world.zone(opzoneInd);

        //Check leaf visibility
        if (m_flags & vis_leafs)
        {
            for (Index lid : opzone.leafs)
            {
                Leaf& leaf = m_world.leaf(lid);

                vec3 mid = (leaf.bbox.min + leaf.bbox.max) * 0.5;
                vec3 bbpos = mid;

                vec3 box = leaf.bbox.max - mid;

                if ((m_flags & vis_restrict_dist) &&
                    !(AABBTest(bbpos, box, pos, vec3(m_distance, m_distance, m_distance)))) continue;

                if (Clipping::LeafVis(pos, portal, leaf.bbox))
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
            if (Clipping::ObjVis(pos, portal, obj))
            {
                if (obj->frame() != m_frame)
                {
                    obj->setFrame(m_frame);

                    for (const DisplayBlock& block : obj->displayData())
                    {
                        if (block.hide) continue;

                        switch (block.type)
                        {
                        case DisplayBlock::display_regular: m_regularList.push_back(&block); break;
                        case DisplayBlock::display_layered: m_layeredList.push_back(&block); break;
                        case DisplayBlock::display_emissive: m_emissiveList.push_back(&block); break;
                        case DisplayBlock::display_transparent: m_transparentList.push_back(&block); break;
                        case DisplayBlock::display_sprite: if(block.displayData[0]->vertexnum) m_spriteList.push_back(&block); break;
                        }
                    }

                    if (obj->instanceData().empty()) continue;

                    for (const InstanceData& data : obj->instanceData()) m_instancedList.push_back(&data);
                }
            }
        }

        // Fog volumes visibility
        for (FogVolume* volume : opzone.fogVolumes)
        {
            if (m_frame == volume->frame()) continue;

            if (Clipping::AABBVis(pos, portal, volume->size(), volume->pos()))
            {
                volume->setFrame(m_frame);

                m_fogVolumes.push_back(volume);
            }
        }

        zoneVisibility(pos, opzoneInd, zoneInd, pind);
    }
}

void Scene::calculateVisibility(const vec3& pos, const vec4& screenPlane, const vec3* frustum, uint64_t frame)
{
    m_frame = frame;

    m_skyVisible = false;

    m_visLeaves.clear();
    m_regularList.clear();
    m_layeredList.clear();
    m_emissiveList.clear();
    m_transparentList.clear();
    m_spriteList.clear();
    m_debugList.clear();

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

    m_regularList.push_back(&m_global);

    Index leaf = m_world.tracePos(pos);

    if (leaf == InvalidIndex)
    {
        m_zone = InvalidIndex;
        markAll(pos, screenPlane, frustum);
        return;
    }
    
    m_zone = m_world.leaf(leaf).zone;
    markZoneObjects(pos, screenPlane, frustum, m_zone);

    zoneVisibility(pos, screenPlane, frustum, m_zone);

    if (!m_emissiveGeometry.displayData.empty()) m_emissiveList.push_back(&m_emissiveGeometry);
    if (!m_transparentGeometry.displayData.empty()) m_transparentList.push_back(&m_transparentGeometry);

    if (!m_fogVolumes.empty())
    {
        std::sort(m_fogVolumes.begin(), m_fogVolumes.end(), [&pos](FogVolume * a, FogVolume * b) {

            float adist = (a->pos() - pos).length();
            float bdist = (b->pos() - pos).length();

            return adist > bdist;
        });
    }
}

void Scene::calculateVisibility(const vec3& pos, uint64_t frame)
{
    m_frame = frame;

    if(m_flags & vis_leafs) m_visLeaves.clear();
    m_regularList.clear();
    m_emissiveList.clear();
    m_transparentList.clear();

    m_global.vertexData = m_world.vertexData();
    m_global.displayData.clear();

    m_regularList.push_back(&m_global);

    Index leaf = m_world.tracePos(pos);

    if (leaf == InvalidIndex)
    {
        m_zone = InvalidIndex;
        return;
    }

    m_zone = m_world.leaf(leaf).zone;
    markZoneObjects(pos);

    zoneVisibility(pos, m_zone, InvalidIndex, InvalidIndex);
}

void Scene::globalLightZoneVisibility(const vec3& dir, Index zoneInd, Index pzoneInd, Index prt)
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

        globalLightZoneVisibility(dir, opzoneInd, zoneInd, pind);
    }
}

void Scene::globalLightVisibility(const vec3& dir, uint64_t frame)
{
    m_frame = frame;

    m_visLeaves.clear();

    m_regularList.clear();
    m_global.displayData.clear();

    m_global.vertexData = m_world.vertexData();
    m_regularList.push_back(&m_global);

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
                globalLightZoneVisibility(dir, zoneInd, InvalidIndex, InvalidIndex);
                break;
            }
        }
        
        zoneInd++;
    }

    for (unsigned long id : m_visLeaves) m_world.leaf(id).globalLit = true;
}

void Scene::addLightRefs(Light* light)
{
    for (Index lfid : m_visLeaves)
    {
        Leaf& leaf = m_world.leaf(lfid);
        light->addReference(leaf.lights);
    }
}

void Scene::enumLights(std::vector<Light*>& omni, std::vector<Light*>& spot, uint64_t frame)
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

bool Scene::isGlobalLit()
{
    for (unsigned long id : m_visLeaves)
    {
        const Leaf& leaf = m_world.leaf(id);
        if (leaf.globalLit) return true;
    }

    return false;
}

} //namespace render