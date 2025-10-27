#include "Editor.h"

#include "View/ViewCamera.h"
#include "Render/Frustum.h"
#include "Render/Clipping.h"
#include "Resources/Resources.h"

#include <utility>
#include <set>

Editor::Editor()
: m_editMode(EditMode::Move)
, m_editType(EditType::Blocks)
, m_displaceMode(DisplaceMode::Up)
, m_curmat("blocks01")
, m_pickedBlock(nullptr)
, m_discreteMove(true)
, m_discreteRotate(true)
, m_discreteScale(true)
, m_moveStep(0.1)
, m_rotateStep(5.0 / 180.0 * math::pi)
, m_scaleStep(0.1)
, m_displaceRadius(0.5)
, m_displacePower(1.0)
, m_paintLayer(1)
, m_showPortals(false)
, m_surfaceIdNum(1)
{
    initGridGeometry();
    Block::Box(2, 2, 2, m_editBlock);

    m_blocks.append(&m_editBlock);
}

void Editor::setEditMode(EditMode mode)
{ 
    m_editMode = mode; 
    calcSelectedVertsCenter(); 
}

void Editor::setEditType(EditType type) 
{ 
    m_editType = type; 
    if (type == EditType::Vertices) fillVertsSelectionBuffer();

    onUpdate();
}

void Editor::cleanup()
{
    clearVertsSelection();

    m_selectedBlocks.clear();
    m_selectedObjects.clear();
    m_selectedSurfaces.clear();
    m_selectedPolys.clear();
    m_blocks.remove(&m_editBlock);

    m_blocks.destroy();
    m_objects.destroy();
    m_surfaces.destroy();

    m_surfaceGroups.clear();

    m_editBlock.deselect();
    m_editBlock.setPos({});

    Block::Box(2, 2, 2, m_editBlock);
    m_blocks.append(&m_editBlock);

    m_selectedObjects.clear();
    m_objects.destroy();

    m_map.cleanup();

    m_mapInfo = {};
}

void Editor::buildMap()
{
    PolygonList polygons;

    for (const Block* block : m_blocks) block->collectPolygons(polygons);

    m_map.build(polygons);
    onUpdate();
}

void Editor::rebuildCsg()
{
    for (Block* block : m_blocks) block->clearGeometry();

    for (Block* block : m_blocks)
    {
        block->generatePolygons();
        clipBlock(block);
    }

    for (Block* block : m_blocks) block->buildGeometry();

    onUpdate();
}

void Editor::collectUpdatedBlocks(bool clear)
{
    for (Block* sblock : m_selectedBlocks)
    {
        if (sblock->type() == BlockType::Edit) continue;
        if (!sblock->hasCsg()) continue;
        if (sblock->hasSubdivision()) continue;

        if (clear) sblock->markForUpdate();

        for (Block* block : m_blocks)
        {
            if (block == sblock) continue;
            if (block->type() == BlockType::Edit) continue;
            if (block->hasSubdivision()) continue;
            if (block->needUpdate()) continue;
            if (!BBoxIntersect(sblock->pos(), sblock->bbox(), block->pos(), block->bbox())) continue;

            block->clearGeometry();
            block->markForUpdate();
        }
    }
}

void Editor::updateCsg()
{
    if (m_selectedBlocks.empty()) return;

    for (Block* sblock : m_selectedBlocks) sblock->clearPolygons();

    for (Block* block : m_blocks)
    {
        if (block->needUpdate()) block->generatePolygons();
        clipBlockUpdated(block);
    }

    for (Block* block : m_blocks)
    {
        if (block->needUpdate()) block->buildGeometry();
    }

    onUpdate();
}

void Editor::createTriangle(float width, float height, PlaneType ptype)
{
    Block::Triangle(width, height, ptype, m_editBlock);
    onUpdate();
}

void Editor::createPlane(float width, float height, PlaneType ptype)
{
    Block::Plane(width, height, ptype, m_editBlock);
    onUpdate();
}

void Editor::createBox(float width, float height, float depth)
{
    Block::Box(width, depth, height, m_editBlock);
    onUpdate();
}

void Editor::createCylinder(float height, float radius, int sides, bool smooth, bool half)
{
    Block::Cylinder(height, radius, sides, smooth, half, m_editBlock);
    onUpdate();
}

void Editor::createCone(float height, float radius, int sides, bool smooth)
{
    Block::Cone(height, radius, sides, smooth, m_editBlock);
    onUpdate();
}

void Editor::createHemisphere(float radius, int sides, bool smooth)
{
    Block::Hemisphere(radius, sides, smooth, m_editBlock);
    onUpdate();
}

void Editor::createSphere(float radius, int sides, bool smooth)
{
    Block::Sphere(radius, sides, smooth, m_editBlock);
    onUpdate();
}

void Editor::addBlock(BlockType type)
{
    if (m_editBlock.polygonsNum() < 4) type = BlockType::Solid;

    Block* newBlock = new Block(m_editBlock, type);
    
    newBlock->setMaterial(loadMaterial(m_curmat));
    newBlock->generatePolygons();

    clipBlock(newBlock);
    newBlock->buildGeometry();

    m_blocks.append(newBlock);

    onUpdate();
}

void Editor::addObject(Object* object)
{
    m_objects.append(object);
    onUpdate();
}

void Editor::addSurface(Surface* surface)
{
    m_surfaces.append(surface);
    onUpdate();
}

SurfaceGraph* Editor::buildSurfaceGraph()
{
    std::vector<EditSurface*> surfaces;

    std::set<Block*> blocks;

    for (PolygonSelection* poly : m_selectedPolys)
    {
        EditSurface* surface = poly->origin->surface.get();

        if (!surface) continue;
    
        blocks.insert(poly->owner);
    }

    if (blocks.empty()) return nullptr;

    for (Block* block : blocks)
    {
        if (block->type() != BlockType::Solid) continue;

        auto blockSurfaces = block->surfaces();

        if (!blockSurfaces.empty()) surfaces.insert(surfaces.end(), blockSurfaces.begin(), blockSurfaces.end());
    }

    size_t id = m_surfaceIdNum++;

    SurfaceGraphPtr surfaceGraph = std::make_shared<SurfaceGraph>(id, surfaces);

    for (EditSurface* surface : surfaces) surface->setSurfaceGraph(surfaceGraph);

    m_surfaceGroups[id].blocks = std::move(blocks);
    m_surfaceGroups[id].surfaceGraph = surfaceGraph;

    return surfaceGraph.get();
}

void Editor::subdividePolygons(int factor)
{
    std::vector<EditSurface*> surfaces;

    for (PolygonSelection* poly : m_selectedPolys)
    {
        Block* block = poly->owner;

        if (block->type() != BlockType::Solid) continue;

        block->subdividePolygon(poly->origin, factor);
        surfaces.push_back(poly->origin->surface.get());
    }

    size_t id = m_surfaceIdNum++;

    SurfaceGraphPtr surfaceGraph = std::make_shared<SurfaceGraph>(id, surfaces);

    for (PolygonSelection* poly : m_selectedPolys)
    {
        if (poly->owner->type() != BlockType::Solid) continue;
        poly->origin->surface->setSurfaceGraph(surfaceGraph);
    }

    m_surfaceGroups[id].surfaceGraph = surfaceGraph;

    onUpdate();
}

void Editor::duplicateSelected()
{
    if (m_editType == EditType::Blocks) duplicateBlocks();
    if (m_editType == EditType::Objects) duplicateObjects();
    if (m_editType == EditType::Surfaces) duplicateSurfaces();
}

void Editor::deleteSelected()
{
    if (m_editType == EditType::Blocks) deleteBlocks();
    if (m_editType == EditType::Objects) deleteObjects();
    if (m_editType == EditType::Surfaces) deleteSurfaces();
}

void Editor::clipBlock(Block* newBlock)
{
    for (Block* block : m_blocks)
    {
        if (block == newBlock) break;
        if (block->type() == BlockType::Edit) continue;
        if (!block->hasCsg()) continue;

        if (!BBoxIntersect(newBlock->pos(), newBlock->bbox(), block->pos(), block->bbox())) continue;

        if (block->type() != BlockType::Solid) block->categorizeBlockPolygons(*newBlock);
        if (newBlock->type() != BlockType::Solid) newBlock->clipBlock(*block);

        block->buildGeometry();
    }
}

void Editor::clipBlockUpdated(Block* newBlock)
{
    for (Block* block : m_blocks)
    {
        if (block == newBlock) break;
        if (block->type() == BlockType::Edit) continue;
        if (!block->hasCsg()) continue;

        if (!BBoxIntersect(newBlock->pos(), newBlock->bbox(), block->pos(), block->bbox())) continue;

        if (block->type() != BlockType::Solid && newBlock->needUpdate()) block->categorizeBlockPolygons(*newBlock);
        if (newBlock->type() != BlockType::Solid && block->needUpdate()) newBlock->clipBlock(*block);
    }
}

EditVertex* Editor::findSelectedVert(vec3* vert, Block* owner)
{
    for (EditVertex* editPt : m_selectedVerts)
    {
        if (editPt->owner == owner)
        {
            float dist = (*(editPt->point) - *vert).length();

            if (dist < math::eps) return editPt;
        }
    }

    return nullptr;
}

bool Editor::testSelectedVerts2d(float x, float y, float scale, int i, int k)
{
    for (EditVertex* editPt : m_selectedVerts)
    {
        vec3 vert = *editPt->point + editPt->owner->pos();
        float dist = vec2(x - vert[i], y - vert[k]).length();

        if (dist < EditVertex::PickDist / scale) return true;
    }

    return false;
}

bool Editor::deselectVerts2d(float x, float y, float scale, int i, int k)
{
    bool result = false;

    for (auto it = m_selectedVerts.begin(); it != m_selectedVerts.end(); )
    {
        EditVertex* editPt = *it;

        it++;

        float dist = vec2(x - (*editPt->point)[i], y - (*editPt->point)[k]).length();

        if (dist < EditVertex::PickDist / scale)
        {
            m_selectedVerts.remove(editPt);
            delete editPt;
        
            result = true;
        }
    }

    return result;
}

void Editor::fillVertsSelectionBuffer()
{
    m_blockVerts.clear();

    for (EditVertex* editPt : m_selectedVerts) m_blockVerts.push(*(editPt->point) + editPt->owner->pos());
}

void Editor::clearVertsSelection()
{
    m_selectedVerts.destroy();
    m_blockVerts.clear();
}

ControlPoint* Editor::findSelectedCp(vec3* cp, Surface* owner)
{
    for (ControlPoint* controlPt : m_selectedCps)
    {
        if (controlPt->owner == owner)
        {
            float dist = (*(controlPt->point) - *cp).length();

            if (dist < math::eps) return controlPt;
        }
    }

    return nullptr;
}

bool Editor::testSelectedCp2d(float x, float y, float scale, int i, int k)
{
    for (ControlPoint* controlPt : m_selectedCps)
    {
        vec3 cp = *controlPt->point + controlPt->owner->pos();
        float dist = vec2(x - cp[i], y - cp[k]).length();

        if (dist < ControlPoint::PickDist / scale) return true;
    }

    return false;
}

