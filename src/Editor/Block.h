#pragma once

#include "math/math3d.h"
#include "Render/Render.h"
#include "Resources/Material.h"
#include "Utils/LinkedList.h"
#include "Utils/EditList.h"

#include "EditPoint.h"
#include "Geometry/Geometry.h"
#include "Geometry/Csg.h"

#include "Surfaces/EditSurface.h"

#include <vector>

enum class BlockType : uint8_t
{
    Edit = 0,
    Add,
    Subtruct,
    Static,
    Solid,
    Invalid = 0xFF
};

enum class PlaneType 
{ 
    XY,
    XZ,
    YZ
};

struct BlockPolygon
{   
    uint8_t flags = 0;
    uint8_t smoothGroop = 0;

    ResourcePtr<Material> material;

    uint16_t offset;
    uint16_t vertnum;

    vec3 s;
    vec3 t;

    vec2 tcoord;

    std::vector<EditPolygon*> editPolygons;
    std::vector<size_t> displayList;

    std::shared_ptr<EditSurface> surface;
};

class Block : public EditListNode<Block>
{
    using BlockPoint = EditPoint<Block>;
    using PointList = LinkedList<BlockPoint>;

public:

    static void Triangle(float width, float height, PlaneType ptype, Block& block);
    static void Plane(float width, float height, PlaneType ptype, Block& block);
    static void Box(float width, float length, float height, Block& block);
    static void Cylinder(float height, float radius, int sides, bool smooth, bool half, Block& block);
    static void Cone(float height, float radius, int sides, bool smooth, Block& block);
    static void Hemisphere(float radius, int sides, bool smooth, Block& block);
    static void Sphere(float radius, int sides, bool smooth, Block& block);

public:
    Block();
    Block(const std::vector<vec3>& vertices, const std::vector<uint16_t>& indices, const std::vector<BlockPolygon>& polygons, BlockType type = BlockType::Edit);
    Block(std::vector<vec3>&& vertices, std::vector<uint16_t>&& indices, std::vector<BlockPolygon>&& polygons, BlockType type = BlockType::Edit);
    Block(const Block& block);
    Block(const Block& block, BlockType type);

    void copy(const Block& block);
    Block* clone();

    void reset(std::vector<vec3>& vertices, std::vector<uint16_t>& indices,  std::vector<BlockPolygon>& polygons);

    void setMaterial(Material* material);
   
    BlockType type() const { return m_type; }
    void setType(BlockType type) { m_type = type; }

    const vec3& pos() const { return m_pos; }
    void setPos(const vec3& pos) { m_pos = pos; }

    const vec3& vertex(uint16_t i) const { return m_vertices[i]; }
    uint16_t index(size_t i) const { return m_indices[i]; }

    size_t polygonsNum() const { return m_polygons.size(); }
    BlockPolygon& polygon(size_t ind) { return m_polygons[ind]; }

    const BBox& bbox() const { return m_bbox; }
    void updateBBox();

    const vec3& rot() const { return m_rot; }
    void setXRot(float rot) { m_rot = { rot, 0, 0 }; }
    void setYRot(float rot) { m_rot = { 0, rot, 0 }; }
    void setZRot(float rot) { m_rot = { 0, 0, rot }; }

    bool pick(const vec3& origin, const vec3& ray, vec3& point, float& dist) const;
    bool pick2d(float x, float y, float scale, float& depth, int i, int j, int k);
    void select() { m_selected = true; }
    void deselect() { m_selected = false; }
    bool isSelected() const { return m_selected; }
    void markForUpdate() { m_needUpdate = true; }
    bool needUpdate() const { return m_needUpdate; }

    vec3* pickVertex(const vec3& origin, const vec3& ray, float& dist);
    bool pickVertex2d(float x, float y, float scale, int i, int k, PointList& pointList);

    const vec3& moveDiff() const { return m_pickdiff; }
    void calculateMoveDiff(const vec3& point);
    void pickMove(const vec3& newpt);
    void pickMove2d(float x, float y, int i, int k);
    void pickRotateX(float Cos, float Sin, const vec3& center);
    void pickRotateY(float Cos, float Sin, const vec3& center);
    void pickRotateZ(float Cos, float Sin, const vec3& center);
    void pickScale(const vec3& center, const vec3& scale);

    EditPolygon* pickPolygon(const vec3& origin, const vec3& ray, float& dist);
    bool pickSurface(const vec3& origin, const vec3& ray, float& dist);

    void displaceSurfaces(const vec3& center, float power, float radius);
    void collectSurfaceVerts(const vec3& point, float radius, std::vector<SurfaceVertexLink>& vlist);
    void convolveSurfaceVerts(const vec3& center, float radius, float& value, vec3& norm, float& num) const;
    
    void subdividePolygon(BlockPolygon* poly, size_t factor);
    SurfaceGraph* getSurfaceGraph();

    std::vector<EditSurface*>& surfaces() { return m_surfaces; }

    void applyRotation();
    void applyScale();

    bool hasSubdivision();
    bool hasCsg();
    void clearGeometry();
    void clearPolygons();
    void smoothPolygons(uint8_t smgroop);
    void generatePolygons();
    void buildGeometry();
    void collectPolygons(PolygonList& polygons) const;

    void categorizeBlockPolygons(Block& block) const;
    void clipBlock(Block& block) const;

    void setMaterial(Material* mat, std::vector<size_t>& displayList);
    void generateUV(const BlockPolygon* poly);

    void bindVertexBuffer(Render::CommandList& commandList) const;

    void display(Render::CommandList& commandList) const;
    void displayVertices(Render::CommandList& commandList) const;
    void displayGeometry(Render::CommandList& commandList) const;
    void displayGeometry(Render::CommandList& commandList, const std::vector<size_t>& displayList) const;
    void displaySurfaces(Render::CommandList& commandList) const;
    void displaySurfaceLayers(Render::CommandList& commandList) const;

    void write(FILE* file) const;
    void writeSurfaces(FILE* file) const;

private:

    struct TangentSpace
    {
        vec3 normal;
        vec3 tangent;
        vec3 binormal;
    };

    void init();
    void cleanEdPolyLists();

    CsgTree m_csgTree;

    std::vector<TangentSpace> m_tbuffer;
    std::vector<size_t> m_vcount;

    Render::VertexArray<vec3> m_vertices;
    std::vector<uint16_t> m_indices;
    std::vector<BlockPolygon> m_polygons;

    Render::IndexBuffer m_indexBuffer;

    std::vector<EditPolygon> m_editPolygons;
    Render::PushBuffer<Vertex> m_geometry;

    std::vector<Render::DisplayData> m_displayData;

    std::vector<EditSurface*> m_surfaces;

    size_t m_displayVerts;

    BlockType m_type;

    BBox m_bbox;
    vec3 m_pos;

    vec3 m_rot;
    vec3 m_scale;

    bool m_selected;
    vec3 m_pickdiff;

    bool m_needUpdate;

    static constexpr float PickDist2d = 0.015;
};