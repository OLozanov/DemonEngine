#include "Csg.h"
#include "Geometry/Block.h"

bool CsgTree::empty()
{
    return m_nodes.empty();
}

void CsgTree::build(const Block& block)
{
    size_t pnum = block.polygonsNum();

    if (!pnum) return;

    m_nodes.clear();
    m_nodes.reserve(pnum);

    std::vector<CsgPolygon> polygons(pnum);

    for (size_t i = 0; i < pnum; i++)
    {
        std::vector<vec3> verts = block.polygonVertices(i);

        vec3 v1 = verts[0];
        vec3 v2 = verts[1];
        vec3 v3 = verts[2];

        vec3 a = v2 - v1;
        vec3 b = v3 - v1;

        vec3 norm = a ^ b;
        norm.normalize();
        float dist = -(v1 * norm);

        polygons[i].plane = { norm, dist };
        polygons[i].vertices = std::move(verts);
    }

    NodeIndex root = allocateNode();
    buildTree(root, polygons);
}

void CsgTree::reset()
{
    m_nodes.clear();
}

void CsgTree::buildTree(NodeIndex nodeId, const std::vector<CsgPolygon>& polygons)
{
    const CsgPolygon& splitter = polygons[0];

    CsgNode& node = m_nodes[nodeId];
    node.plane = splitter.plane;

    node.left = InvalidIndex;
    node.right = InvalidIndex;

    std::vector<CsgPolygon> left;
    std::vector<CsgPolygon> right;

    for (int i = 0; i < polygons.size(); i++)
    {
        if (i == 0) continue;

        PolyType ptype = ClassifyPolygon(splitter.plane, polygons[i].vertices);

        switch (ptype)
        {
        case PolyType::Back:
            left.push_back(polygons[i]);
        break;

        case PolyType::Front:
        case PolyType::Plane:
            right.push_back(polygons[i]);
        break;

        case PolyType::Split:
        {
            std::vector<vec3> vleft;
            std::vector<vec3> vright;

            SplitPoly(splitter.plane, polygons[i].vertices, vleft, vright);

            left.push_back({polygons[i].plane, std::move(vleft)});
            right.push_back({polygons[i].plane, std::move(vright)});
        }
        break;
        }
    }

    if (!left.empty())
    {
        node.left = allocateNode();
        buildTree(node.left, left);
    
    } else node.left = InvalidIndex;

    if (!right.empty())
    {
        node.right = allocateNode();
        buildTree(node.right, right);
    
    } else node.right = InvalidIndex;
}

void CsgTree::clipPolygons(bool negative, bool addativeBlock, const PolygonList& in, PolygonList& out) const
{
    clipPolygons(0, negative, addativeBlock, in, out);
}

void CsgTree::categorizePolygons(bool negative, bool addativeBlock, const PolygonList& in, PolygonList& out) const
{
    categorizePolygons(0, negative, addativeBlock, in, out);

    for (EditPolygon& poly : out) poly.border = false;
}

void CsgTree::clipPolygons(NodeIndex nodeId, bool negative, bool addativeBlock, const PolygonList& in, PolygonList& out) const
{
    const CsgNode& node = m_nodes[nodeId];

    PolygonList left;
    PolygonList right;

    for (int i = 0; i < in.size(); i++)
    {
        PolyType ptype = ClassifyPolygon(node.plane, in[i].vertices);

        const EditPolygon& poly = in[i];

        switch (ptype)
        {
        case PolyType::Back:
            left.push_back(poly);
        break;

        case PolyType::Front:
            right.push_back(poly);
        break;

        case PolyType::Plane:
            if (negative)
            {
                float c = poly.plane.xyz * node.plane.xyz;
                
                if (c > 0 && !addativeBlock) right.push_back(poly);
                else left.push_back(poly);

            }
            else left.push_back(poly);
        break;

        case PolyType::Split:
        {
            VertexList vleft;
            VertexList vright;

            SplitPoly(node.plane, poly.vertices, vleft, vright);

            left.push_back({poly.origin, poly.plane, poly.status, poly.splitter, false, poly.flags, poly.material, std::move(vleft) });
            right.push_back({poly.origin, poly.plane, poly.status, poly.splitter, false, poly.flags, poly.material, std::move(vright) });
        }
        break;
        }
    }

    if (!left.empty())
    {
        if (node.left != InvalidIndex) clipPolygons(node.left, negative, addativeBlock, left, out);
    }

    if (!right.empty())
    {
        if (node.right != InvalidIndex) clipPolygons(node.right, negative, addativeBlock, right, out);
        else out.insert(out.begin(), right.begin(), right.end());
    }
}

void CsgTree::categorizePolygons(NodeIndex nodeId, bool negative, bool addativeBlock, const PolygonList& in, PolygonList& out) const
{
    const CsgNode& node = m_nodes[nodeId];

    PolygonList left;
    PolygonList right;

    for (int i = 0; i < in.size(); i++)
    {
        PolyType ptype = ClassifyPolygon(node.plane, in[i].vertices);

        const EditPolygon& poly = in[i];

        switch (ptype)
        {
        case PolyType::Back:
            left.push_back(poly);
        break;

        case PolyType::Front:
            right.push_back(poly);
        break;

        case PolyType::Plane:
            if (negative)
            {
                float c = poly.plane.xyz * node.plane.xyz;

                if (addativeBlock)
                {
                    if (c > 0) right.push_back(poly);
                    else left.push_back(poly);
                
                }
                else
                {
                    left.push_back(poly);
                    if (c > 0) left.back().border = true;
                }
            } 
            else
            {
                float c = poly.plane.xyz * node.plane.xyz;

                if (poly.status == VolumeStatus::Border)
                {
                    left.push_back(poly);
                }
                else //if (addativeBlock)
                {
                    if (c > 0) right.push_back(poly);
                    else left.push_back(poly);
                }
                /*else
                {
                    if (c > 0)
                        right.push_back(poly);
                    else
                        left.push_back(poly);
                }*/
            }
        break;

        case PolyType::Split:
        {
            VertexList vleft;
            VertexList vright;

            SplitPoly(node.plane, poly.vertices, vleft, vright);

            left.push_back({poly.origin, poly.plane, poly.status, poly.splitter, poly.border, poly.flags, poly.material, std::move(vleft) });
            right.push_back({poly.origin, poly.plane, poly.status, poly.splitter, poly.border, poly.flags, poly.material, std::move(vright) });
        }
        break;
        }
    }

    if (!left.empty())
    {
        if (node.left != InvalidIndex) categorizePolygons(node.left, negative, addativeBlock, left, out);
        else
        {
            SetStatus(left, negative ? VolumeStatus::Outside : VolumeStatus::Inside);
            out.insert(out.begin(), left.begin(), left.end());
        }
    }

    if (!right.empty())
    {
        if (node.right != InvalidIndex) categorizePolygons(node.right, negative, addativeBlock, right, out);
        else
        {
            //SetStatus(right, negative ? VolumeStatus::Inside : VolumeStatus::Outside);
            out.insert(out.begin(), right.begin(), right.end());
        }
    }
}

NodeIndex CsgTree::allocateNode()
{
    m_nodes.emplace_back();
    return m_nodes.size() - 1;
}

void CsgTree::SetStatus(PolygonList& polys, VolumeStatus status)
{
    for (EditPolygon& poly : polys)
    {
        if (poly.border)
            poly.status = VolumeStatus::Border;
        else
            poly.status = status;       
    }
}