#include "Map.h"

#include <map>

void Map::cleanup()
{
    m_portals.clear();
    m_zonePortals.clear();
    m_zones.clear();
    m_leafs.clear();
    m_nodes.clear();

    m_portalGeometry.clear();

    m_vertices.clear();
}

void Map::build(PolygonList& polygons)
{
    cleanup();

    if (polygons.empty()) return;

    buildTree(polygons);
    portalize();

    generateZones();
    collectZonePortals();

    buildPortlaGeometry();
}

void Map::CalcBBox(const PolygonList& plg, BBox& bbox)
{
    bool first = true;

    for (const EditPolygon& poly : plg)
    {
        for (const Vertex& vert : poly.vertices)
        {
            if (first)
            {
                first = false;

                bbox.min = vert.position;
                bbox.max = vert.position;
            }
            else
            {
                if (vert.position.x < bbox.min.x) bbox.min.x = vert.position.x;
                if (vert.position.y < bbox.min.y) bbox.min.y = vert.position.y;
                if (vert.position.z < bbox.min.z) bbox.min.z = vert.position.z;

                if (vert.position.x > bbox.max.x) bbox.max.x = vert.position.x;
                if (vert.position.y > bbox.max.y) bbox.max.y = vert.position.y;
                if (vert.position.z > bbox.max.z) bbox.max.z = vert.position.z;
            }
        }
    }
}

bool Map::selectSplitter(const PolygonList& plg, size_t& splitterId, bool& zonePortal)
{
    constexpr int SplitRate = 15;
    constexpr int DiffRate = 1;
    constexpr int PlaneRate = -5;
    constexpr int PortalFactor = 2;

    bool splitter = false;
    zonePortal = false;  //choose only from zone portals;

    int sprate;

    //choose splitter
    for (size_t polyId = 0; polyId < plg.size(); polyId++)
    {
        const EditPolygon& poly = plg[polyId];

        if (poly.splitter) continue;

        //Zone portals have largest priority
        if (zonePortal)
        {
            if (!(poly.flags & PolyZonePortal)) continue;
        }
        else
        {
            if (poly.flags & PolyZonePortal)
            {
                zonePortal = true;
                splitter = false;
            }
        }

        int split_num = 0;
        int back_num = 0;
        int front_num = 0;
        int plane_num = 0;
        int portal_num = 0;

        //test all polygons
        for (size_t testId = 0; testId < plg.size(); testId++)
        {
            const EditPolygon& testpoly = plg[testId];

            if (testId == polyId) continue;

            PolyType ptype = ClassifyPolygon(poly.plane, testpoly.vertices);

            switch (ptype)
            {
            case PolyType::Back: back_num++; break;
            case PolyType::Front: front_num++; break;

            case PolyType::Split:
                split_num++;

                if (testpoly.flags & PolyZonePortal) split_num += PortalFactor;

                break;

            case PolyType::Plane: plane_num++; break;
            }
        }

        int rate = abs(front_num - back_num) * DiffRate + split_num * SplitRate + plane_num * PlaneRate;

        if (!splitter)
        {
            sprate = rate;
            splitterId = polyId;

            splitter = true;
        }
        else if (rate < sprate)
        {
            sprate = rate;
            splitterId = polyId;
        }
    }

    return splitter;
}

