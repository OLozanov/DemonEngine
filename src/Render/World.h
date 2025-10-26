#pragma once

#include "math/math3d.h"

#include "Collision/Collision.h"

#include "Render/DisplayObject.h"
#include "Render/FogVolume.h"
#include "Render/Render.h"

#include "Utils/MutualReference.h"

#include "stdint.h"
#include <vector>

namespace Render
{

class Light;

using Index = uint32_t;
constexpr Index InvalidIndex = -1;

enum LeafFlag
{
    LeafSky = 1,
    AreaShadow = 2,
    RaytraceGI = 4
};

struct Node
{
    vec4 plane;

    Index leaf;

    Index left;
    Index right;
};

struct Leaf
{
    Index zone;
    Index pnum;

    uint8_t flags;

    BBox bbox;

    uint64_t frame;

    RefList<Light> lights;  //affecting lights
    bool globalLit = false;

    std::vector<Collision::CollisionPolygon> collisionPolygons;

    std::vector<DisplayData> regularGeometry;
    std::vector<DisplayData> transparentGeometry;
    std::vector<DisplayData> emissiveGeometry;
    std::vector<DisplayData> skyGeometry;
    //std::vector<ReflectionSurface> waterGeometry;

    GeometryData raytraceGeometry;
};

struct Portal
{
    vec4 plane;
    vec3 center;

    Index zone[2];

    std::vector<vec3> verts;

    Index bid = -1;
    std::vector<vec3> vbuff[2];
};

struct Zone
{
    uint8_t type;
    vec3 ambient;

    std::vector<Index> leafs;
    std::vector<Index> portals;

    RefList<DisplayObject> objects;
    RefList<FogVolume> fogVolumes;
};

class World
{
    mat4 m_mat;

    VertexBuffer m_vertexBuffer;

    std::vector<Leaf> m_leafs;
    std::vector<Node> m_nodes;
    std::vector<Zone> m_zones;
    std::vector<Portal> m_portals;

    Index tracePos(const Node& node, const vec3& pos);

    void traceObject(DisplayObject* object, Node& node, const vec3& pos, const vec3& bbox, const vec3* axis);
    void traceLight(Light* light, Node& node, const vec3& pos, const vec3& bbox);
    void traceFogVolume(FogVolume* volume, Node& node, const vec3& pos, const vec3& bbox);

public:

    void setData(const std::vector<Render::Vertex>& vertices,
                 std::vector<Leaf>& leafs, 
                 std::vector<Node>& nodes, 
                 std::vector<Zone>& zones,
                 std::vector<Portal>& portals);

    void reset();

    static const mat4& GlobalMat() 
    { 
        static mat4 identity = {};
        return identity; 
    }

    static const vec3& GlobalPos()
    {
        static vec3 pos = {};
        return pos;
    }
  
    const VertexData* vertexData() const { return m_vertexBuffer; }

    const std::vector<Leaf>& leafs() const { return m_leafs; }
    const std::vector<Node>& nodes() const { return m_nodes; }
    const std::vector<Zone>& zones() const { return m_zones; }
    const std::vector<Portal>& portals() const { return m_portals; }

    Leaf& leaf(size_t ind) { return m_leafs[ind]; }
    Node& node(size_t ind) { return m_nodes[ind]; }
    Zone& zone(size_t ind) { return m_zones[ind]; }
    Portal& portal(size_t ind) { return m_portals[ind]; }

    bool empty() { return m_nodes.empty(); }

    Index tracePos(const vec3& pos) { return tracePos(m_nodes[0], pos); }

    void addObject(DisplayObject* object);
    void addLight(Light* light);
    void addFogVolume(FogVolume* volume);

    void moveObject(DisplayObject* object);
    void moveLight(Light* light);
    void moveFogVolume(FogVolume* volume);

    void resetLeafFrameNum();
};

} //namespace render