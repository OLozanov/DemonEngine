#pragma once

#include <vector>
#include "math/math3d.h"

#include <queue>

namespace UI
{

class Canvas;

class Path
{
public:
    using VertexId = size_t;

private:
    using NodeId = size_t;
    using EdgeId = size_t;
    using TrapId = size_t;

    static constexpr NodeId InvalidId = -1;
    static constexpr NodeId GraphSource = 0;

    struct Loop
    {
        size_t first;
        size_t last;
    };

    enum class EventType 
    { 
        Start,
        End,
        Intersect
    };

    struct VertEvent
    {
        EventType type;
        float y;
        
        EdgeId edge;

        bool operator<(const VertEvent& e) const { return e.y < y; }
    };

    struct Edge
    {
        VertexId v1;
        VertexId v2;

        float dx;
        float top;

        EdgeId right = InvalidId;

        EdgeId next = InvalidId;
        EdgeId prev = InvalidId;
    };

    struct Trapezoid
    {
        float top;
        float bottom;

        EdgeId left;
        EdgeId right;
    };

    friend class UI::Canvas;

public:
    void begin(bool closed = true);
    void end();

    void moveTo(float x, float y);
    void lineTo(float x, float y);

    void curveTo(const vec2& pt1, const vec2& pt2);                     // Quadratic Bezier
    void curveTo(const vec2& pt1, const vec2& pt2, const vec2& pt3);    // Cubiv Bezier

    bool closed() const { return m_closed; }

    const std::vector<vec2>& vertices() const { return m_vertices; }
    const std::vector<Loop>& loops() const { return m_loops; }
    const std::vector<vec2>& triangles() const { return m_triangles; }

private:
    void build();

    bool less(EdgeId e1, EdgeId e2);

    void insertEdge(EdgeId edge);
    void removeEdge(EdgeId edge);
    void swapEdges(EdgeId e1, EdgeId e2);
    void addTrapezoid(EdgeId eid, float bottom);
    void bindEdges(EdgeId edge, float top);
    bool intersection(EdgeId e1, EdgeId e2, vec2& pt);

    void tesselate();

private:
    
    std::vector<vec2> m_vertices;
    //std::vector<VertexId> m_triangles;   // triangle list
    std::vector<vec2> m_triangles;

    std::vector<Loop> m_loops;

    bool m_closed;
    vec2 m_curpos;

    std::priority_queue<VertEvent> m_vqueue;
    std::vector<Edge> m_edges;
    std::vector<Trapezoid> m_trapezoids;

    // Scanline
    float m_cury;
    EdgeId m_sedges;
    EdgeId m_curedge;
};

} //namespace UI
