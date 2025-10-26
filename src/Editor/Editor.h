#pragma once

#include "Render/Render.h"
#include "Utils/EventHandler.h"

#include "Render/SceneConstantBuffer.h"

#include "RenderContext.h"

#include "IConsole.h"

#include "EditPoint.h"
#include "Block.h"
#include "Objects/Object.h"
#include "Surfaces/Surface.h"
#include "Decal.h"

#include "Geometry/Map.h"

#include <map>
#include <set>

class ViewCamera;

enum class EditMode
{
    Move,
    Rotate,
    Scale
};

enum class EditType
{
    Vertices,
    Blocks,
    Objects,
    Polygons,
    Decals,
    Surfaces,
    ControlPoints,
    Displace,
};

enum class DisplaceMode
{
    Up,
    Down,
    Smooth,
    Paint
};

enum class EditorSelectionType
{
    None,
    One,
    Multiple
};

using EditVertex = EditPoint<Block>;
using ControlPoint = EditPoint<Surface>;

struct PolygonSelection : public ListNode<PolygonSelection>
{
    Block* owner;
    BlockPolygon* origin;

    PolygonSelection(Block* block, BlockPolygon* poly)
    : owner(block)
    , origin(poly)
    {
    }
};

struct SurfaceGroup
{
    std::set<Block*> blocks;
    SurfaceGraphPtr surfaceGraph;
};

struct MapInfo
{
    std::string mapSky = "sky01";
    bool enableGlobalLight = false;
    vec3 dirLightDirection = { 0.8, -1.0, -0.8 };
    vec3 dirLightColor = { 1.0, 1.0, 1.0 };
    bool enableGi = false;
    vec3 giColor = { 0.12, 0.12, 0.2 };
};

struct EditorInfo
{
    // Main View camera parameters
    vec3 campos = { -3, 3, -3 };
    float camvangle = 45.0f;
    float camhangle = 45.0f;
    
    vec2 topViewOffset = { 0, 0};
    float topViewScale = 0.5f;

    vec2 sideViewOffset = { 0, 0 };
    float sideViewScale = 0.5f;

    vec2 frontViewOffset = { 0, 0 };
    float frontViewScale = 0.5f;
};

struct ColorRangeParam
{
    vec4 color;
    vec3 rangeColor;
    float radius;
    vec3 center;
    float padding;
};

class Editor
{
public:
    Editor(IConsole& console);

    void cleanup();
    void buildMap();
    void rebuildCsg();
    void collectUpdatedBlocks(bool clear = false);
    void updateCsg();

    void write(const std::string& filename, const EditorInfo& editorInfo);
    void read(const std::string& filename, EditorInfo& editorInfo);

    void showPortals(bool show) { m_showPortals = show; }

    void setDiscreteMove(bool discrete) { m_discreteMove = discrete; }
    void setDiscreteRotate(bool discrete) { m_discreteRotate = discrete; }
    void setDiscreteScale(bool discrete) { m_discreteScale = discrete; }

    void setMoveStep(float step) { m_moveStep = step; }
    void setRotateStep(float step) { m_rotateStep = step / 180.0 * math::pi; }
    void setScaleStep(float step) { m_scaleStep = step; }

    void setDisplaceMode(DisplaceMode mode) { m_displaceMode = mode; }
    void setDisplaceRadius(float radius) { m_displaceRadius = radius; }
    void setDisplacePower(float power) { m_displacePower = power; }
    void setPaintLayer(int layer) { m_paintLayer = layer; }

    void createTriangle(float width, float height, PlaneType ptype);
    void createPlane(float width, float height, PlaneType ptype);
    void createBox(float width, float height, float depth);
    void createCylinder(float height, float radius, int sides, bool smooth, bool half);
    void createCone(float height, float radius, int sides, bool smooth);
    void createHemisphere(float radius, int sides, bool smooth);
    void createSphere(float radius, int sides, bool smooth);

    void addBlock(BlockType type);
    void addObject(Object* object);
    void addSurface(Surface* surface);
    
    SurfaceGraph* buildSurfaceGraph();
    void subdividePolygons(int factor);

    void duplicateSelected();
    void deleteSelected();

    MapInfo& mapInfo() { return m_mapInfo; }

    void setEditMode(EditMode mode);
    EditMode getEditMode() { return m_editMode; }

    void setEditType(EditType type);
    EditType getEditType() { return m_editType; }

    void setCurrentMaterial(Material* mat) { m_curmat = mat; }
    Material* getCurrentMaterial() { return m_curmat; }

    void setPolygonFlag(PolygonFlag flag, bool set);
    uint8_t getPolygonFlags();

    void setPolygonSmoothGroop(uint8_t groop);
    uint8_t getPolygonSmoothGroop();

    EditSurface* getPolygonSurface();
    void addSurfaceLayer(Material* mat);