bool Editor::deselectCp2d(float x, float y, float scale, int i, int k)
{
    bool result = false;

    for (auto it = m_selectedCps.begin(); it != m_selectedCps.end(); )
    {
        ControlPoint* controlPt = *it;

        it++;

        float dist = vec2(x - (*controlPt->point)[i], y - (*controlPt->point)[k]).length();

        if (dist < EditVertex::PickDist / scale)
        {
            m_selectedCps.remove(controlPt);
            delete controlPt;

            result = true;
        }
    }

    return result;
}

void Editor::fillCpSelectionBuffer()
{
    m_controlPoints.clear();

    for (ControlPoint* controlPt : m_selectedCps) m_controlPoints.push(*(controlPt->point) + controlPt->owner->pos());
}

void Editor::clearCpSelection()
{
    m_selectedCps.destroy();
    m_controlPoints.clear();
}

void Editor::calcSelectedCpCenter()
{
    m_cpcenter = {};
    int num = 0;

    if (m_selectedCps.empty()) return;

    if (m_editMode == EditMode::Scale)
    {
        for (ControlPoint* cp : m_selectedCps)
        {
            m_cpcenter += *(cp->point) + cp->owner->pos();
            num++;
        }

        if (!m_selectedCps.empty()) m_cpcenter *= 1.0 / num;

    }
    else m_cpcenter = m_pickedSurface->pos();

    for (ControlPoint* cp : m_selectedCps)
    {
        vec3 pos = *(cp->point) + cp->owner->pos();
        cp->diff = pos - m_cpcenter;
    }
}

void Editor::setPolygonFlag(PolygonFlag flag, bool set)
{
    if (m_selectedPolys.empty()) return;

    PolygonSelection* poly = m_selectedPolys.head();

    if (set) poly->origin->flags |= flag;
    else poly->origin->flags &= ~flag;

    for (EditPolygon* editPoly : poly->origin->editPolygons) editPoly->flags = poly->origin->flags;
}

uint8_t Editor::getPolygonFlags()
{
    if (m_selectedPolys.empty()) return 0;

    PolygonSelection* poly = m_selectedPolys.head();
    return poly->origin->flags;
}

void Editor::setPolygonSmoothGroop(uint8_t groop)
{
    if (m_selectedPolys.empty()) return;

    for (PolygonSelection* poly : m_selectedPolys) 
        poly->origin->smoothGroop = groop;

    // TODO: normals/tangents need to be rebuilt
}

uint8_t Editor::getPolygonSmoothGroop()
{
    if (m_selectedPolys.empty()) return 0;

    PolygonSelection* poly = m_selectedPolys.head();
    return poly->origin->smoothGroop;
}

EditSurface* Editor::getPolygonSurface()
{
    if (m_selectedPolys.empty()) return nullptr;

    PolygonSelection* poly = m_selectedPolys.head();
    return poly->origin->surface.get();
}

void Editor::addSurfaceLayer(Material* mat)
{
    if (m_selectedPolys.empty()) return;

    for (PolygonSelection* poly : m_selectedPolys)
    {
        EditSurface* surf = poly->origin->surface.get();

        if (surf) surf->addLayer(mat);
    }
}

void Editor::applyMaterialToPolygons()
{
    for (PolygonSelection* poly : m_selectedPolys)
    {
        poly->origin->material = loadMaterial(m_curmat);
        poly->owner->setMaterial(loadMaterial(m_curmat), poly->origin->displayList);

        if (poly->origin->surface) poly->origin->surface->setMaterial(loadMaterial(m_curmat));

        for (EditPolygon* editPoly : poly->origin->editPolygons) editPoly->material = loadMaterial(m_curmat);
    }

    onUpdate();
}

void Editor::movePolyU(float val)
{
    for (PolygonSelection* poly : m_selectedPolys)
    {
        if (poly->origin->surface)
        {
            poly->origin->surface->moveTexCoordS(val);
        }
        else
        {
            poly->origin->tcoord.x += val;
            poly->owner->generateUV(poly->origin);
        }
    }

    onUpdate();
}

void Editor::movePolyV(float val)
{
    for (PolygonSelection* poly : m_selectedPolys)
    {
        if (poly->origin->surface)
        {
            poly->origin->surface->moveTexCoordT(val);
        }
        else
        {
            poly->origin->tcoord.y += val;
            poly->owner->generateUV(poly->origin);
        }
    }

    onUpdate();
}

void Editor::scalePolyU(float val)
{
    for (PolygonSelection* poly : m_selectedPolys)
    {
        if (poly->origin->surface)
        {
            poly->origin->surface->scaleTexCoordS(val);
        }
        else
        {
            poly->origin->s *= val;
            poly->owner->generateUV(poly->origin);
        }
    }

    onUpdate();
}

void Editor::scalePolyV(float val)
{
    for (PolygonSelection* poly : m_selectedPolys)
    {
        if (poly->origin->surface)
        {
            poly->origin->surface->scaleTexCoordT(val);
        }
        else
        {
            poly->origin->t *= val;
            poly->owner->generateUV(poly->origin);
        }
    }

    onUpdate();
}

void Editor::rotatePolyUV(float ang)
{
    float Cos = cos(ang / 180.0 * math::pi);
    float Sin = sin(ang / 180.0 * math::pi);

    for (PolygonSelection* poly : m_selectedPolys)
    {
        if (poly->origin->surface)
        {
            poly->origin->surface->rotateTexCoord(ang);
        }
        else
        {
            vec3 s = poly->origin->s;
            vec3 t = poly->origin->t;

            poly->origin->s = s * Cos - t * Sin;
            poly->origin->t = s * Sin + t * Cos;

            poly->owner->generateUV(poly->origin);
        }
    }

    onUpdate();
}

void Editor::applyMaterialToSurfaces()
{
    for (Surface* surface : m_selectedSurfaces) surface->setMaterial(loadMaterial(m_curmat));

    onUpdate();
}

void Editor::moveSurfaceTexS(float val)
{
    for (Surface* surface : m_selectedSurfaces) surface->moveTextureS(val);

    onUpdate();
}

void Editor::moveSurfaceTexT(float val)
{
    for (Surface* surface : m_selectedSurfaces) surface->moveTextureT(val);

    onUpdate();
}

void Editor::scaleSurfaceTexS(float val)
{
    for (Surface* surface : m_selectedSurfaces) surface->scaleTextureS(val);

    onUpdate();
}

void Editor::scaleSurfaceTexT(float val)
{
    for (Surface* surface : m_selectedSurfaces) surface->scaleTextureT(val);

    onUpdate();
}

void Editor::rotateSurfaceTex(float ang)
{
    for (Surface* surface : m_selectedSurfaces) surface->rotateTexture(ang);

    onUpdate();
}

EditorSelectionType Editor::selectedPolygonsNum()
{
    if (m_selectedPolys.empty()) return EditorSelectionType::None;

    if (m_selectedPolys.head() == m_selectedPolys.tail()) return EditorSelectionType::One;
    else return EditorSelectionType::Multiple;
}

PolygonSelection* Editor::selectedPolygon()
{
    if (m_selectedPolys.empty()) return nullptr;

    return m_selectedPolys.head();
}

EditorSelectionType Editor::selectedSurfacesNum()
{
    if (m_selectedSurfaces.empty()) return EditorSelectionType::None;

    if (m_selectedSurfaces.head() == m_selectedSurfaces.tail()) return EditorSelectionType::One;
    else return EditorSelectionType::Multiple;
}

Surface* Editor::selectedSurface()
{
    if (m_selectedSurfaces.empty()) return nullptr;

    return m_selectedSurfaces.head();
}

Block* Editor::selectedBlock()
{
    if (m_selectedBlocks.empty()) return nullptr;

    if (m_selectedBlocks.head() == m_selectedBlocks.tail())
    {
        Block* block = m_selectedBlocks.head();
        return block->type() != BlockType::Edit ? block : nullptr;
    }
    else return nullptr;
}

Object* Editor::selectedObject()
{
    if (m_selectedObjects.empty()) return nullptr;

    if (m_selectedObjects.head() == m_selectedObjects.tail()) return m_selectedObjects.head();
    else return nullptr;
}

EditorSelectionType Editor::selectedCpNum()
{
    if (m_selectedCps.empty()) return EditorSelectionType::None;

    if (m_selectedCps.head() == m_selectedCps.tail()) return EditorSelectionType::One;
    else return EditorSelectionType::Multiple;
}

void Editor::setCpWeight(float weight)
{
    std::set<Surface*> changedSurfs;

    for (ControlPoint* cpoint : m_selectedCps)
    {
        vec4* cp = reinterpret_cast<vec4*>(cpoint->point);
        cp->w = weight;

        changedSurfs.insert(cpoint->owner);
    }

    for (auto surf : changedSurfs) surf->update();

    onUpdate();
}

float Editor::getCpWeight()
{
    if (m_selectedCps.empty()) return 1.0;

    ControlPoint* cpoint = m_selectedCps.head();
    vec4* cp = reinterpret_cast<vec4*>(cpoint->point);

    return cp->w;
}

bool Editor::selectVertices(const vec3& origin, const vec3& ray, vec3& pickPoint, bool append)
{
    vec3* pickedVert = nullptr;
    float pickDist;
 
    Block* pickBlock = nullptr;

    for (Block* block : m_selectedBlocks)
    {
        float dist;

        vec3* vert = block->pickVertex(origin, ray, dist);

        if (vert)
        {
            if (!pickedVert || dist < pickDist)
            {
                pickedVert = vert;
                pickDist = dist;
                pickBlock = block;
            }
        }
    }

    if (pickedVert)
    {
        EditVertex* editPt = findSelectedVert(pickedVert, pickBlock);

        if (editPt)
        {
            if (append)
            {
                m_selectedVerts.remove(editPt);
                delete editPt;

                fillVertsSelectionBuffer();
            }
        }
        else
        {
            if (!append) clearVertsSelection();

            editPt = new EditPoint(pickedVert, pickBlock);
            m_selectedVerts.append(editPt);
            m_blockVerts.push(*pickedVert + pickBlock->pos());
        }

        m_pickedVertex = editPt;

        pickPoint = *pickedVert + pickBlock->pos();

        if (m_editMode == EditMode::Move)
        {
            for (EditVertex* vert : m_selectedVerts)
            {
                vec3 pos = *(vert->point) + vert->owner->pos();
                vert->diff = pos - pickPoint;
            }
        
        } else calcSelectedVertsCenter();

        onUpdate();
        return true;
    }
    else
    {
        m_pickedVertex = nullptr;

        if (!append) clearVertsSelection();

        onUpdate();
        return false;
    }
}

bool Editor::selectVertices2d(float x, float y, float scale, bool append, int i, int k)
{
    bool result = false;

    Block* pickBlock = nullptr;

    bool select = false;

    if (!append)
    {
        if (!testSelectedVerts2d(x, y, scale, i, k))
        {
            m_selectedVerts.destroy();
            select = true;
        
        } else result = true;
    }
    else
    {
        if (!deselectVerts2d(x, y, scale, i, k)) select = true;
    }

    if (select)
    {
        for (Block* block : m_selectedBlocks)
        {
            float dist;

            bool pick = block->pickVertex2d(x, y, scale, i, k, m_selectedVerts);

            if (pick)
            {
                pickBlock = block;
                result = true;
            }
        }
    }

    if (m_editMode == EditMode::Move)
    {
        float mindist = std::numeric_limits<float>::infinity();

        for (EditVertex* vert : m_selectedVerts)
        {
            vec3 pos = *(vert->point) + vert->owner->pos();
            vert->diff[i] = pos[i] - x;
            vert->diff[k] = pos[k] - y;

            float dist = sqrt(vert->diff[i] * vert->diff[i] + vert->diff[k] * vert->diff[k]);
            
            if (dist < mindist)
            {
                mindist = dist;
                m_pickedVertex = vert;
            }
        }
    
    } else calcSelectedVertsCenter();

    fillVertsSelectionBuffer();

    onUpdate();

    return result;
}

