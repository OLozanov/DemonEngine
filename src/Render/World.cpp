#include "World.h"
#include "Render/Light.h"

namespace Render
{

void World::setData(const std::vector<Render::Vertex>& vertices,
                    const std::vector<Render::IndexType>& indices,
                    std::vector<Leaf>& leafs,
                    std::vector<Node>& nodes,
                    std::vector<Zone>& zones,
                    std::vector<Portal>& portals)
{
    m_vertexBuffer.setData(vertices.data(), vertices.size());
    m_indexBuffer.setData(indices.data(), indices.size());

    m_leafs = std::move(leafs);
    m_nodes = std::move(nodes);
    m_zones = std::move(zones);
    m_portals = std::move(portals);
}

void World::reset()
{
    m_vertexBuffer.reset();
    m_indexBuffer.reset();

    m_leafs.clear();
    m_nodes.clear();
    m_zones.clear();
    m_portals.clear();
}

Index World::tracePos(const Node& node, const vec3& pos) const
{
    if (node.leaf != InvalidIndex) return node.leaf;

    float dist = pos * node.plane.xyz + node.plane.w;

    if (dist >= 0)
    {
        if (node.right != InvalidIndex) return tracePos(m_nodes[node.right], pos);
        else return node.leaf;
    }
    else
    {
        if (node.left != InvalidIndex) return tracePos(m_nodes[node.left], pos);
        else return InvalidIndex;
    }

    return InvalidIndex;
}

void World::traceObject(DisplayObject* object, Node& node, const vec3& pos, const vec3& bbox, const vec3* axis)
{
    if (node.leaf != InvalidIndex)
    {
        Leaf& leaf = m_leafs[node.leaf];

        Zone& zone = m_zones[leaf.zone];
        object->addReference(zone.objects);

        return;
    }

    float r = fabs(axis[0] * node.plane.xyz) * bbox.x + fabs(axis[1] * node.plane.xyz) * bbox.y + fabs(axis[2] * node.plane.xyz) * bbox.z;
    float dist = pos * node.plane.xyz + node.plane.w;

    if ((dist + r) > -math::eps)
    {
        if (node.right != InvalidIndex) traceObject(object, m_nodes[node.right], pos, bbox, axis);
    }

    if ((dist - r) < math::eps)
    {
        if (node.left != InvalidIndex) traceObject(object, m_nodes[node.left], pos, bbox, axis);
    }
}

void World::traceLight(Light* light, Node& node, const vec3& pos, const vec3& bbox)
{
    if (node.leaf != InvalidIndex)
    {
        Leaf& leaf = m_leafs[node.leaf];
        light->addReference(leaf.lights);
        return;
    }

    float r = fabs(node.plane.x) * bbox.x + fabs(node.plane.y) * bbox.y + fabs(node.plane.z) * bbox.z;
    float dist = pos * node.plane.xyz + node.plane.w;

    if ((dist + r) > -math::eps)
    {
        if (node.right != InvalidIndex) traceLight(light, m_nodes[node.right], pos, bbox);
    }

    if ((dist - r) < math::eps)
    {
        if (node.left != InvalidIndex) traceLight(light, m_nodes[node.left], pos, bbox);
    }
}

void World::traceDecal(Decal* decal, Node& node, const vec3& pos, const vec3& bbox, const vec3* axis)
{
    if (node.leaf != InvalidIndex)
    {
        Leaf& leaf = m_leafs[node.leaf];

        Zone& zone = m_zones[leaf.zone];
        decal->addReference(zone.decals);

        return;
    }

    float r = fabs(axis[0] * node.plane.xyz) * bbox.x + fabs(axis[1] * node.plane.xyz) * bbox.y + fabs(axis[2] * node.plane.xyz) * bbox.z;
    float dist = pos * node.plane.xyz + node.plane.w;

    if ((dist + r) > -math::eps)
    {
        if (node.right != InvalidIndex) traceDecal(decal, m_nodes[node.right], pos, bbox, axis);
    }

    if ((dist - r) < math::eps)
    {
        if (node.left != InvalidIndex) traceDecal(decal , m_nodes[node.left], pos, bbox, axis);
    }
}

void World::traceFogVolume(FogVolume* volume, Node& node, const vec3& pos, const vec3& bbox)
{
    if (node.leaf != InvalidIndex)
    {
        Leaf& leaf = m_leafs[node.leaf];

        Zone& zone = m_zones[leaf.zone];
        volume->addReference(zone.fogVolumes);

        return;
    }

    float r = fabs(node.plane.x) * bbox.x + fabs(node.plane.y) * bbox.y + fabs(node.plane.z) * bbox.z;
    float dist = pos * node.plane.xyz + node.plane.w;

    if ((dist + r) > -math::eps)
    {
        if (node.right != InvalidIndex) traceFogVolume(volume, m_nodes[node.right], pos, bbox);
    }

    if ((dist - r) < math::eps)
    {
        if (node.left != InvalidIndex) traceFogVolume(volume, m_nodes[node.left], pos, bbox);
    }
}

void World::addObject(DisplayObject* object)
{
    const mat4& mat = object->mat();
    const BBox& bbox = object->bbox();

    vec3 opos = object->mat()[3];

    mat3 rot = mat;

    vec3 mid = (bbox.min + bbox.max) * 0.5;
    vec3 bbpos = opos + rot * mid;

    vec3 box = bbox.max - mid;

    traceObject(object, m_nodes[0], bbpos, box, &rot[0]);
}

void World::addLight(Light* light)
{
    float ldist = light->radius() + light->falloff();
    traceLight(light, m_nodes[0], light->pos(), { ldist, ldist, ldist });
}

void  World::addDecal(Decal* decal)
{
    const mat3& mat = decal->orientation();
    traceDecal(decal, m_nodes[0], decal->pos(), decal->size(), &mat[0]);
}

void World::addFogVolume(FogVolume* volume)
{
    traceFogVolume(volume, m_nodes[0], volume->pos(), volume->size());
}

void World::moveObject(DisplayObject* object)
{
    object->clearReferences();
    addObject(object);
}

void World::moveLight(Light* light)
{
    light->clearReferences();
    addLight(light);
}

void World::moveFogVolume(FogVolume* volume)
{
    volume->clearReferences();
    addFogVolume(volume);
}

uint8_t World::zoneType(const vec3& pos) const
{
    Index leafId = tracePos(pos);

    if (leafId != InvalidIndex)
    {
        const Leaf& leaf = m_leafs[leafId];

        const Zone& zone = m_zones[leaf.zone];
        return zone.type;
    }
    else return 0;
}

float World::getSubmergeDepth(const vec3& pos) const
{
    Index leafId = tracePos(pos);

    if (leafId != InvalidIndex)
    {
        const Leaf& leaf = m_leafs[leafId];
        const Zone& zone = m_zones[leaf.zone];

        if (zone.type == 0) return 0;

        for (int p = 0; p < zone.portals.size(); p++)
        {
            const Portal& portal = m_portals[zone.portals[p]];

            if (fabs(1 - fabs(portal.plane.y)) > math::eps) continue;

            Index opid = portal.zone[0] == leaf.zone ? portal.zone[1] : portal.zone[0];
            const Zone& opzone = m_zones[opid];

            if (opzone.type != zone.type) return fabs(portal.plane.xyz * pos + portal.plane.w);
        }
    }

    return 0;
}

void World::resetLeafFrameNum()
{
    for (Leaf& leaf : m_leafs) leaf.frame = 0;
}

} //namespace Render