    void applyMaterialToPolygons();
    void movePolyU(float val);
    void movePolyV(float val);
    void scalePolyU(float val);
    void scalePolyV(float val);
    void rotatePolyUV(float ang);

    void applyMaterialToSurfaces();
    void moveSurfaceTexS(float val);
    void moveSurfaceTexT(float val);
    void scaleSurfaceTexS(float val);
    void scaleSurfaceTexT(float val);
    void rotateSurfaceTex(float ang);
   
    EditorSelectionType selectedPolygonsNum();
    PolygonSelection* selectedPolygon();

    EditorSelectionType selectedSurfacesNum();
    Surface* selectedSurface();

    Block* selectedBlock();
    Object* selectedObject();
    Block& editBlock() { return m_editBlock; }

    EditorSelectionType selectedCpNum();
    void setCpWeight(float weight);
    float getCpWeight();

    bool selectVertices(const vec3& origin, const vec3& ray, vec3& pickPoint, bool append);
    bool selectVertices2d(float x, float y, float scale, bool append, int i, int k);
    const vec3& vertsSelectionCenter() const { return m_vcenter; }
    void moveVertices(vec3 newpt);
    void moveVertices2d(float x, float y, int i, int k);
    void calcVertsXAngle(float y, float z);
    void calcVertsYAngle(float x, float z);
    void calcVertsZAngle(float x, float y);
    void rotateVertsX(const vec3& newpt);
    void rotateVertsY(const vec3& newpt);
    void rotateVertsZ(const vec3& newpt);
    void scaleVertices(const vec3& scale);

    void clearBlockSelection();
    bool selectBlocks(const vec3& origin, const vec3& ray, vec3& pickPoint, float& pickDist, bool append);
    bool selectBlocks2d(float x, float y, float scale, bool append, vec3& pickPoint, int i, int j, int k);
    const vec3& blockSelectionCenter() const;
    void moveBlocks(vec3 newpt);
    void moveBlocks2d(float x, float y, int i, int k);
    void calcBlockXAngle(float y, float z);
    void calcBlockYAngle(float x, float z);
    void calcBlockZAngle(float x, float y);
    void rotateBlocksX(const vec3& newpt);
    void rotateBlocksY(const vec3& newpt);
    void rotateBlocksZ(const vec3& newpt);
    void scaleBlocks(const vec3& scale);
    void applyBlockRotation();
    void applyBlockScale();
    void updateBlockBBoxes();

    void selectPolygon(const vec3& origin, const vec3& ray, bool append);

    void addDecal(const vec3& origin, const vec3& ray);

    void clearObjectSelection();
    bool selectObjects(const vec3& origin, const vec3& ray, vec3& pickPoint, float& pickDist, bool append);
    bool selectObjects2d(float x, float y, float scale, bool append, vec3& pickPoint, int i, int j, int k);
    void moveObjects(vec3 newpt);
    void moveObjects2d(float x, float y, int i, int k);
    void calcObjectXAngle(float y, float z);
    void calcObjectYAngle(float x, float z);
    void calcObjectZAngle(float x, float y);
    void rotateObjectsX(const vec3& newpt);
    void rotateObjectsY(const vec3& newpt);
    void rotateObjectsZ(const vec3& newpt);
    void applyObjectRotation();

    void clearSurfaceSelection();
    bool selectSurfaces(const vec3& origin, const vec3& ray, vec3& pickPoint, float& pickDist, bool append);
    bool selectSurfaces2d(float x, float y, float scale, bool append, vec3& pickPoint, int i, int j, int k);
    const vec3& surfaceSelectionCenter() const;
    void moveSurfaces(vec3 newpt);
    void moveSurfaces2d(float x, float y, int i, int k);
    void calcSurfaceXAngle(float y, float z);
    void calcSurfaceYAngle(float x, float z);
    void calcSurfaceZAngle(float x, float y);
    void rotateSurfacesX(const vec3& newpt);
    void rotateSurfacesY(const vec3& newpt);
    void rotateSurfacesZ(const vec3& newpt);
    void scaleSurfaces(const vec3& scale);
    void applySurfaceRotation();
    void applySurfaceScale();

    bool selectCp(const vec3& origin, const vec3& ray, vec3& pickPoint, bool append);
    bool selectCp2d(float x, float y, float scale, bool append, int i, int k);
    const vec3& cpSelectionCenter() const { return m_cpcenter; }
    void moveCp(vec3 newpt);
    void moveCp2d(float x, float y, int i, int k);
    void calcCpXAngle(float y, float z);
    void calcCpYAngle(float x, float z);
    void calcCpZAngle(float x, float y);
    void rotateCpX(const vec3& newpt);
    void rotateCpY(const vec3& newpt);
    void rotateCpZ(const vec3& newpt);
    void scaleCp(const vec3& scale);

    void surfaceIntersect(const vec3& origin, const vec3& ray);
    void surfaceDisplace(const vec3& origin, const vec3& ray);