void Editor::calcSelectedVertsCenter()
{
    m_vcenter = {};
    int num = 0;

    if (m_selectedVerts.empty()) return;

    if (m_editMode == EditMode::Scale)
    {
        for (EditVertex* vert : m_selectedVerts)
        {
            m_vcenter += *(vert->point) + vert->owner->pos();
            num++;
        }

        if (!m_selectedVerts.empty()) m_vcenter *= 1.0 / num;
    
    } else m_vcenter = m_pickedBlock->pos();

    for (EditVertex* vert : m_selectedVerts)
    {
        vec3 pos = *(vert->point) + vert->owner->pos();
        vert->diff = pos - m_vcenter;
    }
}

void Editor::moveVertices(vec3 newpt)
{
    if (m_discreteMove)
    {
        const vec3& diff = m_pickedVertex->diff;
        vec3 pos = newpt + diff;

        pos.x = round(pos.x / m_moveStep) * m_moveStep;
        pos.y = round(pos.y / m_moveStep) * m_moveStep;
        pos.z = round(pos.z / m_moveStep) * m_moveStep;

        newpt = pos - diff;
    }

    for (EditVertex* vert : m_selectedVerts) *vert->point = newpt + vert->diff - vert->owner->pos();

    fillVertsSelectionBuffer();
}

void Editor::moveVertices2d(float x, float y, int i, int k)
{
    if (m_discreteMove)
    {
        const vec3& diff = m_pickedVertex->diff;

        float px = x + diff[i];
        float py = y + diff[k];

        px = round(px / m_moveStep) * m_moveStep;
        py = round(py / m_moveStep) * m_moveStep;

        x = px - diff[i];
        y = py - diff[k];
    }

    for (EditVertex* vert : m_selectedVerts)
    {
        const vec3& pos = vert->owner->pos();

        (*vert->point)[i] = x + vert->diff[i] - pos[i];
        (*vert->point)[k] = y + vert->diff[k] - pos[k];
    }

    fillVertsSelectionBuffer();
}

void Editor::calcVertsXAngle(float y, float z)
{
    if (m_selectedVerts.empty())
    {
        m_ang = 0;
        return;
    }

    float dy = y - m_vcenter.y;
    float dz = z - m_vcenter.z;

    m_ang = atan2(-dy, dz);
}

void Editor::calcVertsYAngle(float x, float z)
{
    if (m_selectedVerts.empty())
    {
        m_ang = 0;
        return;
    }

    float dx = x - m_vcenter.x;
    float dz = z - m_vcenter.z;

    m_ang = atan2(-dz, dx);
}

void Editor::calcVertsZAngle(float x, float y)
{
    if (m_selectedVerts.empty())
    {
        m_ang = 0;
        return;
    }

    float dx = x - m_vcenter.x;
    float dy = y - m_vcenter.y;

    m_ang = atan2(dx, -dy);
}

void Editor::rotateVertsX(const vec3& newpt)
{
    if (m_selectedVerts.empty()) return;

    float dy = newpt.y - m_vcenter.y;
    float dz = newpt.z - m_vcenter.z;

    float rot = atan2(-dy, dz) - m_ang;

    if (m_discreteRotate) rot = floor(rot / m_rotateStep) * m_rotateStep;

    float Cos = cos(rot);
    float Sin = -sin(rot);

    for (EditVertex* vert : m_selectedVerts) vert->rotateX(Cos, Sin, m_vcenter);

    fillVertsSelectionBuffer();
}

void Editor::rotateVertsY(const vec3& newpt)
{
    if (m_selectedVerts.empty()) return;

    float dx = newpt.x - m_vcenter.x;
    float dz = newpt.z - m_vcenter.z;

    float rot = atan2(-dz, dx) - m_ang;

    if (m_discreteRotate) rot = floor(rot / m_rotateStep) * m_rotateStep;

    float Cos = cos(rot);
    float Sin = -sin(rot);

    for (EditVertex* vert : m_selectedVerts) vert->rotateY(Cos, Sin, m_vcenter);

    fillVertsSelectionBuffer();
}

void Editor::rotateVertsZ(const vec3& newpt)
{
    if (m_selectedVerts.empty()) return;

    float dx = newpt.x - m_vcenter.x;
    float dy = newpt.y - m_vcenter.y;

    float rot = atan2(dx, -dy) - m_ang;

    if (m_discreteRotate) rot = floor(rot / m_rotateStep) * m_rotateStep;

    float Cos = cos(rot);
    float Sin = -sin(rot);

    for (EditVertex* vert : m_selectedVerts) vert->rotateZ(Cos, Sin, m_vcenter);

    fillVertsSelectionBuffer();
}

void Editor::scaleVertices(const vec3& scale)
{
    if (m_selectedVerts.empty()) return;

    vec3 scalevec;

    if (m_discreteScale)
    {
        scalevec.x = floor(scale.x / m_scaleStep) * m_scaleStep;
        scalevec.y = floor(scale.y / m_scaleStep) * m_scaleStep;
        scalevec.z = floor(scale.z / m_scaleStep) * m_scaleStep;
    }

    for (EditVertex* vert : m_selectedVerts) vert->scale(m_vcenter, m_discreteScale ? scalevec : scale);

    fillVertsSelectionBuffer();
}

void Editor::clearBlockSelection()
{
    for (Block* block : m_selectedBlocks) block->deselect();
    m_selectedBlocks.clear();

    m_pickedBlock = nullptr;
}

bool Editor::selectBlocks(const vec3& origin, const vec3& ray, vec3& pickPoint, float& pickDist, bool append)
{
    Block* pickedBlock = nullptr;

    for (Block* block : m_blocks)
    {
        float dist;
        vec3 point;

        if (block->pick(origin, ray, point, dist))
        {
            if (!pickedBlock || dist < pickDist)
            {
                pickedBlock = block;
                pickDist = dist;

                pickPoint = point;
            }
        }
    }

    if (pickedBlock)
    {    
        if (append)
        {
            if (pickedBlock->isSelected())
            {
                pickedBlock->deselect();
                m_selectedBlocks.remove(pickedBlock);

                m_pickedBlock = nullptr;
            }
            else
            {
                pickedBlock->select();
                m_selectedBlocks.append(pickedBlock);

                m_pickedBlock = pickedBlock;
            }
        }
        else
        {
            if (!pickedBlock->isSelected())
            {
                clearBlockSelection();

                pickedBlock->select();
                m_selectedBlocks.append(pickedBlock);
            }

            m_pickedBlock = pickedBlock;
        }
    }
    else
    {
        if (!append) clearBlockSelection();
    }

    if (m_pickedBlock && m_editMode == EditMode::Rotate)
    {
        for (Block* block : m_selectedBlocks) block->calculateMoveDiff(m_pickedBlock->pos());
    }
    else
    {
        if (m_editMode == EditMode::Scale && m_pickedBlock)
            for (Block* block : m_selectedBlocks) block->calculateMoveDiff(m_pickedBlock->pos());
        else
            for (Block* block : m_selectedBlocks) block->calculateMoveDiff(pickPoint);
    }

    onUpdate();

    return !m_selectedBlocks.empty();
}

bool Editor::selectBlocks2d(float x, float y, float scale, bool append, vec3& pickPoint, int i, int j, int k)
{
    Block* pickedBlock = nullptr;

    float pickDepth;

    pickPoint[k] = 0;

    for (Block* block : m_blocks)
    {
        float depth = 0;
        vec3 point;

        if (block->pick2d(x, y, scale, depth, i, j, k))
        {
            if (!pickedBlock || depth > pickDepth)
            {
                pickedBlock = block;
                pickDepth = depth;

                pickPoint[i] = x;
                pickPoint[j] = y;
            }
        }
    }

    if (pickedBlock)
    {
        if (append)
        {
            if (pickedBlock->isSelected())
            {
                pickedBlock->deselect();
                m_selectedBlocks.remove(pickedBlock);

                m_pickedBlock = nullptr;
            }
            else
            {
                pickedBlock->select();
                m_selectedBlocks.append(pickedBlock);

                m_pickedBlock = pickedBlock;
            }
        }
        else
        {
            if (!pickedBlock->isSelected())
            {
                clearBlockSelection();

                pickedBlock->select();
                m_selectedBlocks.append(pickedBlock);
            }

            m_pickedBlock = pickedBlock;
        }
    }
    else
    {
        if (!append) clearBlockSelection();
    }

    if (m_pickedBlock && m_editMode == EditMode::Rotate)
    {
        for (Block* block : m_selectedBlocks) block->calculateMoveDiff(m_pickedBlock->pos());
    }
    else
    {
        if (m_editMode == EditMode::Scale && m_pickedBlock)
            for (Block* block : m_selectedBlocks) block->calculateMoveDiff(m_pickedBlock->pos());
        else
            for (Block* block : m_selectedBlocks) block->calculateMoveDiff(pickPoint);
    }

    onUpdate();

    return !m_selectedBlocks.empty();
}

const vec3& Editor::blockSelectionCenter() const
{
    if (m_pickedBlock) return m_pickedBlock->pos();

    return {};
}

void Editor::calcBlockXAngle(float y, float z)
{
    if (!m_pickedBlock)
    {
        m_ang = 0;
        return;
    }

    const vec3& pos = m_pickedBlock->pos();

    float dy = y - pos.y;
    float dz = z - pos.z;

    m_ang = atan2(-dy, dz);
}

void Editor::calcBlockYAngle(float x, float z)
{
    if (!m_pickedBlock)
    {
        m_ang = 0;
        return;
    }

    const vec3& pos = m_pickedBlock->pos();

    float dx = x - pos.x;
    float dz = z - pos.z;

    m_ang = atan2(-dz, dx);
}

void Editor::calcBlockZAngle(float x, float y)
{
    if (!m_pickedBlock)
    {
        m_ang = 0;
        return;
    }

    const vec3& pos = m_pickedBlock->pos();

    float dx = x - pos.x;
    float dy = y - pos.y;

    m_ang = atan2(dx, -dy);
}

void Editor::moveBlocks(vec3 newpt)
{
    if (m_discreteMove)
    {
        const vec3& diff = m_pickedBlock->moveDiff();
        vec3 pos = newpt + diff;

        pos.x = round(pos.x / m_moveStep) * m_moveStep;
        pos.y = round(pos.y / m_moveStep) * m_moveStep;
        pos.z = round(pos.z / m_moveStep) * m_moveStep;

        newpt = pos - diff;
    }

    for (Block* block : m_selectedBlocks) block->pickMove(newpt);
}

void Editor::moveBlocks2d(float x, float y, int i, int k)
{
    if (m_discreteMove)
    {
        const vec3& diff = m_pickedBlock->moveDiff();
    
        float px = x + diff[i];
        float py = y + diff[k];

        px = round(px / m_moveStep) * m_moveStep;
        py = round(py / m_moveStep) * m_moveStep;

        x = px - diff[i];
        y = py - diff[k];
    }

    for (Block* block : m_selectedBlocks) block->pickMove2d(x, y, i, k);
}

