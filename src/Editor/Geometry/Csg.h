#pragma once

// CSG -- Constructive Solid Geometry

#include "Geometry.h"

#include "math/math3d.h"
#include "stdint.h"

class Block;

using NodeIndex = size_t;

struct CsgPolygon
{
    vec4 plane;
    std::vector<vec3> vertices;
};

struct CsgNode
{
    vec4 plane;

    NodeIndex left;
    NodeIndex right;
};

class CsgTree
{
public:

    bool empty();

    void build(const Block& block);
    void reset();
 
    void clipPolygons(bool negative, bool addativeBlock, const PolygonList& in, PolygonList& out) const;
    void categorizePolygons(bool negative, bool addativeBlock, const PolygonList& in, PolygonList& out) const;

private:

    void buildTree(NodeIndex nodeId, const std::vector<CsgPolygon>& polygons);
    void clipPolygons(NodeIndex nodeId, bool negative, bool addativeBlock, const PolygonList& in, PolygonList& out) const;
    void categorizePolygons(NodeIndex nodeId, bool negative, bool addativeBlock, const PolygonList& in, PolygonList& out) const;

    static void SetStatus(PolygonList& polys, VolumeStatus status);

    NodeIndex allocateNode();

    std::vector<CsgNode> m_nodes;

    static constexpr NodeIndex InvalidIndex = -1;
};