MapIndex Map::buildTree(PolygonList& plg)
{
    size_t splitterId;
    bool zportals;

    m_nodes.emplace_back();
    MapIndex nodeId = m_nodes.size() - 1;

    MapNode& node = m_nodes[nodeId];

    CalcBBox(plg, node.bbox);

    if (!selectSplitter(plg, splitterId, zportals))
    {
        m_leafs.emplace_back();
        MapIndex leafId = m_leafs.size() - 1;
        node.leaf = leafId;
        node.left = InvalidMapIndex;
        node.right = InvalidMapIndex;

        MapLeaf& leaf = m_leafs[leafId];

        leaf.polygons = std::move(plg);
        leaf.bbox = node.bbox;
        leaf.zone = InvalidMapIndex;
        leaf.flags = 0;

        return nodeId;
    }

    EditPolygon& splitter = plg[splitterId];

    node.plane = splitter.plane;
    node.leaf = InvalidMapIndex;
    splitter.splitter = true;

    PolygonList frontList;
    PolygonList backList;

    for (size_t polyId = 0; polyId < plg.size(); polyId++)
    {
        EditPolygon& poly = plg[polyId];

        if (polyId == splitterId)
        {
            if (zportals) addZonePortal(poly);
            else frontList.emplace_back(std::move(poly));

            continue;
        }

        PolyType ptype = ClassifyPolygon(splitter.plane, poly.vertices);

        switch (ptype)
        {
        case PolyType::Back:
            backList.emplace_back(poly);
        break;

        case PolyType::Front:
            frontList.emplace_back(poly);
        break;

        case PolyType::Plane:

            if (poly.flags & PolyZonePortal) addZonePortal(poly);
            else
            {
                float sg = (poly.plane.xyz) * (splitter.plane.xyz);

                if (sg > 0) 
                    frontList.emplace_back(poly);
                else 
                    backList.emplace_back(poly);
            }

        break;

        case PolyType::Split:
        {
            VertexList vleft;
            VertexList vright;

            SplitPoly(splitter.plane, poly.vertices, vleft, vright);

            backList.push_back({ nullptr, poly.plane, {}, poly.splitter, false, poly.flags, poly.material, std::move(vleft) });
            frontList.push_back({ nullptr, poly.plane, {}, poly.splitter, false, poly.flags, poly.material, std::move(vright) });
        }
        break;
        }
    }

    //Build sub trees
    if (!backList.empty())
        m_nodes[nodeId].left = buildTree(backList);
    else 
        m_nodes[nodeId].left = InvalidMapIndex;

    if (!frontList.empty()) 
        m_nodes[nodeId].right = buildTree(frontList);
    else 
        m_nodes[nodeId].right = InvalidMapIndex;

    return nodeId;
}

void Map::getPortalBounds(const Portal& portal, vec3& min, vec3& max)
{
    bool first = true;

    for (const vec3& vert : portal.vertices)
    {
        if (first)
        {
            first = false;

            max = vert;
            min = vert;
        }
        else
        {
            if (min.x > vert.x) min.x = vert.x;
            if (min.y > vert.y) min.y = vert.y;
            if (min.z > vert.z) min.z = vert.z;

            if (max.x < vert.x) max.x = vert.x;
            if (max.y < vert.y) max.y = vert.y;
            if (max.z < vert.z) max.z = vert.z;
        }
    }
}

Portal Map::generatePortal(const MapNode& node)
{
    Portal portal;

    portal.plane = node.plane;
    portal.zonePortal = false;

    portal.leafnum = 0;
    portal.owner[0] = InvalidMapIndex;
    portal.owner[1] = InvalidMapIndex;

    vec3 mid;

    mid.x = (node.bbox.max.x + node.bbox.min.x) / 2;
    mid.y = (node.bbox.max.y + node.bbox.min.y) / 2;
    mid.z = (node.bbox.max.z + node.bbox.min.z) / 2;

    vec3 pt = mid - node.plane.xyz * (mid * node.plane.xyz + node.plane.w);

    vec3 v = { 0, 0, 0 };

    if (fabs(node.plane.y) > fabs(node.plane.z))
    {
        if (fabs(node.plane.z) < fabs(node.plane.x)) v.z = 1;
        else v.x = 1;
    }
    else
    {
        if (fabs(node.plane.y) <= fabs(node.plane.x)) v.y = 1;
        else v.x = 1;
    }

    vec3 s = node.plane.xyz ^ v;
    s.normalize();

    vec3 t = node.plane.xyz ^ s;
    t.normalize();

    vec3 dif = node.bbox.max - node.bbox.min;
    float size = dif.length() / 2;

    s = s * size;
    t = t * size;

    portal.vertices.resize(4);

    portal.vertices[0] = pt + s - t;
    portal.vertices[1] = pt + s + t;
    portal.vertices[2] = pt - s + t;
    portal.vertices[3] = pt - s - t;

    return portal;
}