void Editor::rotateBlocksX(const vec3& newpt)
{
    if (!m_pickedBlock) return;

    const vec3& bpos = m_pickedBlock->pos();

    float dy = newpt.y - bpos.y;
    float dz = newpt.z - bpos.z;

    float rot = atan2(-dy, dz) - m_ang;

    if (m_discreteRotate) rot = int(rot / m_rotateStep) * m_rotateStep;

    float Cos = cos(rot);
    float Sin = -sin(rot);

    if (fabs(Cos) < math::eps) Cos = 0.0f;
    if (fabs(Sin) < math::eps) Sin = 0.0f;

    const vec3& center = m_pickedBlock->pos();

    for (Block* block : m_selectedBlocks)
    {
        block->setXRot(rot);
        if (block != m_pickedBlock) block->pickRotateX(Cos, Sin, center);
    }
}

void Editor::rotateBlocksY(const vec3& newpt)
{
    if (!m_pickedBlock) return;

    const vec3& bpos = m_pickedBlock->pos();

    float dx = newpt.x - bpos.x;
    float dz = newpt.z - bpos.z;

    float rot = atan2(-dz, dx) - m_ang;

    if (m_discreteRotate) rot = int(rot / m_rotateStep) * m_rotateStep;

    float Cos = cos(rot);
    float Sin = -sin(rot);

    if (fabs(Cos) < math::eps) Cos = 0.0f;
    if (fabs(Sin) < math::eps) Sin = 0.0f;

    const vec3& center = m_pickedBlock->pos();

    for (Block* block : m_selectedBlocks)
    {
        block->setYRot(rot);
        if (block != m_pickedBlock) block->pickRotateY(Cos, Sin, center);
    }
}

void Editor::rotateBlocksZ(const vec3& newpt)
{
    if (!m_pickedBlock) return;

    const vec3& bpos = m_pickedBlock->pos();

    float dx = newpt.x - bpos.x;
    float dy = newpt.y - bpos.y;

    float rot = atan2(dx, -dy) - m_ang;

    if (m_discreteRotate) rot = int(rot / m_rotateStep) * m_rotateStep;

    float Cos = cos(rot);
    float Sin = -sin(rot);

    if (fabs(Cos) < math::eps) Cos = 0.0f;
    if (fabs(Sin) < math::eps) Sin = 0.0f;

    const vec3& center = m_pickedBlock->pos();

    for (Block* block : m_selectedBlocks)
    {
        block->setZRot(rot);
        if (block != m_pickedBlock) block->pickRotateZ(Cos, Sin, center);
    }
}

void Editor::scaleBlocks(const vec3& scale)
{
    if (!m_pickedBlock) return;

    vec3 scalevec;

    if (m_discreteScale)
    {
        scalevec.x = floor(scale.x / m_scaleStep) * m_scaleStep;
        scalevec.y = floor(scale.y / m_scaleStep) * m_scaleStep;
        scalevec.z = floor(scale.z / m_scaleStep) * m_scaleStep;
    }

    const vec3& center = m_pickedBlock->pos();

    for (Block* block : m_selectedBlocks) block->pickScale(center, m_discreteScale ? scalevec : scale);
}

void Editor::applyBlockRotation()
{
    for (Block* block : m_selectedBlocks) block->applyRotation();
}

void Editor::applyBlockScale()
{
    for (Block* block : m_selectedBlocks) block->applyScale();
}

void Editor::updateBlockBBoxes()
{
    for (Block* block : m_selectedBlocks) block->updateBBox();
}

void Editor::duplicateBlocks()
{
    if (m_selectedBlocks.empty()) return;

    EditList<Block> newBlocks;

    for (Block* block : m_selectedBlocks)
    {
        if (block->type() == BlockType::Edit) continue;

        Block* newBlock = new Block(*block);
        newBlock->setPos(block->pos() + DuplicateShift);

        m_blocks.append(newBlock);
        newBlocks.append(newBlock);

        newBlock->select();
    }

    if (!newBlocks.empty())
    {
        clearBlockSelection();
        m_selectedBlocks = std::move(newBlocks);
    }

    onUpdate();
}

void Editor::duplicateObjects()
{
    if (m_selectedObjects.empty()) return;

    EditList<Object> newObjects;

    for (Object* object : m_selectedObjects)
    {
        Object* newObject = object->clone();
        newObject->setPos(object->pos() + DuplicateShift);

        m_objects.append(newObject);
        newObjects.append(newObject);

        newObject->select();
    }

    if (!newObjects.empty())
    {
        clearObjectSelection();
        m_selectedObjects = std::move(newObjects);
    }

    onUpdate();
}

void Editor::duplicateSurfaces()
{
    if (m_selectedSurfaces.empty()) return;

    EditList<Surface> newSurfaces;

    for (Surface* surface : m_selectedSurfaces)
    {
        Surface* newSurface = surface->clone();
        newSurface->setPos(surface->pos() + DuplicateShift);

        m_surfaces.append(newSurface);
        newSurfaces.append(newSurface);

        newSurface->select();
    }

    if (!newSurfaces.empty())
    {
        clearSurfaceSelection();
        m_selectedSurfaces = std::move(newSurfaces);
    }

    onUpdate();
}

void Editor::deleteBlocks()
{
    for (auto it = m_selectedBlocks.begin(); it != m_selectedBlocks.end(); )
    {
        Block* block = *it;

        it++;

        if (block->type() != BlockType::Edit)
        {
            m_blocks.remove(block);
            m_selectedBlocks.remove(block);

            delete block;
        }
    }

    onUpdate();
}

bool Editor::deselectPolygon(EditPolygon* poly)
{
    for (auto it = m_selectedPolys.begin(); it != m_selectedPolys.end(); )
    {
        PolygonSelection* selection = *it;

        it++;

        if (poly->origin->surface)
        {
            if (poly->origin->surface == selection->origin->surface)
            {
                m_selectedPolys.remove(selection);
                delete selection;

                return true;
            }

            continue;
        }

        for (EditPolygon* selPoly : selection->origin->editPolygons)
        {
            if (selPoly == poly)
            {
                m_selectedPolys.remove(selection);
                delete selection;

                return true;
            }
        }
    }

    return false;
}

void Editor::selectPolygon(const vec3& origin, const vec3& ray, bool append)
{
    Block* pickBlock = nullptr;
    EditPolygon* pickPoly = nullptr;
    float mindist = 0;

    EditorSelectionType polyNum = selectedPolygonsNum();

    for (Block* block : m_blocks)
    {
        if (block->type() == BlockType::Edit) continue;

        float dist;
        EditPolygon* poly = block->pickPolygon(origin, ray, dist);

        if (poly)
        {
            if (!pickPoly || dist < mindist)
            {
                pickBlock = block;
                pickPoly = poly;
                mindist = dist;
            }
        }
    }

    if (pickPoly)
    {
        if (!append)
        {
            if (polyNum == EditorSelectionType::One && pickPoly->origin == m_selectedPolys.head()->origin)
            {
                m_selectedPolys.destroy();
            }
            else
            {
                m_selectedPolys.destroy();

                PolygonSelection* sel = new PolygonSelection(pickBlock, pickPoly->origin);
                m_selectedPolys.append(sel);
            }
        }
        else if (!deselectPolygon(pickPoly))
        {
            PolygonSelection* sel = new PolygonSelection(pickBlock, pickPoly->origin);
            m_selectedPolys.append(sel);
        }
    }
    else
    {
        if (!append) m_selectedPolys.destroy();
    }

    onPolygonSelectionUpdate();
}

void Editor::addDecal(const vec3& origin, const vec3& ray)
{
    Block* pickBlock = nullptr;
    EditPolygon* pickPoly = nullptr;

    float mindist = 0;

    for (Block* block : m_blocks)
    {
        if (block->type() == BlockType::Edit) continue;

        float dist;
        EditPolygon* poly = block->pickPolygon(origin, ray, dist);

        if (poly)
        {
            if (!pickPoly || dist < mindist)
            {
                pickBlock = block;
                pickPoly = poly;
                mindist = dist;
            }
        }
    }

    if (pickPoly)
    {
        vec3 pos = origin + ray * mindist;
        
        const vec3& ps = pickPoly->origin->s;
        const vec3& pt = pickPoly->origin->t;
      
        const vec3& norm = pickPoly->plane.xyz;

        vec3 s = ps - norm * (ps * norm);
        vec3 t = pt - norm * (pt * norm);

        s.normalize();
        t.normalize();

        //if ((norm ^ s) * t < 0) t = -t;

        Decal* decal = new Decal(loadMaterial(m_curmat));

        decal->setPos(pos);
        decal->setOrientation({ s, t, norm });
        decal->buildGeometry(pickPoly->origin->editPolygons, pickBlock->type() == BlockType::Subtruct);

        m_decals.append(decal);
    }
}

void Editor::clearObjectSelection()
{
    for (Object* object : m_selectedObjects) object->deselect();
    m_selectedObjects.clear();

    m_pickedObject = nullptr;
}

bool Editor::selectObjects(const vec3& origin, const vec3& ray, vec3& pickPoint, float& pickDist, bool append)
{
    Object* pickedObject = nullptr;

    for (Object* object : m_objects)
    {
        float dist;
        vec3 point;

        if (object->pick(origin, ray, point, dist))
        {
            if (!pickedObject || dist < pickDist)
            {
                pickedObject = object;
                pickDist = dist;

                pickPoint = point;
            }
        }
    }

    if (pickedObject)
    {
        if (append)
        {
            if (pickedObject->isSelected())
            {
                pickedObject->deselect();
                m_selectedObjects.remove(pickedObject);

                m_pickedObject = nullptr;
            }
            else
            {
                pickedObject->select();
                m_selectedObjects.append(pickedObject);

                m_pickedObject = pickedObject;
            }
        }
        else
        {
            if (!pickedObject->isSelected())
            {
                clearObjectSelection();

                pickedObject->select();
                m_selectedObjects.append(pickedObject);
            }

            m_pickedObject = pickedObject;
        }
    }
    else
    {
        if (!append) clearObjectSelection();
    }

    if (m_pickedObject && m_editMode == EditMode::Rotate)
    {
        for (Object* object : m_selectedObjects) object->calculateMoveDiff(m_pickedObject->pos());
    }
    else
    {
        if (m_editMode == EditMode::Scale && m_pickedObject)
            for (Object* object : m_selectedObjects) object->calculateMoveDiff(m_pickedObject->pos());
        else
            for (Object* object : m_selectedObjects) object->calculateMoveDiff(pickPoint);
    }

    onUpdate();
    onObjectSelectionUpdate();

    return !m_selectedObjects.empty();
}

