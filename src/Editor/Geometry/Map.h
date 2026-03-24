#pragma once

#include "math/math3d.h"
#include "Render/Render.h"
#include "Geometry.h"
#include "Geometry/Block.h"

#include <vector>
#include <list>

class Block;

using MapIndex = uint32_t;

enum class ZoneType : uint8_t
{
	Default,
	Water,
	Lava,
	Slime
};

struct MapPolygon
{
	vec4 plane;

	bool splitter;
	uint8_t flags;
	ResourcePtr<Material> material;

	std::vector<IndexType> vertices;
};

struct Portal
{
	vec4 plane;

	bool zonePortal;
	MapIndex zonePortalId;

	std::vector<vec3> vertices;

	int leafnum;
	MapIndex owner[2];
};

struct Zone
{
	ZoneType type;
	std::vector<MapIndex> leafs;
	std::vector<MapIndex> portals;
};

struct MapLeaf
{
    std::vector<MapPolygon> polygons;
	std::vector<Render::DisplayData> displayData;
	Render::GeometryData rtxGeometry;

    BBox bbox;

    std::vector<Portal*> portals;

	MapIndex zone;
	uint8_t flags;
};

struct MapNode
{
    vec4 plane;
	BBox bbox;

	MapIndex leaf;

	MapIndex left;
	MapIndex right;
};

class Map
{
	using IndexList = std::vector<IndexType>;
	using PolygonList = std::vector<MapPolygon>;

public:
	void cleanup();
	void build(PolygonList& polygons);
	void build(const LinkedList<Block>& blocks);

	bool empty() const { return m_nodes.empty(); }
	bool hasPortals() const { return !m_portals.empty(); }

	void generateDisplayData();

	void writeVertexData(FILE* file);
	void writeLeafs(FILE* file);
	void writeNodes(FILE* file);
	void writePortals(FILE* file);
	void writeZones(FILE* file);

	void displayPortals(Render::CommandList& commandList) const;

private:
	void addBlockGeometry(const Block* block, std::vector<MapPolygon>& polygons);

	void calcBBox(const PolygonList& plg, BBox& bbox);

	PolyType classifyPolygon(const vec4& plane, const std::vector<IndexType>& verts);
	void splitPoly(const vec4& plane, const IndexList& verts, IndexList& left, IndexList& right);
	bool selectSplitter(const PolygonList& plg, size_t& splitterId, bool& zonePortal);
	MapIndex buildTree(PolygonList& plg);

	static void getPortalBounds(const Portal& portal, vec3& min, vec3& max);

	Portal generatePortal(const MapNode& node);
	void clipPortal(Portal& portal, MapIndex nodeId, std::list<Portal>& clipPortals);
	bool isPortalDuplicate(Portal& portal);
	void zonePortals();
	void portalize();

	void collectZonePortals();
	void collectLeafs(MapIndex zoneId, MapIndex leafId);
	void generateZones();

	void addZonePortal(const MapPolygon& poly);

	void buildPortlaGeometry();

private:
	std::list<Portal> m_portals;
	std::vector<Portal> m_zonePortals;
	std::vector<Zone> m_zones;
	std::vector<MapLeaf> m_leafs;
	std::vector<MapNode> m_nodes;

	std::vector<Vertex> m_vertices;
	std::vector<IndexType> m_indices;

	size_t m_zonePortalNum;

	Render::PushBuffer<vec3> m_portalGeometry;

	static constexpr MapIndex InvalidMapIndex = -1;
	static constexpr MapIndex TreeRoot = 0;
};