void Map::clipPortal(Portal& portal, MapIndex nodeId, std::list<Portal>& plist)
{
    MapNode& node = m_nodes[nodeId];

    if (node.leaf != InvalidMapIndex)
    {
        if (portal.leafnum < 2)
        {
            portal.owner[portal.leafnum] = node.leaf;
            portal.leafnum++;

            plist.emplace_back(std::move(portal));
        }

        return;
    }

    PolyType ptype = ClassifyPolygon(node.plane, portal.vertices);

    switch (ptype)
    {
    case PolyType::Back:

        if (node.left != InvalidMapIndex) clipPortal(portal, node.left, plist);

    break;

    case PolyType::Front:

        if (node.right != InvalidMapIndex) clipPortal(portal, node.right, plist);

    break;

    case PolyType::Split:
    {
        std::vector<vec3> backVerts;
        std::vector<vec3> frontVerts;

        Portal front;

        SplitPoly(node.plane, portal.vertices, backVerts, frontVerts);

        if (node.left != InvalidMapIndex)
        {
            Portal back;

            back.plane = portal.plane;

            back.zonePortal = portal.zonePortal;

            back.vertices = std::move(backVerts);

            back.leafnum = portal.leafnum;
            back.owner[0] = portal.owner[0];
            back.owner[1] = portal.owner[1];

            clipPortal(back, node.left, plist);
        }

        std::swap(portal.vertices, frontVerts);

        clipPortal(portal, node.right, plist);
    }
    break;

    case PolyType::Plane:
    {
        std::list<Portal> clipList;

        clipPortal(portal, node.right, clipList);

        if (clipList.empty()) break;

        if (node.left == InvalidMapIndex)
        {
            plist.insert(plist.end(), std::make_move_iterator(clipList.begin()), std::make_move_iterator(clipList.end()));
        }
        else
        {
            for (Portal& prt : clipList) clipPortal(prt, node.left, plist);
        }
    }
    break;
    }
}

bool Map::isPortalDuplicate(Portal& portal)
{
    vec3 max, min;

    getPortalBounds(portal, min, max);

    vec3 diff = max - min;
    float d1 = diff.length();

    for (auto it = m_portals.begin(); it != m_portals.end(); )
    {
        auto cur = it++;
        const Portal& tportal = *cur;

        if ((portal.owner[0] == tportal.owner[0]) &&
            (portal.owner[1] == tportal.owner[1]))
        {
            getPortalBounds(tportal, min, max);

            diff = max - min;
            float d2 = diff.length();

            if (d1 > d2)
            {
                if (tportal.zonePortal && !portal.zonePortal) portal.zonePortal = true;

                m_portals.erase(cur);

            } else return true;
        }
    }

    return false;
}

void Map::addZonePortal(const EditPolygon& poly)
{
    std::vector<vec3> verts;
    verts.reserve(poly.vertices.size());

    for (const Vertex& vert : poly.vertices) verts.push_back(vert.position);

    m_zonePortals.push_back({ poly.plane, true, 0, std::move(verts), 0, {0, 0} });
}

void Map::zonePortals()
{
    for (Portal& portal : m_zonePortals)
    {
        std::list<Portal> cportals;

        clipPortal(portal, TreeRoot, cportals);

        for (Portal& cportal : cportals)
        {
            if (cportal.leafnum == 2)
            {
                if (!isPortalDuplicate(cportal))
                {
                    m_portals.emplace_back(std::move(cportal));
                }
            }
        }
    }
}

void Map::portalize()
{
    zonePortals();

    for (MapNode& node : m_nodes)
    {
        if (node.leaf != InvalidMapIndex) continue;

        Portal portal = generatePortal(node);

        std::list<Portal> cportals;

        clipPortal(portal, TreeRoot, cportals);

        for (Portal& portal : cportals)
        {
            if (portal.leafnum == 2)
            {
                if (!isPortalDuplicate(portal)) m_portals.emplace_back(std::move(portal));
            }
        }
    }

    for (Portal& portal : m_portals)
    {
        m_leafs[portal.owner[0]].portals.push_back(&portal);
        m_leafs[portal.owner[1]].portals.push_back(&portal);
    }
}

void Map::collectZonePortals()
{
    m_zonePortals.clear();
    m_zonePortalNum = 0;

    for (Portal& portal : m_portals)
    {
        if (portal.zonePortal)
        {
            portal.zonePortalId = m_zonePortalNum++;
            m_zonePortals.push_back(portal);
        }
    }

    for (Zone& zone : m_zones)
    {
        for (MapIndex leafId : zone.leafs)
        {
            const MapLeaf& leaf = m_leafs[leafId];

            for (Portal* portal : leaf.portals)
            {
                if (portal->zonePortal) zone.portals.push_back(portal->zonePortalId);
            }
        }
    }
}