bool Editor::selectObjects2d(float x, float y, float scale, bool append, vec3& pickPoint, int i, int j, int k)
{
    Object* pickedObject = nullptr;

    float pickDepth;

    pickPoint[k] = 0;

    for (Object* object : m_objects)
    {
        float depth = 0;
        vec3 point;

        if (object->pick2d(x, y, scale, depth, i, j, k))
        {
            if (!pickedObject || depth > pickDepth)
            {
                pickedObject = object;
                pickDepth = depth;

                pickPoint[i] = x;
                pickPoint[j] = y;
            }
        }
    }

    if (pickedObject)
    {
        if (append)
        {
            if (pickedObject->isSelected())
            {
                pickedObject->deselect();
                m_selectedObjects.remove(pickedObject);

                m_pickedObject = nullptr;
            }
            else
            {
                pickedObject->select();
                m_selectedObjects.append(pickedObject);

                m_pickedObject = pickedObject;
            }
        }
        else
        {
            if (!pickedObject->isSelected())
            {
                clearObjectSelection();

                pickedObject->select();
                m_selectedObjects.append(pickedObject);
            }

            m_pickedObject = pickedObject;
        }
    }
    else
    {
        if (!append) clearObjectSelection();
    }

    if (m_pickedObject && m_editMode == EditMode::Rotate)
    {
        for (Object* object : m_selectedObjects) object->calculateMoveDiff(m_pickedObject->pos());
    }
    else
    {
        if (m_editMode == EditMode::Scale && m_pickedObject)
            for (Object* object : m_selectedObjects) object->calculateMoveDiff(m_pickedObject->pos());
        else
            for (Object* object : m_selectedObjects) object->calculateMoveDiff(pickPoint);
    }

    onUpdate();
    onObjectSelectionUpdate();

    return !m_selectedObjects.empty();
}

void Editor::moveObjects(vec3 newpt)
{
    if (m_discreteMove)
    {
        const vec3& diff = m_pickedObject->moveDiff();
        vec3 pos = newpt + diff;

        pos.x = round(pos.x / m_moveStep) * m_moveStep;
        pos.y = round(pos.y / m_moveStep) * m_moveStep;
        pos.z = round(pos.z / m_moveStep) * m_moveStep;

        newpt = pos - diff;
    }

    for (Object* object : m_selectedObjects) object->pickMove(newpt);
}

void Editor::moveObjects2d(float x, float y, int i, int k)
{
    if (m_discreteMove)
    {
        const vec3& diff = m_pickedObject->moveDiff();

        float px = x + diff[i];
        float py = y + diff[k];

        px = round(px / m_moveStep) * m_moveStep;
        py = round(py / m_moveStep) * m_moveStep;

        x = px - diff[i];
        y = py - diff[k];
    }

    for (Object* object : m_selectedObjects) object->pickMove2d(x, y, i, k);
}

void Editor::calcObjectXAngle(float y, float z)
{
    if (!m_pickedObject)
    {
        m_ang = 0;
        return;
    }

    const vec3& pos = m_pickedObject->pos();

    float dy = y - pos.y;
    float dz = z - pos.z;

    m_ang = atan2(-dy, dz);
}

void Editor::calcObjectYAngle(float x, float z)
{
    if (!m_pickedObject)
    {
        m_ang = 0;
        return;
    }

    const vec3& pos = m_pickedObject->pos();

    float dx = x - pos.x;
    float dz = z - pos.z;

    m_ang = atan2(-dz, dx);
}

void Editor::calcObjectZAngle(float x, float y)
{
    if (!m_pickedObject)
    {
        m_ang = 0;
        return;
    }

    const vec3& pos = m_pickedObject->pos();

    float dx = x - pos.x;
    float dy = y - pos.y;

    m_ang = atan2(dx, -dy);
}

void Editor::rotateObjectsX(const vec3& newpt)
{
    if (!m_pickedObject) return;

    const vec3& bpos = m_pickedObject->pos();

    float dy = newpt.y - bpos.y;
    float dz = newpt.z - bpos.z;

    float rot = atan2(-dy, dz) - m_ang;

    if (m_discreteRotate) rot = int(rot / m_rotateStep) * m_rotateStep;

    float Cos = cos(rot);
    float Sin = -sin(rot);

    const vec3& center = m_pickedObject->pos();

    for (Object* object : m_selectedObjects)
    {
        object->setXRot(rot);
        if (object != m_pickedObject) object->pickRotateX(Cos, Sin, center);
    }
}

void Editor::rotateObjectsY(const vec3& newpt)
{
    if (!m_pickedObject) return;

    const vec3& bpos = m_pickedObject->pos();

    float dx = newpt.x - bpos.x;
    float dz = newpt.z - bpos.z;

    float rot = atan2(-dz, dx) - m_ang;

    if (m_discreteRotate) rot = int(rot / m_rotateStep) * m_rotateStep;

    float Cos = cos(rot);
    float Sin = -sin(rot);

    const vec3& center = m_pickedObject->pos();

    for (Object* object : m_selectedObjects)
    {
        object->setYRot(rot);
        if (object != m_pickedObject) object->pickRotateY(Cos, Sin, center);
    }
}

void Editor::rotateObjectsZ(const vec3& newpt)
{
    if (!m_pickedObject) return;

    const vec3& bpos = m_pickedObject->pos();

    float dx = newpt.x - bpos.x;
    float dy = newpt.y - bpos.y;

    float rot = atan2(dx, -dy) - m_ang;

    if (m_discreteRotate) rot = int(rot / m_rotateStep) * m_rotateStep;

    float Cos = cos(rot);
    float Sin = -sin(rot);

    const vec3& center = m_pickedObject->pos();

    for (Object* object : m_selectedObjects)
    {
        object->setZRot(rot);
        if (object != m_pickedObject) object->pickRotateZ(Cos, Sin, center);
    }
}

void Editor::applyObjectRotation()
{
    for (Object* object : m_selectedObjects) object->applyRotation();
}

void Editor::clearSurfaceSelection()
{
    for (Surface* surface : m_selectedSurfaces) surface->deselect();
    m_selectedSurfaces.clear();

    m_pickedSurface = nullptr;
}

bool Editor::selectSurfaces(const vec3& origin, const vec3& ray, vec3& pickPoint, float& pickDist, bool append)
{
    Surface* pickedSurface = nullptr;

    for (Surface* surface : m_surfaces)
    {
        float dist;
        vec3 point;

        if (surface->pick(origin, ray, point, dist))
        {
            if (!pickedSurface || dist < pickDist)
            {
                pickedSurface = surface;
                pickDist = dist;

                pickPoint = point;
            }
        }
    }

    if (pickedSurface)
    {
        if (append)
        {
            if (pickedSurface->isSelected())
            {
                pickedSurface->deselect();
                m_selectedSurfaces.remove(pickedSurface);

                m_pickedSurface = nullptr;
            }
            else
            {
                pickedSurface->select();
                m_selectedSurfaces.append(pickedSurface);

                m_pickedSurface = pickedSurface;
            }
        }
        else
        {
            if (!pickedSurface->isSelected())
            {
                clearSurfaceSelection();

                pickedSurface->select();
                m_selectedSurfaces.append(pickedSurface);
            }

            m_pickedSurface = pickedSurface;
        }
    }
    else
    {
        if (!append) clearSurfaceSelection();
    }

    if (m_pickedSurface && m_editMode == EditMode::Rotate)
    {
        for (Surface* surface : m_selectedSurfaces) surface->calculateMoveDiff(m_pickedSurface->pos());
    }
    else
    {
        if (m_editMode == EditMode::Scale && m_pickedSurface)
            for (Surface* surface : m_selectedSurfaces) surface->calculateMoveDiff(m_pickedSurface->pos());
        else
            for (Surface* surface : m_selectedSurfaces) surface->calculateMoveDiff(pickPoint);
    }

    onUpdate();
    onSurfaceSelectionUpdate();

    return !m_selectedSurfaces.empty();
}

bool Editor::selectSurfaces2d(float x, float y, float scale, bool append, vec3& pickPoint, int i, int j, int k)
{
    Surface* pickedSurface = nullptr;

    float pickDepth;

    pickPoint[k] = 0;

    for (Surface* surface : m_surfaces)
    {
        float depth = 0;
        vec3 point;

        if (surface->pick2d(x, y, scale, depth, i, j, k))
        {
            if (!pickedSurface || depth > pickDepth)
            {
                pickedSurface = surface;
                pickDepth = depth;

                pickPoint[i] = x;
                pickPoint[j] = y;
            }
        }
    }

    if (pickedSurface)
    {
        if (append)
        {
            if (pickedSurface->isSelected())
            {
                pickedSurface->deselect();
                m_selectedSurfaces.remove(pickedSurface);

                m_pickedSurface = nullptr;
            }
            else
            {
                pickedSurface->select();
                m_selectedSurfaces.append(pickedSurface);

                m_pickedSurface = pickedSurface;
            }
        }
        else
        {
            if (!pickedSurface->isSelected())
            {
                clearSurfaceSelection();

                pickedSurface->select();
                m_selectedSurfaces.append(pickedSurface);
            }

            m_pickedSurface = pickedSurface;
        }
    }
    else
    {
        if (!append) clearSurfaceSelection();
    }

    if (m_pickedSurface && m_editMode == EditMode::Rotate)
    {
        for (Surface* surface : m_selectedSurfaces) surface->calculateMoveDiff(m_pickedSurface->pos());
    }
    else
    {
        if (m_editMode == EditMode::Scale && m_pickedSurface)
            for (Surface* surface : m_selectedSurfaces) surface->calculateMoveDiff(m_pickedSurface->pos());
        else
            for (Surface* surface : m_selectedSurfaces) surface->calculateMoveDiff(pickPoint);
    }

    onUpdate();
    onSurfaceSelectionUpdate();

    return !m_selectedSurfaces.empty();
}

const vec3& Editor::surfaceSelectionCenter() const
{
    if (m_pickedSurface) return m_pickedSurface->pos();

    return {};
}

void Editor::moveSurfaces(vec3 newpt)
{
    if (m_discreteMove)
    {
        const vec3& diff = m_pickedSurface->moveDiff();
        vec3 pos = newpt + diff;

        pos.x = round(pos.x / m_moveStep) * m_moveStep;
        pos.y = round(pos.y / m_moveStep) * m_moveStep;
        pos.z = round(pos.z / m_moveStep) * m_moveStep;

        newpt = pos - diff;
    }

    for (Surface* surface : m_selectedSurfaces) surface->pickMove(newpt);
}

void Editor::moveSurfaces2d(float x, float y, int i, int k)
{
    if (m_discreteMove)
    {
        const vec3& diff = m_pickedSurface->moveDiff();

        float px = x + diff[i];
        float py = y + diff[k];

        px = round(px / m_moveStep) * m_moveStep;
        py = round(py / m_moveStep) * m_moveStep;

        x = px - diff[i];
        y = py - diff[k];
    }

    for (Surface* surface : m_selectedSurfaces) surface->pickMove2d(x, y, i, k);
}

void Editor::calcSurfaceXAngle(float y, float z)
{
    if (!m_pickedSurface)
    {
        m_ang = 0;
        return;
    }

    const vec3& pos = m_pickedSurface->pos();

    float dy = y - pos.y;
    float dz = z - pos.z;

    m_ang = atan2(-dy, dz);
}

void Editor::calcSurfaceYAngle(float x, float z)
{
    if (!m_pickedSurface)
    {
        m_ang = 0;
        return;
    }

    const vec3& pos = m_pickedSurface->pos();

    float dx = x - pos.x;
    float dz = z - pos.z;

    m_ang = atan2(-dz, dx);
}

void Editor::calcSurfaceZAngle(float x, float y)
{
    if (!m_pickedSurface)
    {
        m_ang = 0;
        return;
    }

    const vec3& pos = m_pickedSurface->pos();

    float dx = x - pos.x;
    float dy = y - pos.y;

    m_ang = atan2(dx, -dy);
}

