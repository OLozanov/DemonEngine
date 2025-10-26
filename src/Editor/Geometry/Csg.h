#pragma once

// CSG -- Constructive Solid Geometry

#include "Geometry.h"

#include "math/math3d.h"
#include "stdint.h"

using NodeIndex = size_t;

struct CsgNode
{
    vec4 plane;

    EditPolygon leaf;

    NodeIndex left;
    NodeIndex right;
};

class CsgTree
{
public:

    bool empty();

    void build(const PolygonList& polygons);
    void reset();
 
    void clipPolygons(bool negative, bool addativeBlock, const PolygonList& in, PolygonList& out) const;
    void categorizePolygons(bool negative, bool addativeBlock, const PolygonList& in, PolygonList& out) const;

private:

    void buildTree(NodeIndex nodeId, const PolygonList& polygons);
    void clipPolygons(NodeIndex nodeId, bool negative, bool addativeBlock, const PolygonList& in, PolygonList& out) const;
    void categorizePolygons(NodeIndex nodeId, bool negative, bool addativeBlock, const PolygonList& in, PolygonList& out) const;

    static void SetStatus(PolygonList& polys, VolumeStatus status);

    NodeIndex allocateNode();

    std::vector<CsgNode> m_nodes;

    static constexpr NodeIndex InvalidIndex = -1;
};