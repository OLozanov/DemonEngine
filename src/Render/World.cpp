#include "World.h"
#include "Render/Light.h"

namespace Render
{

void World::setData(const std::vector<Render::Vertex>& vertices,
                    std::vector<Leaf>& leafs,
                    std::vector<Node>& nodes,
                    std::vector<Zone>& zones,
                    std::vector<Portal>& portals)
{
    m_vertexBuffer.setData(vertices.data(), vertices.size());

    m_leafs = std::move(leafs);
    m_nodes = std::move(nodes);
    m_zones = std::move(zones);
    m_portals = std::move(portals);
}

void World::reset()
{
    m_vertexBuffer.reset();

    m_leafs.clear();
    m_nodes.clear();
    m_zones.clear();
    m_portals.clear();
}

Index World::tracePos(const Node& node, const vec3& pos)
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

    if ((dist + r) >= 0)
    {
        if (node.right != InvalidIndex) traceObject(object, m_nodes[node.right], pos, bbox, axis);
    }

    if ((dist - r) < 0)
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

    if ((dist + r) >= 0)
    {
        if (node.right != InvalidIndex) traceLight(light, m_nodes[node.right], pos, bbox);
    }

    if ((dist - r) < 0)
    {
        if (node.left != InvalidIndex) traceLight(light, m_nodes[node.left], pos, bbox);
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

    if ((dist + r) >= 0)
    {
        if (node.right != InvalidIndex) traceFogVolume(volume, m_nodes[node.right], pos, bbox);
    }

    if ((dist - r) < 0)
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

void World::resetLeafFrameNum()
{
    for (Leaf& leaf : m_leafs) leaf.frame = 0;
}

} //namespace render