void Editor::rotateSurfacesX(const vec3& newpt)
{
    if (!m_pickedSurface) return;

    const vec3& bpos = m_pickedSurface->pos();

    float dy = newpt.y - bpos.y;
    float dz = newpt.z - bpos.z;

    float rot = atan2(-dy, dz) - m_ang;

    if (m_discreteRotate) rot = int(rot / m_rotateStep) * m_rotateStep;

    float Cos = cos(rot);
    float Sin = -sin(rot);

    const vec3& center = m_pickedSurface->pos();

    for (Surface* surface : m_selectedSurfaces)
    {
        surface->setXRot(rot);
        if (surface != m_pickedSurface) surface->pickRotateX(Cos, Sin, center);
    }
}

void Editor::rotateSurfacesY(const vec3& newpt)
{
    if (!m_pickedSurface) return;

    const vec3& bpos = m_pickedSurface->pos();

    float dx = newpt.x - bpos.x;
    float dz = newpt.z - bpos.z;

    float rot = atan2(-dz, dx) - m_ang;

    if (m_discreteRotate) rot = int(rot / m_rotateStep) * m_rotateStep;

    float Cos = cos(rot);
    float Sin = -sin(rot);

    const vec3& center = m_pickedSurface->pos();

    for (Surface* surface : m_selectedSurfaces)
    {
        surface->setYRot(rot);
        if (surface != m_pickedSurface) surface->pickRotateY(Cos, Sin, center);
    }
}

void Editor::rotateSurfacesZ(const vec3& newpt)
{
    if (!m_pickedSurface) return;

    const vec3& bpos = m_pickedSurface->pos();

    float dx = newpt.x - bpos.x;
    float dy = newpt.y - bpos.y;

    float rot = atan2(dx, -dy) - m_ang;

    if (m_discreteRotate) rot = int(rot / m_rotateStep) * m_rotateStep;

    float Cos = cos(rot);
    float Sin = -sin(rot);

    const vec3& center = m_pickedSurface->pos();

    for (Surface* surface : m_selectedSurfaces)
    {
        surface->setZRot(rot);
        if (surface != m_pickedSurface) surface->pickRotateZ(Cos, Sin, center);
    }
}

void Editor::scaleSurfaces(const vec3& scale)
{
    if (!m_pickedSurface) return;

    vec3 scalevec;

    if (m_discreteScale)
    {
        scalevec.x = floor(scale.x / m_scaleStep) * m_scaleStep;
        scalevec.y = floor(scale.y / m_scaleStep) * m_scaleStep;
        scalevec.z = floor(scale.z / m_scaleStep) * m_scaleStep;
    }

    const vec3& center = m_pickedSurface->pos();

    for (Surface* surface : m_selectedSurfaces) surface->pickScale(center, m_discreteScale ? scalevec : scale);
}

void Editor::applySurfaceRotation()
{
    for (Surface* surface : m_selectedSurfaces) surface->applyRotation();
}

void Editor::applySurfaceScale()
{
    for (Surface* surface : m_selectedSurfaces) surface->applyScale();
}

bool Editor::selectCp(const vec3& origin, const vec3& ray, vec3& pickPoint, bool append)
{
    vec3* pickedCp = nullptr;
    float pickDist;

    Surface* pickSurface = nullptr;

    for (Surface* surface : m_selectedSurfaces)
    {
        float dist;

        vec3* cp = surface->pickCp(origin, ray, dist);

        if (cp)
        {
            if (!pickedCp || dist < pickDist)
            {
                pickedCp = cp;
                pickDist = dist;
                pickSurface = surface;
            }
        }
    }

    if (pickedCp)
    {
        ControlPoint* controlPt = findSelectedCp(pickedCp, pickSurface);

        if (controlPt)
        {
            if (append)
            {
                m_selectedCps.remove(controlPt);
                delete controlPt;

                fillCpSelectionBuffer();
            }
        }
        else
        {
            if (!append) clearCpSelection();

            controlPt = new ControlPoint(pickedCp, pickSurface);
            m_selectedCps.append(controlPt);
            m_controlPoints.push(*pickedCp + pickSurface->pos());
        }

        m_pickedCp = controlPt;

        pickPoint = *pickedCp + pickSurface->pos();

        if (m_editMode == EditMode::Move)
        {
            for (ControlPoint* cp : m_selectedCps)
            {
                vec3 pos = *(cp->point) + cp->owner->pos();
                cp->diff = pos - pickPoint;
            }

        }
        else calcSelectedCpCenter();

        onUpdate();
        return true;
    }
    else
    {
        m_pickedCp = nullptr;

        if (!append) clearCpSelection();

        onUpdate();
        return false;
    }

    return false;
}

bool Editor::selectCp2d(float x, float y, float scale, bool append, int i, int k)
{
    bool result = false;

    Surface* pickSurface = nullptr;

    bool select = false;

    if (!append)
    {
        if (!testSelectedCp2d(x, y, scale, i, k))
        {
            m_selectedCps.destroy();
            select = true;

        }
        else result = true;
    }
    else
    {
        if (!deselectCp2d(x, y, scale, i, k)) select = true;
    }

    if (select)
    {
        for (Surface* surface : m_selectedSurfaces)
        {
            float dist;

            bool pick = surface->pickCp2d(x, y, scale, i, k, m_selectedCps);

            if (pick)
            {
                pickSurface = surface;
                result = true;
            }
        }
    }

    if (m_editMode == EditMode::Move)
    {
        float mindist = std::numeric_limits<float>::infinity();

        for (ControlPoint* cp : m_selectedCps)
        {
            vec3 pos = *(cp->point) + cp->owner->pos();
            cp->diff[i] = pos[i] - x;
            cp->diff[k] = pos[k] - y;

            float dist = sqrt(cp->diff[i] * cp->diff[i] + cp->diff[k] * cp->diff[k]);

            if (dist < mindist)
            {
                mindist = dist;
                m_pickedCp = cp;
            }
        }

    }
    else calcSelectedCpCenter();

    fillCpSelectionBuffer();

    onUpdate();

    return result;
}

void Editor::moveCp(vec3 newpt)
{
    if (m_discreteMove)
    {
        const vec3& diff = m_pickedCp->diff;
        vec3 pos = newpt + diff;

        pos.x = round(pos.x / m_moveStep) * m_moveStep;
        pos.y = round(pos.y / m_moveStep) * m_moveStep;
        pos.z = round(pos.z / m_moveStep) * m_moveStep;

        newpt = pos - diff;
    }

    std::set<Surface*> changedSurfs;

    for (ControlPoint* cp : m_selectedCps)
    {
        *cp->point = newpt + cp->diff - cp->owner->pos();
        changedSurfs.insert(cp->owner);
    }

    for (auto surf : changedSurfs) surf->update();

    fillCpSelectionBuffer();
}

void Editor::moveCp2d(float x, float y, int i, int k)
{
    if (m_discreteMove)
    {
        const vec3& diff = m_pickedCp->diff;

        float px = x + diff[i];
        float py = y + diff[k];

        px = round(px / m_moveStep) * m_moveStep;
        py = round(py / m_moveStep) * m_moveStep;

        x = px - diff[i];
        y = py - diff[k];
    }

    std::set<Surface*> changedSurfs;

    for (ControlPoint* cp : m_selectedCps)
    {
        const vec3& pos = cp->owner->pos();

        (*cp->point)[i] = x + cp->diff[i] - pos[i];
        (*cp->point)[k] = y + cp->diff[k] - pos[k];

        changedSurfs.insert(cp->owner);
    }

    for (auto surf : changedSurfs) surf->update();

    fillCpSelectionBuffer();
}

void Editor::calcCpXAngle(float y, float z)
{
    if (m_selectedCps.empty())
    {
        m_ang = 0;
        return;
    }

    float dy = y - m_cpcenter.y;
    float dz = z - m_cpcenter.z;

    m_ang = atan2(-dy, dz);
}

void Editor::calcCpYAngle(float x, float z)
{
    if (m_selectedCps.empty())
    {
        m_ang = 0;
        return;
    }

    float dx = x - m_cpcenter.x;
    float dz = z - m_cpcenter.z;

    m_ang = atan2(-dz, dx);
}

void Editor::calcCpZAngle(float x, float y)
{
    if (m_selectedCps.empty())
    {
        m_ang = 0;
        return;
    }

    float dx = x - m_cpcenter.x;
    float dy = y - m_cpcenter.y;

    m_ang = atan2(dx, -dy);
}

void Editor::rotateCpX(const vec3& newpt)
{
    if (m_selectedCps.empty()) return;

    float dy = newpt.y - m_cpcenter.y;
    float dz = newpt.z - m_cpcenter.z;

    float rot = atan2(-dy, dz) - m_ang;

    if (m_discreteRotate) rot = floor(rot / m_rotateStep) * m_rotateStep;

    float Cos = cos(rot);
    float Sin = -sin(rot);

    std::set<Surface*> changedSurfs;

    for (ControlPoint* cp : m_selectedCps)
    {
        cp->rotateX(Cos, Sin, m_cpcenter);
        changedSurfs.insert(cp->owner);
    }

    for (auto surf : changedSurfs) surf->update();

    fillCpSelectionBuffer();
}

void Editor::rotateCpY(const vec3& newpt)
{
    if (m_selectedCps.empty()) return;

    float dx = newpt.x - m_cpcenter.x;
    float dz = newpt.z - m_cpcenter.z;

    float rot = atan2(-dz, dx) - m_ang;

    if (m_discreteRotate) rot = floor(rot / m_rotateStep) * m_rotateStep;

    float Cos = cos(rot);
    float Sin = -sin(rot);

    std::set<Surface*> changedSurfs;

    for (ControlPoint* cp : m_selectedCps)
    {
        cp->rotateY(Cos, Sin, m_cpcenter);
        changedSurfs.insert(cp->owner);
    }

    for (auto surf : changedSurfs) surf->update();

    fillCpSelectionBuffer();
}

void Editor::rotateCpZ(const vec3& newpt)
{
    if (m_selectedCps.empty()) return;

    float dx = newpt.x - m_cpcenter.x;
    float dy = newpt.y - m_cpcenter.y;

    float rot = atan2(dx, -dy) - m_ang;

    if (m_discreteRotate) rot = floor(rot / m_rotateStep) * m_rotateStep;

    float Cos = cos(rot);
    float Sin = -sin(rot);

    std::set<Surface*> changedSurfs;

    for (ControlPoint* cp : m_selectedCps)
    {
        cp->rotateZ(Cos, Sin, m_cpcenter);
        changedSurfs.insert(cp->owner);
    }

    for (auto surf : changedSurfs) surf->update();

    fillCpSelectionBuffer();
}

void Editor::scaleCp(const vec3& scale)
{
    if (m_selectedCps.empty()) return;

    vec3 scalevec;

    if (m_discreteScale)
    {
        scalevec.x = floor(scale.x / m_scaleStep) * m_scaleStep;
        scalevec.y = floor(scale.y / m_scaleStep) * m_scaleStep;
        scalevec.z = floor(scale.z / m_scaleStep) * m_scaleStep;
    }

    std::set<Surface*> changedSurfs;

    for (ControlPoint* cp : m_selectedCps)
    {
        cp->scale(m_cpcenter, m_discreteScale ? scalevec : scale);
        changedSurfs.insert(cp->owner);
    }

    for (auto surf : changedSurfs) surf->update();

    fillCpSelectionBuffer();
}