void Map::collectLeafs(MapIndex zoneId, MapIndex leafId)
{
    Zone& zone = m_zones[zoneId];
    MapLeaf& leaf = m_leafs[leafId];

    for (Portal* portal : leaf.portals)
    {
        if (portal->zonePortal) continue;

        //Get opposed leaf
        MapIndex opleafId = portal->owner[0];
        if (opleafId == leafId) opleafId = portal->owner[1];

        MapLeaf& opleaf = m_leafs[opleafId];

        if (opleaf.zone != InvalidMapIndex) continue;

        //Add leaf to zone
        zone.leafs.push_back(opleafId);
        opleaf.zone = zoneId;

        collectLeafs(zoneId, opleafId);
    }
}

void Map::generateZones()
{
    for (int leafId = 0; leafId < m_leafs.size(); leafId++)
    {
        MapLeaf& leaf = m_leafs[leafId];

        if (leaf.zone != InvalidMapIndex) continue;

        m_zones.emplace_back();
        MapIndex zoneId = m_zones.size() - 1;

        m_zones[zoneId].type = ZoneType::Default;
        m_zones[zoneId].leafs.push_back(leafId);

        leaf.zone = zoneId;

        collectLeafs(zoneId, leafId);
    }
}

void Map::buildPortlaGeometry()
{
    for (const Portal& portal : m_portals)
    {
        for (int i = 0; i < portal.vertices.size(); i++)
        {
            size_t v = (i % 2 == 0) ? portal.vertices.size() - 1 - i / 2 : i / 2;
            m_portalGeometry.push(portal.vertices[v]);
        }
    }
}

void Map::generateDisplayData()
{
    m_vertices.clear();

    uint32_t offset = 0;
    uint32_t rtxOffset = 0;

    for (MapLeaf& leaf : m_leafs)
    {
        leaf.displayData.clear();

        std::map<Material*, std::vector<MapIndex>> polygons;

        //Sort polygons by texture
        for (int p = 0; p < leaf.polygons.size(); p++)
        {
            EditPolygon& poly = leaf.polygons[p];

            if (poly.flags & PolyInvisible) continue;

            if (poly.flags & PolySky)
            {
                leaf.flags = 1;
                continue;
            }

            polygons[poly.material].push_back(p);
        }

        uint32_t rtxVNum = 0;

        for (const auto& polylist : polygons)
        {
            uint32_t vnum = 0;

            for (MapIndex index : polylist.second)
            {
                EditPolygon& poly = leaf.polygons[index];

                uint32_t triangles = poly.vertices.size() - 2;

                for (int i = 0; i < triangles; i++)
                {
                    m_vertices.push_back(poly.vertices[0]);
                    m_vertices.push_back(poly.vertices[i + 1]);
                    m_vertices.push_back(poly.vertices[i + 2]);
                    vnum += 3;
                }

                if (!(poly.flags & PolySky || poly.flags & PolyTransparent || poly.flags & PolyWater))
                {
                    for (int i = 0; i < triangles; i++)
                    {
                        m_rtxVertices.push_back(poly.vertices[0].position);
                        m_rtxVertices.push_back(poly.vertices[i + 1].position);
                        m_rtxVertices.push_back(poly.vertices[i + 2].position);
                        rtxVNum += 3;
                    }
                }
            }

            leaf.displayData.push_back({ polylist.first, offset, vnum });
            offset += vnum;
        }

        leaf.rtxGeometry = { rtxOffset, rtxVNum };
        rtxOffset += rtxVNum;
    }
}

void Map::writeVertices(FILE* file)
{
    uint32_t vnum = m_vertices.size();

    fwrite(&vnum, sizeof(uint32_t), 1, file);
    fwrite(m_vertices.data(), sizeof(Vertex), vnum, file);

    uint32_t rtxvnum = m_rtxVertices.size();

    fwrite(&rtxvnum, sizeof(uint32_t), 1, file);
    fwrite(m_rtxVertices.data(), sizeof(vec3), vnum, file);
}

