#include "Path.h"

namespace UI
{

void Path::begin(bool closed)
{
    m_closed = closed;
    m_curpos = vec2{ 0.0f };

    m_loops.clear();
    m_vertices.clear();
    m_triangles.clear();
}

void Path::end()
{
    if (!m_loops.empty())
    {
        Loop& loop = m_loops.back();
        loop.last = m_vertices.size() - 1;
    }

    if (m_closed) build();
}

void Path::moveTo(float x, float y)
{
    size_t offset = m_vertices.size();
    
    if (!m_loops.empty())
    {
        Loop& loop = m_loops.back();
        loop.last = offset - 1;
    }

    m_loops.emplace_back(offset, offset);

    m_vertices.emplace_back(x, y);
}

void Path::lineTo(float x, float y)
{
    if (m_loops.empty())
    {
        m_loops.emplace_back(0, 0);
        m_vertices.emplace_back(0.0f);
    }

    m_vertices.emplace_back(x, y);
}

void Path::curveTo(const vec2& pt1, const vec2& pt2)
{
    if (m_loops.empty())
    {
        m_loops.emplace_back(0, 0);
        m_vertices.emplace_back(0.0f);
    }

    const vec2& pt0 = m_vertices.back();

    for (size_t i = 1; i <= 10; i++)
    {
        float t = i * 0.1f;
        float rt = 1.0f - i;

        vec2 a = pt0 * rt + pt1 * t;
        vec2 b = pt1 * rt + pt2 * t;
    
        vec2 pt = a * rt + b * t;

        m_vertices.push_back(pt);
    }
}

void Path::curveTo(const vec2& pt1, const vec2& pt2, const vec2& pt3)
{
    if (m_loops.empty())
    {
        m_loops.emplace_back(0, 0);
        m_vertices.emplace_back(0.0f);
    }

    const vec2& pt0 = m_vertices.back();

    for (size_t i = 1; i <= 10; i++)
    {
        float t = i * 0.1f;
        float rt = 1.0f - i;

        vec2 a = pt0 * rt + pt1 * t;
        vec2 b = pt1 * rt + pt2 * t;
        vec2 c = pt2 * rt + pt3 * t;

        vec2 d = a * rt + b * t;
        vec2 e = b * rt + c * t;

        vec2 pt = d * rt + e * t;

        m_vertices.push_back(pt);
    }
}

void Path::build()
{
    if (m_loops.empty()) return;

    m_edges.clear();
    m_trapezoids.clear();

    for (const Loop& loop : m_loops)
    {
        if (loop.last - loop.first < 2) continue;

        for (size_t i = loop.first; i <= loop.last; i++)
        {
            size_t k = (i == loop.last) ? loop.first : i + 1;

            const vec2& a = m_vertices[i];
            const vec2& b = m_vertices[k];

            if (fabs(a.y - b.y) < math::eps) continue;

            VertexId v1 = a.y < b.y ? i : k;
            VertexId v2 = a.y < b.y ? k : i;

            float dx = (b.x - a.x) / (b.y - a.y);

            size_t edge = m_edges.size();
            m_edges.emplace_back(v1, v2, dx);

            m_vqueue.emplace(EventType::Start, m_vertices[v1].y, edge);
        }
    }

    if (m_edges.empty()) return;

    tesselate();

    for (const Trapezoid& trap : m_trapezoids)
    {
        const Edge& left = m_edges[trap.left];
        const Edge& right = m_edges[trap.right];

        float x1 = m_vertices[left.v1].x + (trap.top - m_vertices[left.v1].y) * left.dx;
        float x2 = m_vertices[left.v1].x + (trap.bottom - m_vertices[left.v1].y) * left.dx;

        float x3 = m_vertices[right.v1].x + (trap.top - m_vertices[right.v1].y) * right.dx;
        float x4 = m_vertices[right.v1].x + (trap.bottom - m_vertices[right.v1].y) * right.dx;

        //if (x2 > x4) std::swap(x2, x4);

        m_triangles.emplace_back(x1, trap.top);
        m_triangles.emplace_back(x3, trap.top);
        m_triangles.emplace_back(x4, trap.bottom);

        m_triangles.emplace_back(x4, trap.bottom);
        m_triangles.emplace_back(x2, trap.bottom);
        m_triangles.emplace_back(x1, trap.top);
    }
}

bool Path::less(EdgeId e1, EdgeId e2)
{
    const Edge& edge1 = m_edges[e1];
    const Edge& edge2 = m_edges[e2];

    float x1 = m_vertices[edge1.v1].x + (m_cury - m_vertices[edge1.v1].y) * edge1.dx;
    float x2 = m_vertices[edge2.v1].x + (m_cury - m_vertices[edge2.v1].y) * edge2.dx;

    if (fabs(x1 - x2) < math::eps)
        return edge1.dx < edge2.dx;
    else
        return x1 < x2;
}

void Path::insertEdge(EdgeId edge)
{
    if (m_sedges == InvalidId)
    {
        m_sedges = edge;
        return;
    }

    EdgeId cur = m_sedges;

    while (true)
    {
        EdgeId next = m_edges[cur].next;

        if (less(cur, edge))
        {
            if (next == InvalidId)
            {
                m_edges[cur].next = edge;
                m_edges[edge].prev = cur;

                return;
            }
            else
            {
                cur = next;
                continue;
            }
        }

        EdgeId prev = m_edges[cur].prev;

        m_edges[cur].prev = edge;
        m_edges[edge].next = cur;
        m_edges[edge].prev = prev;

        if (prev != InvalidId)
            m_edges[prev].next = edge;
        else
            m_sedges = edge;

        return;
    }
}

void Path::removeEdge(EdgeId edge)
{
    EdgeId next = m_edges[edge].next;
    EdgeId prev = m_edges[edge].prev;

    if (next != InvalidId)
        m_edges[next].prev = prev;

    if (prev != InvalidId)
        m_edges[prev].next = next;
    else
        m_sedges = next;
}

void Path::swapEdges(EdgeId e1, EdgeId e2)  // Only for adjacent edges
{
    if (m_edges[e2].next == e1) std::swap(e1, e2);

    EdgeId next1 = m_edges[e1].next;
    EdgeId prev1 = m_edges[e1].prev;

    EdgeId next2 = m_edges[e2].next;
    EdgeId prev2 = m_edges[e2].prev;
      
    m_edges[e1].next = next2;
    m_edges[e1].prev = e2;
    m_edges[e2].next = e1;
    m_edges[e2].prev = prev1;

    if (prev1 != InvalidId)
    {
        m_edges[prev1].next = e2;

        if (m_edges[prev1].right == e1)
            m_edges[prev1].right = e2;
    }
    else
        m_sedges = e2;

    if (next2 != InvalidId)
        m_edges[next2].prev = e1;

    if (m_edges[e1].right == e2)
    {
        m_edges[e1].right = InvalidId;
        m_edges[e2].right = e1;
    }
   
    if (m_edges[e2].right != InvalidId)
    {
        m_edges[e1].right = m_edges[e2].right;
        m_edges[e2].right = InvalidId;
    }
}

void Path::addTrapezoid(EdgeId eid, float bottom)
{
    Edge& edge = m_edges[eid];
    EdgeId rid = edge.right;

    if (rid != InvalidId)
    {
        m_trapezoids.emplace_back(edge.top, bottom, eid, rid);
        m_edges[eid].right = InvalidId;
    }
}

void Path::bindEdges(EdgeId left, float top)
{
    while (left != InvalidId)
    {
        if (m_edges[left].right != InvalidId) addTrapezoid(left, top);

        EdgeId right = m_edges[left].next;
        if (right == InvalidId) break;

        m_edges[left].right = right;
        m_edges[left].top = top;

        left = m_edges[right].next;
    }
}

bool Path::intersection(EdgeId e1, EdgeId e2, vec2& pt)
{
    const Edge& edge1 = m_edges[e1];
    const Edge& edge2 = m_edges[e2];

    const vec2& a = m_vertices[edge1.v1];
    const vec2& b = m_vertices[edge1.v2];
    const vec2& c = m_vertices[edge2.v1];
    const vec2& d = m_vertices[edge2.v2];

    if (b.x + math::eps < c.x) return false;
    if (a.x - math::eps > d.x) return false;

    if (b.y + math::eps < c.y) return false;
    if (a.y - math::eps > d.y) return false;

    float det = (a.x - b.x) * (c.y - d.y) - (a.y - b.y) * (c.x - d.x);

    if (fabs(det) < math::eps) return false;

    float n1 = (a.x - c.x) * (c.y - d.y) - (a.y - c.y) * (c.x - d.x);
    float n2 = (a.x - c.x) * (a.y - c.y) - (a.y - c.y) * (a.x - c.x);

    float t = n1 / det;
    float u = n2 / det;

    if (t < 0.0f || t > 1.0f) return false;
    if (u < 0.0f || u > 1.0f) return false;

    pt = a + (b - a) * t;

    return true;
}

void Path::tesselate()
{
    m_sedges = InvalidId;
    m_curedge = InvalidId;

    EdgeId stopped = InvalidId;

    while (!m_vqueue.empty())
    {
        VertEvent event = m_vqueue.top();
        m_vqueue.pop();

        if (fabs(m_cury - event.y) > math::eps)
        {
            for (EdgeId edge = stopped; edge != InvalidId; edge = m_edges[edge].next)
                if (m_edges[edge].right != InvalidId)
                {
                    VertexId v2 = m_edges[edge].v2;
                    addTrapezoid(edge, m_vertices[v2].y);
                }

            bindEdges(m_sedges, m_cury);
            m_cury = event.y;
        }

        Edge& edge = m_edges[event.edge];

        switch (event.type)
        {
        case EventType::Start:
        {
            insertEdge(event.edge);
            m_vqueue.emplace(EventType::End, m_vertices[edge.v2].y, event.edge);

            vec2 pt;

            EdgeId next = m_edges[event.edge].next;
            EdgeId prev = m_edges[event.edge].prev;

            if (prev != InvalidId && intersection(event.edge, prev, pt))
                m_vqueue.emplace(EventType::Intersect, pt.y, prev);

            if (next != InvalidId && intersection(event.edge, next, pt))
                m_vqueue.emplace(EventType::Intersect, pt.y, event.edge);
        }
        break;
        case EventType::End:
            removeEdge(event.edge);

            if (edge.right != InvalidId)
            {
                edge.next = stopped;
                edge.prev = InvalidId;
                
                if (stopped != InvalidId) m_edges[stopped].prev = event.edge;
                stopped = event.edge;
            }
        break;
        case EventType::Intersect:
            for (EdgeId edge = event.edge; edge != InvalidId; edge = m_edges[edge].next)
                if (m_edges[edge].right != InvalidId)
                {
                    VertexId v2 = m_edges[edge].v2;
                    addTrapezoid(edge, event.y);
                }

            swapEdges(event.edge, m_edges[event.edge].next);
        break;
        }
    }

    for (EdgeId edge = stopped; edge != InvalidId; edge = m_edges[edge].next)
        if (m_edges[edge].right != InvalidId)
        {
            VertexId v2 = m_edges[edge].v2;
            addTrapezoid(edge, m_vertices[v2].y);
        }
}

} //namespace UI