void Editor::surfaceIntersect(const vec3& origin, const vec3& ray)
{
    float dist;
    m_pointSurface = false;

    for (PolygonSelection* poly : m_selectedPolys)
    {
        EditSurface* surface = poly->origin->surface.get();

        if (!surface) continue;

        vec3 localOrigin = origin - poly->owner->pos();

        float surfdist;

        if (surface->pick(localOrigin, ray, surfdist))
        {
            if (!m_pointSurface || surfdist < dist)
            {
                dist = surfdist;
                m_pointSurface = true;
            }
        }
    }

    if (m_pointSurface) m_surfaceIntersect = origin + ray * dist;
}

void Editor::surfaceDisplace(const vec3& origin, const vec3& ray)
{
    bool pick = false;

    float pickDist;
    vec3 pickPoint;

    for (PolygonSelection* poly : m_selectedPolys)
    {
        EditSurface* surface = poly->origin->surface.get();

        if (!surface) continue;
    
        vec3 localOrigin = origin - poly->owner->pos();

        float surfdist;

        if (surface->pick(localOrigin, ray, surfdist))
        {
            if (!pick || surfdist < pickDist)
            {
                pickDist = surfdist;
                pick = true;
            }
        }
    }

    if (pick)
    {
        vec3 pickPoint = origin + ray * pickDist;

        m_pointSurface = true;
        m_surfaceIntersect = pickPoint;

        if (m_displaceMode == DisplaceMode::Smooth)
        {           
            SurfaceGraph* surfaceGraph = nullptr;

            for (PolygonSelection* poly : m_selectedPolys)
            {
                EditSurface* surface = poly->origin->surface.get();

                if (!surface) continue;
            
                surfaceGraph = surface->surfaceGraph();
                if (surfaceGraph) break;
            }

            if (!surfaceGraph) surfaceGraph = buildSurfaceGraph();

            surfaceGraph->smooth(pickPoint, m_displacePower, m_displaceRadius);
            
            return;
        }

        for (PolygonSelection* poly : m_selectedPolys)
        {
            EditSurface* surface = poly->origin->surface.get();

            if (!surface) continue;

            const BBox& bbox = surface->bbox();

            vec3 spos = (bbox.min + bbox.max) * 0.5;
            vec3 sbox = bbox.max - spos;

            if (!AABBTest(spos + poly->owner->pos(), sbox, pickPoint, { m_displaceRadius, m_displaceRadius, m_displaceRadius })) continue;

            float power = m_displacePower * 0.001;

            switch (m_displaceMode)
            {
            case DisplaceMode::Up:
                surface->displace(pickPoint - poly->owner->pos(), power, m_displaceRadius);
            break;
            case DisplaceMode::Down:
                surface->displace(pickPoint - poly->owner->pos(), -power, m_displaceRadius);
            break;
            case DisplaceMode::Paint:
                surface->paintLayer(pickPoint - poly->owner->pos(), m_displaceRadius, m_paintLayer - 1);
            break;
            }
        }
    }
}

void Editor::addSurfaceDetails(size_t layer, const std::string& model, const std::string& material, float density)
{
    for (PolygonSelection* poly : m_selectedPolys)
    {
        EditSurface* surface = poly->origin->surface.get();

        if (!surface) continue;
    
        surface->addDetails(layer, model, material, density);
    }
}

void Editor::clearSurfaceDetails()
{
    for (PolygonSelection* poly : m_selectedPolys)
    {
        EditSurface* surface = poly->origin->surface.get();

        if (!surface) continue;

        surface->clearDetails();
    }
}

void Editor::deleteObjects()
{
    for (auto it = m_selectedObjects.begin(); it != m_selectedObjects.end(); )
    {
        Object* object = *it;

        it++;

        m_objects.remove(object);
        delete object;
    }

    m_selectedObjects.clear();

    onUpdate();
}

void Editor::deleteSurfaces()
{
    for (auto it = m_selectedSurfaces.begin(); it != m_selectedSurfaces.end(); )
    {
        Surface* surface = *it;

        it++;

        m_surfaces.remove(surface);
        delete surface;
    }

    m_selectedSurfaces.clear();

    onUpdate();
}

Material* Editor::loadMaterial(const std::string& name)
{
    bool exist = ResourceManager::MaterialExist(name);

    Material* mat = ResourceManager::GetMaterial(name);

    if (mat && !exist)
    {
        mat->name = name;
        //m_console.addMaterial(name, mat);
    }

    return mat;
}

void Editor::calculateFrustrum(const ViewCamera& camera, const mat4& projMat, vec3* frustum)
{
    static const vec2 quad[] =
    {
        { -1.0f, 1.0f },
        { 1.0f, 1.0f },
        { 1.0f, -1.0f },
        { -1.0f, -1.0f }
    };

    float fovx = 1.0 / projMat[0][0];
    float fovy = 1.0 / projMat[1][1];

    for (int i = 0; i < 4; i++)
    {
        frustum[i] = camera.basis() * vec3(quad[i].x * fovx, quad[i].y * fovy, 1.0) + camera.pos();
    }
}

void Editor::initGridGeometry()
{
    Render::RenderingPipeline::Init();

    std::vector<vec3> grid;

    for (int i = -GridSize; i <= GridSize; i++)
    {
        grid.emplace_back(i * GridScale, 0, -GridSize * GridScale);
        grid.emplace_back(i * GridScale, 0, GridSize * GridScale);
    }

    for (int i = -GridSize; i <= GridSize; i++)
    {
        grid.emplace_back(-GridSize * GridScale, 0, i * GridScale);
        grid.emplace_back(GridSize * GridScale, 0, i * GridScale);
    }

    m_gridBuffer.setData(grid.data(), grid.size());

    std::vector<vec3> fineGrid;

    static constexpr float FineGridDist = (float)GridScale / FineGridGran;

    for (int i = -FineGridSize; i < FineGridSize; i++)
    {
        for (int k = 0; k < (FineGridGran - 1); k++)
        {
            fineGrid.emplace_back(i * GridScale + (k + 1) * FineGridDist, 0, -FineGridSize * GridScale);
            fineGrid.emplace_back(i * GridScale + (k + 1) * FineGridDist, 0, FineGridSize * GridScale);
        }
    }

    for (int i = -FineGridSize; i < FineGridSize; i++)
    {
        for (int k = 0; k < (FineGridGran - 1); k++)
        {
            fineGrid.emplace_back(-FineGridSize * GridScale, 0, i * GridScale + (k + 1) * FineGridDist);
            fineGrid.emplace_back(FineGridSize * GridScale, 0, i * GridScale + (k + 1) * FineGridDist);
        }
    }

    m_fineGridBuffer.setData(fineGrid.data(), fineGrid.size());
}

void Editor::drawGrid(float x, float y)
{
    Render::CommandList& commandList = m_renderContext.commandList(rc_regular);

    vec3 gridPos = { floorf(x / GridMoveStep) * GridMoveStep, 0, floorf(y / GridMoveStep) * GridMoveStep };

    commandList.setConstant(1, mat4::Translate(gridPos));
    commandList.setTopology(Render::topology_linelist);

    commandList.setConstant(2, vec4(0.4, 0.4, 0.4, 1.0));
    commandList.setConstant(3, 1.0f);
    commandList.bindVertexBuffer(m_fineGridBuffer);
    commandList.draw(m_fineGridBuffer.size());

    commandList.setConstant(2, vec4(0.6, 0.6, 0.6, 1.0));
    commandList.setConstant(3, 2.0f);
    commandList.bindVertexBuffer(m_gridBuffer);
    commandList.draw(m_gridBuffer.size());
}

void Editor::drawGridOrtho(const mat4& gridMat, float x, float y)
{
    Render::CommandList& commandList = m_renderContext.commandList(rc_regular);

    vec3 gridPos = { floorf(x / GridMoveStep) * GridMoveStep, 0, floorf(y / GridMoveStep) * GridMoveStep };

    commandList.setConstant(1, gridMat);
    commandList.setTopology(Render::topology_linelist);

    commandList.setConstant(2, vec4(0.4, 0.4, 0.4, 1.0));
    commandList.setConstant(3, 1.0f);
    commandList.bindVertexBuffer(m_fineGridBuffer);
    commandList.draw(m_fineGridBuffer.size());

    commandList.setConstant(2, vec4(0.6, 0.6, 0.6, 1.0));
    commandList.setConstant(3, 2.0f);
    commandList.bindVertexBuffer(m_gridBuffer);
    commandList.draw(m_gridBuffer.size());
}