void Map::writeLeafs(FILE* file)
{
    uint32_t lnum = m_leafs.size();
    fwrite(&lnum, sizeof(uint32_t), 1, file);

    for (const MapLeaf& leaf : m_leafs)
    {
        fwrite(&leaf.bbox.min, sizeof(vec3), 1, file);
        fwrite(&leaf.bbox.max, sizeof(vec3), 1, file);

        fwrite(&leaf.zone, sizeof(MapIndex), 1, file);
        fwrite(&leaf.flags, sizeof(uint8_t), 1, file);

        // display data
        uint32_t dnum = leaf.displayData.size();
        fwrite(&dnum, sizeof(uint32_t), 1, file);

        for (const Render::DisplayData& elem : leaf.displayData)
        {
            if (elem.material)
            {
                const std::string& matname = elem.material->name;
                uint32_t mlen = matname.size();

                fwrite(&mlen, sizeof(uint32_t), 1, file);
                fwrite(matname.c_str(), 1, mlen, file);
            }
            else
            {
                uint32_t mlen = 0;
                fwrite(&mlen, sizeof(uint32_t), 1, file);
            }

            fwrite(&elem.offset, sizeof(uint32_t), 1, file);
            fwrite(&elem.vertexnum, sizeof(uint32_t), 1, file);
        }

        // Raytracing data
        fwrite(&leaf.rtxGeometry.offset, sizeof(uint32_t), 1, file);
        fwrite(&leaf.rtxGeometry.count, sizeof(uint32_t), 1, file);
        
        // collision polygons
        uint32_t polynum = leaf.polygons.size();
        fwrite(&polynum, sizeof(uint32_t), 1, file);

        for (const EditPolygon& poly : leaf.polygons)
        {
            uint32_t vnum = poly.vertices.size();
            fwrite(&vnum, sizeof(uint32_t), 1, file);

            for (const Vertex& vert : poly.vertices)
                fwrite(&vert.position, sizeof(vec3), 1, file);
        }
    }
}

void Map::writeNodes(FILE* file)
{
    uint32_t nnum = m_nodes.size();
    fwrite(&nnum, sizeof(uint32_t), 1, file);

    for (const MapNode& node : m_nodes)
    {
        fwrite(&node.plane, sizeof(vec4), 1, file);
        fwrite(&node.leaf, sizeof(MapIndex), 1, file);
        fwrite(&node.left, sizeof(MapIndex), 1, file);
        fwrite(&node.right, sizeof(MapIndex), 1, file);
    }
}

void Map::writePortals(FILE* file)
{
    uint32_t pnum = m_zonePortals.size();
    fwrite(&pnum, sizeof(uint32_t), 1, file);

    for (const Portal& portal : m_zonePortals)
    {
        const MapLeaf& owner0 = m_leafs[portal.owner[0]];
        const MapLeaf& owner1 = m_leafs[portal.owner[1]];

        fwrite(&portal.plane, sizeof(vec4), 1, file);
        fwrite(&owner0.zone, sizeof(MapIndex), 1, file);
        fwrite(&owner1.zone, sizeof(MapIndex), 1, file);

        uint32_t vnum = portal.vertices.size();
        fwrite(&vnum, sizeof(uint32_t), 1, file);
        fwrite(portal.vertices.data(), sizeof(vec3), vnum, file);
    }
}

void Map::writeZones(FILE* file)
{
    uint32_t znum = m_zones.size();
    fwrite(&znum, sizeof(uint32_t), 1, file);

    for (const Zone& zone : m_zones)
    {
        fwrite(&zone.type, sizeof(ZoneType), 1, file);

        uint32_t lnum = zone.leafs.size();
        fwrite(&lnum, sizeof(uint32_t), 1, file);
        fwrite(zone.leafs.data(), sizeof(MapIndex), lnum, file);

        uint32_t pnum = zone.portals.size();
        fwrite(&pnum, sizeof(uint32_t), 1, file);
        fwrite(zone.portals.data(), sizeof(MapIndex), pnum, file);
    }
}

void Map::displayPortals(Render::CommandList& commandList) const
{
    commandList.bindVertexBuffer(m_portalGeometry);

    size_t offset = 0;

    for (const Portal& portal : m_portals)
    {
        if (portal.zonePortal)
            commandList.setConstant(2, vec4(0.4, 1.0, 0.4, 1.0));
        else
            commandList.setConstant(2, vec4(0.4, 0.4, 1.0, 1.0));

        commandList.draw(portal.vertices.size(), offset);
        offset += portal.vertices.size();
    }
}