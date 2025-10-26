#pragma once

#include "Render/Render.h"
#include "Geometry/Geometry.h"
#include "Resources/Resources.h"
#include "Utils/EditList.h"
#include "EditPoint.h"
#include "stdint.h"

enum class SurfaceType : uint8_t
{
    BezierPatch = 0,
    BezierTriangle = 1,
    BSpline = 2,
    Invalid = 0xFF
};

class Surface : public EditListNode<Surface>
{
public:

    using Cp = EditPoint<Surface>;
    using CpList = LinkedList<Cp>;

    enum Flags
    {
        surf_collision = 1,
        surf_accurate_normals = 2,
        surf_displacement = 4,
        surf_blend_material = 8,
        surf_cyclic_x = 16,
        surf_cyclic_y = 32,
        surf_endpoint_x = 64,
        surf_endpoint_y = 128
    };

public:

    Surface() : m_scale(1, 1, 1) {}
    virtual ~Surface() = default;

    const vec3& pos() { return m_pos; }
    void setPos(const vec3& pos) { m_pos = pos; }
    void select() { m_selected = true; }
    void deselect() { m_selected = false; }
    bool isSelected() { return m_selected; }

    const vec3& rot() const { return m_rot; }
    void setXRot(float rot) { m_rot = { rot, 0, 0 }; }
    void setYRot(float rot) { m_rot = { 0, rot, 0 }; }
    void setZRot(float rot) { m_rot = { 0, 0, rot }; }

    void setFlag(unsigned int flg, bool active);
    bool getFlags(unsigned int flg) const;

    void setMapMode(TextureMapping mapMode);
    TextureMapping getMapMode() const;

    void setMaterial(Material* material) { m_material = material; }
    Material* material() { return m_material.get(); }

    void setTSpace(const vec3& s, const vec3& t);

    const vec3& moveDiff() const { return m_pickdiff; }
    void calculateMoveDiff(const vec3& point) { m_pickdiff = m_pos - point; }

    void pickMove(const vec3& newpt);
    void pickMove2d(float x, float y, int i, int k);
    void pickRotateX(float Cos, float Sin, const vec3& center);
    void pickRotateY(float Cos, float Sin, const vec3& center);
    void pickRotateZ(float Cos, float Sin, const vec3& center);
    void pickScale(const vec3& center, const vec3& scale);

    void moveTextureS(float val);
    void moveTextureT(float val);
    void scaleTextureS(float val);
    void scaleTextureT(float val);
    void rotateTexture(float ang);

    void applyRotation();
    void applyScale();

    virtual SurfaceType type() const = 0;
    virtual Surface* clone() const = 0;

    virtual void update() = 0;
    virtual void reconfig() {}

    virtual uint32_t getXResolution() = 0;
    virtual uint32_t getYResolution() = 0;

    virtual void setResolution(uint32_t x, uint32_t y = 0) = 0;

    virtual bool pick(const vec3& origin, const vec3& ray, vec3& point, float& dist) const = 0;
    virtual bool pick2d(float x, float y, float scale, float& depth, int i, int j, int k) const = 0;

    virtual vec3* pickCp(const vec3& origin, const vec3& ray, float& dist) = 0;
    virtual bool pickCp2d(float x, float y, float scale, int i, int k, CpList& cpList) = 0;

    virtual void display(Render::CommandList& commandList) const = 0;
    virtual void displayControlMesh(Render::CommandList& commandList) const = 0;
    virtual void displayControlPoints(Render::CommandList& commandList) const = 0;

    virtual void writeEditorInfo(FILE* file) const = 0;
    virtual void writeGameInfo(FILE* file) const = 0;

protected:

    virtual void moveTexCoordS(float val) = 0;
    virtual void moveTexCoordT(float val) = 0;
    virtual void scaleTexCoordS(float val) = 0;
    virtual void scaleTexCoordT(float val) = 0;
    virtual void rotateTexCoord(float ang) = 0;

    virtual void applyTransform(const mat3& mat) = 0;
    virtual void applyScaleInternal() = 0;

    virtual void updateTCoord() = 0;

protected:

    vec3 m_pos;

    vec3 m_rot;
    vec3 m_scale;

    uint32_t m_flags;
    ResourcePtr<Material> m_material;

    //Texture mapping
    TextureMapping m_mapMode;
    vec3 m_s;
    vec3 m_t;
    vec2 m_uv;

    bool m_selected;

    vec3 m_pickdiff;
};