void Editor::display(Render::FrameBuffer& frameBuffer, const ViewCamera& camera, const mat4& projMat, int width, int height)
{
    //vec3 frustum[4];
    Render::Frustum frustum;

    //calculateFrustrum(camera, projMat, frustum);

    const vec3& campos = camera.pos();
    const vec3& camdir = camera.direction();
    float camdist = camdir * campos;

    vec4 screenPlane = vec4(camdir, -camdist);

    Render::CommandList& commandList = m_renderContext.commandList(rc_regular);
    Render::CommandList& colorCM = m_renderContext.commandList(rc_color);
    Render::CommandList& lineCM = m_renderContext.commandList(rc_line);

    mat4 projViewMat = projMat * camera.viewMat();

    m_sceneConstantBuffer->projViewMat = projViewMat;
    m_sceneConstantBuffer->worldMat = camera.rotMat();
    m_sceneConstantBuffer->fovx = width;
    m_sceneConstantBuffer->fovy = height;

    frustum.update(projViewMat);

    Render::RenderingPipeline::StartRender(commandList, Render::RenderingPipeline::rm_color_line);
    commandList.bindConstantBuffer(0, m_sceneConstantBuffer);
    commandList.setViewport(width, height);

    commandList.barrier(frameBuffer.writeBarriers());
    commandList.bindFrameBuffer(frameBuffer);
    commandList.clearDepth(frameBuffer, 1.0f);
    commandList.clearBuffer(frameBuffer, { 0.2f, 0.2f, 0.2f, 1.0f });

    Render::RenderingPipeline::StartRender(colorCM, Render::RenderingPipeline::rm_color);
    colorCM.bindConstantBuffer(0, m_sceneConstantBuffer);
    colorCM.setViewport(width, height);
    colorCM.bindFrameBuffer(frameBuffer);
    colorCM.setTopology(Render::topology_trianglelist);

    Render::RenderingPipeline::StartRender(lineCM, Render::RenderingPipeline::rm_color_line);
    lineCM.bindConstantBuffer(0, m_sceneConstantBuffer);
    lineCM.setViewport(width, height);
    lineCM.bindFrameBuffer(frameBuffer);
    lineCM.setTopology(Render::topology_linelist);

    const vec3& viewpos = camera.pos();
    drawGrid(viewpos.x, viewpos.z);

    commandList.setRenderMode(Render::RenderingPipeline::rm_simple);
    commandList.setTopology(Render::topology_trianglestrip);
    commandList.setConstant(1, mat4());
    commandList.setConstant(2, vec4(1.0, 1.0, 1.0, 1.0));
    
    for (const Block* block : m_blocks)
    {
        if (block->type() == BlockType::Edit) continue;

        const vec3& blockPos = block->pos();
        const BBox& blockBox = block->bbox();

        BBox bbox = { blockBox.min + blockPos, blockBox.max + blockPos };

        if (!frustum.test(bbox)) continue;

        block->displayGeometry(commandList);
    }

    commandList.setRenderMode(Render::RenderingPipeline::rm_simple_decal);
    commandList.setConstant(2, vec4(1.0, 1.0, 1.0, 1.0));
    for (const Decal* decal : m_decals) decal->display(commandList);

    commandList.setRenderMode(Render::RenderingPipeline::rm_color_line);
    commandList.setTopology(Render::topology_linelist);
    commandList.setConstant(3, 1.0f);

    for (const Block* block : m_blocks)
    {
        if (block->isSelected()) continue;

        const vec3& blockPos = block->pos();
        const BBox& blockBox = block->bbox();

        BBox bbox = { blockBox.min + blockPos, blockBox.max + blockPos };

        if (!frustum.test(bbox)) continue;

        block->display(commandList);
    }

    commandList.setConstant(3, 2.0f);

    for (const Block* block : m_blocks)
    {
        if (!block->isSelected()) continue;

        const vec3& blockPos = block->pos();
        const BBox& blockBox = block->bbox();

        BBox bbox = { blockBox.min + blockPos, blockBox.max + blockPos };

        if (!frustum.test(bbox)) continue;

        block->display(commandList);
    }

    if (m_editType == EditType::Decals)
    {
        commandList.setConstant(3, 1.0f);

        for (const Decal* decal : m_decals) decal->displayWire(commandList);
    }

    if (m_editType == EditType::Polygons && !m_selectedPolys.empty())
    {
        commandList.setRenderMode(Render::RenderingPipeline::RenderMode::rm_color);
        commandList.setTopology(Render::topology_trianglestrip);
        commandList.setConstant(1, mat4());
        commandList.setConstant(2, vec4(0.0, 1.0, 0.0, 0.5));

        const Block* curblock = nullptr;

        for (PolygonSelection* poly : m_selectedPolys)
        {
            if (poly->origin->surface) continue;

            const Block* block = poly->owner;

            if (block != curblock)
            {
                curblock = block;
                block->bindVertexBuffer(commandList);
            }

            block->displayGeometry(commandList, poly->origin->displayList);
        }
    }

    if (m_showPortals && m_map.hasPortals())
    {
        commandList.setRenderMode(Render::RenderingPipeline::RenderMode::rm_color);
        commandList.setTopology(Render::topology_trianglestrip);
        commandList.setConstant(1, mat4());

        m_map.displayPortals(commandList);
    }

    Object::ResetObjects();

    commandList.setRenderMode(Render::RenderingPipeline::rm_simple);
    commandList.setTopology(Render::topology_trianglelist);

    for (Object* object : m_objects) object->display(m_renderContext);

    Object::DisplayObjects(commandList);

    // Parametric surfaces
    commandList.setRenderMode(Render::RenderingPipeline::rm_simple);
    commandList.setTopology(Render::topology_trianglelist);

    for (Surface* surface : m_surfaces)
    {
        if (surface->isSelected()) commandList.setConstant(2, vec4(0.0, 0.8, 0.0, 1.0));
        else commandList.setConstant(2, vec4(1.0, 1.0, 1.0, 1.0));

        surface->display(commandList);
    }

    commandList.setRenderMode(Render::RenderingPipeline::rm_color_line);
    commandList.setTopology(Render::topology_linelist);

    for (Surface* surface : m_surfaces)
    {
        if (surface->isSelected())
        {
            commandList.setConstant(2, vec4(0.0, 0.0, 0.8, 1.0));
            commandList.setConstant(3, 2.0f);
        }
        else
        {
            commandList.setConstant(2, vec4(0.0, 0.0, 0.5, 1.0));
            commandList.setConstant(3, 1.0f);
        }

        surface->displayControlMesh(commandList);
    }

    // Subdivision surfaces
    commandList.setRenderMode(Render::RenderingPipeline::rm_simple);
    commandList.setTopology(Render::topology_trianglelist);
    commandList.setConstant(2, vec4(1.0, 1.0, 1.0, 1.0));

    for (const Block* block : m_blocks) block->displaySurfaces(commandList);

    commandList.setRenderMode(Render::RenderingPipeline::rm_simple_layered);

    for (const Block* block : m_blocks) block->displaySurfaceLayers(commandList);

    if ((m_editType == EditType::Polygons || m_editType == EditType::Displace) && !m_selectedPolys.empty())
    {
        float alpha = m_editType == EditType::Polygons ? 0.5 : 0.2;

        bool surfaceRange = m_editType == EditType::Displace && m_pointSurface;

        commandList.setRenderMode(surfaceRange ? 
                                  Render::RenderingPipeline::RenderMode::rm_color_range :
                                  Render::RenderingPipeline::RenderMode::rm_color);

        commandList.setTopology(Render::topology_trianglelist);

        if (surfaceRange)
        {
            ColorRangeParam param = { vec4(0.0, 1.0, 0.0, alpha),
                                      vec3(0.0, 0.0, 1.0),
                                      m_displaceRadius,
                                      m_surfaceIntersect,
                                      0 };

            commandList.setConstant(2, param);
        } 
        else
            commandList.setConstant(2, vec4(0.0, 1.0, 0.0, alpha));

        for (PolygonSelection* poly : m_selectedPolys)
        {
            if (!poly->origin->surface) continue;

            const Block* block = poly->owner;

            commandList.setConstant(1, mat4::Translate(block->pos()));
            poly->origin->surface->display(commandList);
        }
    }

    // Points
    if (m_editType == EditType::ControlPoints)
    {
        if (!m_selectedSurfaces.empty())
        {
            commandList.clearDepth(frameBuffer, 1.0f);
            commandList.setRenderMode(Render::RenderingPipeline::RenderMode::rm_color_point);
            commandList.setTopology(Render::topology_pointlist);
            commandList.setConstant(2, vec4(0.0, 1.0, 1.0, 1.0));

            for (const Surface* surface : m_selectedSurfaces) surface->displayControlPoints(commandList);
        }

        if (!m_selectedCps.empty())
        {
            commandList.setConstant(1, mat4());
            commandList.setConstant(2, vec4(1.0, 0.0, 0.0, 1.0));
            commandList.bindVertexBuffer(m_controlPoints);
            commandList.draw(m_controlPoints.size());
        }
    }

    if (m_editType == EditType::Vertices)
    {
        if (!m_selectedBlocks.empty())
        {
            commandList.clearDepth(frameBuffer, 1.0f);
            commandList.setRenderMode(Render::RenderingPipeline::RenderMode::rm_color_point);
            commandList.setTopology(Render::topology_pointlist);
            commandList.setConstant(2, vec4(1.0, 1.0, 0.0, 1.0));

            for (const Block* block : m_selectedBlocks) block->displayVertices(commandList);
        }

        if (!m_selectedVerts.empty())
        {
            commandList.setConstant(1, mat4());
            commandList.setConstant(2, vec4(1.0, 0.0, 0.0, 1.0));
            commandList.bindVertexBuffer(m_blockVerts);
            commandList.draw(m_blockVerts.size());
        }
    }

    //commandList.barrier(frameBuffer.readBarriers());

    commandList.finish();
    Render::D3DInstance::GetInstance().execute(commandList);

    //lineCM.barrier(frameBuffer.readBarriers());

    //lineCM.finish();
    //Render::D3DInstance::GetInstance().execute(lineCM);

    colorCM.barrier(frameBuffer.readBarriers());

    colorCM.finish();
    Render::D3DInstance::GetInstance().execute(colorCM);
}

void Editor::displayOrtho(Render::FrameBuffer& frameBuffer, 
                          const mat4& viewProj,
                          const mat4& viewMat,
                          const mat4& gridMat, 
                          const vec3& viewpos, 
                          float scale, 
                          int width, 
                          int height)
{
    Render::CommandList& commandList = m_renderContext.commandList(rc_regular);

    mat4 viewPojMat = viewProj * mat4::Scale(scale) * mat4::Translate(viewpos);

    m_sceneConstantBuffer->projViewMat = viewPojMat;
    m_sceneConstantBuffer->worldMat = viewMat;

    Render::RenderingPipeline::StartRender(commandList, Render::RenderingPipeline::rm_color_line);
    commandList.bindConstantBuffer(0, m_sceneConstantBuffer);
    commandList.setViewport(width, height);

    commandList.barrier(frameBuffer.writeBarriers());
    commandList.bindFrameBuffer(frameBuffer);
    commandList.clearBuffer(frameBuffer, { 0.2f, 0.2f, 0.2f, 1.0f });

    drawGridOrtho(gridMat, viewpos.x, viewpos.y);

    commandList.setConstant(3, 1.0f);

    for (const Block* block : m_blocks)
    {
        if (block->isSelected()) continue;
        block->display(commandList);
    }

    commandList.setConstant(3, 2.0f);

    for (const Block* block : m_blocks)
    {
        if (!block->isSelected()) continue;
        block->display(commandList);
    }

    if (m_editType == EditType::Vertices)
    {
        commandList.setRenderMode(Render::RenderingPipeline::RenderMode::rm_color_point);

        if (!m_selectedBlocks.empty())
        {
            commandList.setTopology(Render::topology_pointlist);
            commandList.setConstant(2, vec4(1.0, 1.0, 0.0, 1.0));

            for (const Block* block : m_selectedBlocks) block->displayVertices(commandList);
        }

        if (!m_selectedVerts.empty())
        {
            commandList.setConstant(1, mat4());
            commandList.setConstant(2, vec4(1.0, 0.0, 0.0, 1.0));
            commandList.bindVertexBuffer(m_blockVerts);
            commandList.draw(m_blockVerts.size());
        }
    }

    Object::ResetObjects();

    commandList.setRenderMode(Render::RenderingPipeline::rm_color_wire);
    commandList.setTopology(Render::topology_trianglelist);

    for (Object* object : m_objects) object->displayOrtho(commandList);

    Object::DisplayObjects(commandList);

    // Parametric surfaces
    commandList.setRenderMode(Render::RenderingPipeline::rm_color_line);
    commandList.setTopology(Render::topology_linelist);
    commandList.setConstant(3, 1.0f);

    for (Surface* surface : m_surfaces)
    {
        if (surface->isSelected())
        {
            commandList.setConstant(2, vec4(0.0, 0.0, 0.8, 1.0));
            commandList.setConstant(3, 2.0f);
        }
        else
        {
            commandList.setConstant(2, vec4(0.0, 0.0, 0.5, 1.0));
            commandList.setConstant(3, 1.0f);
        }

        surface->displayControlMesh(commandList);
    }

    if (m_editType == EditType::ControlPoints)
    {
        commandList.setRenderMode(Render::RenderingPipeline::RenderMode::rm_color_point);

        if (!m_selectedSurfaces.empty())
        {
            commandList.setTopology(Render::topology_pointlist);
            commandList.setConstant(2, vec4(0.0, 1.0, 1.0, 1.0));

            for (const Surface* surface : m_selectedSurfaces) surface->displayControlPoints(commandList);
        }

        if (!m_selectedCps.empty())
        {
            commandList.setConstant(1, mat4());
            commandList.setConstant(2, vec4(1.0, 0.0, 0.0, 1.0));
            commandList.bindVertexBuffer(m_controlPoints);
            commandList.draw(m_controlPoints.size());
        }
    }

    commandList.barrier(frameBuffer.readBarriers());

    commandList.finish();
    Render::D3DInstance::GetInstance().execute(commandList);
}