    void addSurfaceDetails(size_t layer, const std::string& model, const std::string& material, float density);
    void clearSurfaceDetails();

    Material* loadMaterial(const std::string& name);

    void updateViews() { onUpdate(); }

    void display(Render::FrameBuffer& frameBuffer,
                 const ViewCamera& camera,
                 const mat4& projMat,
                 int width,
                 int height);

    void displayOrtho(Render::FrameBuffer& frameBuffer, 
                      const mat4& viewProj,
                      const mat4& viewMat,
                      const mat4& gridMat, 
                      const vec3& viewpos, 
                      float scale, 
                      int width, 
                      int height);

    Event<void()> onUpdate;
    Event<void()> onPolygonSelectionUpdate;
    Event<void()> onObjectSelectionUpdate;
    Event<void()> onSurfaceSelectionUpdate;

private:

    void clipBlock(Block* newBlock);
    void clipBlockUpdated(Block* newBlock);

    EditVertex* findSelectedVert(vec3* vert, Block* owner);
    bool testSelectedVerts2d(float x, float y, float scale, int i, int k);
    bool deselectVerts2d(float x, float y, float scale, int i, int k);
    void fillVertsSelectionBuffer();
    void clearVertsSelection();
    void calcSelectedVertsCenter();

    ControlPoint* findSelectedCp(vec3* cp, Surface* owner);
    bool testSelectedCp2d(float x, float y, float scale, int i, int k);
    bool deselectCp2d(float x, float y, float scale, int i, int k);
    void fillCpSelectionBuffer();
    void clearCpSelection();
    void calcSelectedCpCenter();

    void duplicateBlocks();
    void duplicateObjects();
    void duplicateSurfaces();

    void deleteBlocks();
    void deleteObjects();
    void deleteSurfaces();

    bool deselectPolygon(EditPolygon* poly);

    void calculateFrustrum(const ViewCamera& camera, const mat4& projMat, vec3* frustum);

    void initGridGeometry();
    void drawGrid(float x, float y);
    void drawGridOrtho(const mat4& gridMat, float x, float y);

    void writeMapInfo(FILE* file);
    void writeBlocks(FILE* file);
    void writeObjects(FILE* file);
    void writeEntities(FILE* file);
    void writeSurfaces(FILE* file);
    void writeGameSurfaces(FILE* file);

    void readMapInfo(FILE* file);
    void readBlocks(FILE* file);
    void readObjects(FILE* file);
    void readEntities(FILE* file);
    void readSurfaces(FILE* file);

private:
    IConsole& m_console;

    //render::CommandList m_commandList;
    RenderContext m_renderContext;

    Render::ConstantBuffer<Render::SceneConstantBuffer> m_sceneConstantBuffer;

    Render::VertexBuffer m_gridBuffer;
    Render::VertexBuffer m_fineGridBuffer;
    Render::PushBuffer<vec3> m_vertexBuffer;

    EditMode m_editMode;
    EditType m_editType;
    DisplaceMode m_displaceMode;

    Map m_map;

    Material* m_curmat;

    Block m_editBlock;

    LinkedList<Block> m_blocks;
    EditList<Block> m_selectedBlocks;
    Block* m_pickedBlock;
    EditVertex* m_pickedVertex;

    vec3 m_vcenter;

    LinkedList<EditVertex> m_selectedVerts;
    Render::PushBuffer<vec3> m_blockVerts;

    LinkedList<PolygonSelection> m_selectedPolys;
    vec3 m_surfaceIntersect;
    bool m_pointSurface;

    LinkedList<Object> m_objects;
    EditList<Object> m_selectedObjects;
    Object* m_pickedObject;

    LinkedList<Surface> m_surfaces;
    EditList<Surface> m_selectedSurfaces;
    Surface* m_pickedSurface;
    ControlPoint* m_pickedCp;

    LinkedList<Decal> m_decals;
    EditList<Decal> m_selectedDecals;
    Decal* m_pickedDecal;

    vec3 m_cpcenter;

    LinkedList<ControlPoint> m_selectedCps;
    Render::PushBuffer<vec3> m_controlPoints;

    size_t m_surfaceIdNum;
    std::map<size_t, SurfaceGroup> m_surfaceGroups;

    float m_ang;

    bool m_discreteMove;
    bool m_discreteRotate;
    bool m_discreteScale;
    float m_moveStep;
    float m_rotateStep;
    float m_scaleStep;

    MapInfo m_mapInfo;

    float m_displaceRadius;
    float m_displacePower;
    int m_paintLayer;

    bool m_showPortals;

    static constexpr int GridSize = 100;
    static constexpr int FineGridSize = GridSize;
    static constexpr int GridScale = 5;
    static constexpr int FineGridGran = 5;
    static constexpr float GridMoveStep = 50;
    static constexpr vec3 DuplicateShift = { 0.5f, 0.5f, 0.5f };
};