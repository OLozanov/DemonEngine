#pragma once

#include "math/math3d.h"
#include "Render/Render.h"
#include "RenderContext.h"
#include "Resources/Resources.h"

#include "Utils/LinkedList.h"
#include "Utils/EditList.h"

#include "TypeInfo.h"

#include <string>

enum class ObjectType : uint8_t
{
    OmniLightObject = 0,
    SpotLightObject = 8,
    StaticObject = 1,
    Entity = 2,
    AmbientSound = 3,
    Trigger = 4,
    PlayerStart = 5,
    MapFinish = 6,
    Ladder = 7,
    FogVolume = 9,
    Invalid = 0xFF
};

class Object : public EditListNode<Object>
{
public:
    virtual ~Object() = default;

    const vec3 pos() const { return m_pos; }
    void setPos(const vec3& pos) { m_pos = pos; }

    void setOrientation(const mat3& mat) { m_mat = mat; }

    const vec3& rot() const { return m_rot; }
    void setXRot(float rot) { m_rot = { rot, 0, 0 }; }
    void setYRot(float rot) { m_rot = { 0, rot, 0 }; }
    void setZRot(float rot) { m_rot = { 0, 0, rot }; }

    void select() { m_selected = true; }
    void deselect() { m_selected = false; }
    bool isSelected() const { return m_selected; }

    const vec3& moveDiff() const { return m_pickdiff; }
    void calculateMoveDiff(const vec3& point);
    void pickMove(const vec3& newpt);
    void pickMove2d(float x, float y, int i, int k);
    void pickRotateX(float Cos, float Sin, const vec3& center);
    void pickRotateY(float Cos, float Sin, const vec3& center);
    void pickRotateZ(float Cos, float Sin, const vec3& center);

    virtual void applyRotation();

    virtual ObjectType type() const;
    virtual const TypeInfo& getTypeInfo() const;

    virtual Object* clone() const = 0;

    virtual void write(FILE* file) const = 0;

    virtual bool pick(const vec3& origin, const vec3& ray, vec3& point, float& dist) const = 0;
    virtual bool pick2d(float x, float y, float scale, float& depth, int i, int j, int k) const = 0;

    virtual void display(RenderContext& rc) const = 0;
    virtual void displayOrtho(Render::CommandList& commandList) const = 0;

    static void ResetObjects();
    static void DisplayObjects(Render::CommandList& commandList);

protected:
    static void AddSprite(const vec3& position, ImageHandle id, float size, const vec3& color);

protected:
    std::string m_id;
    vec3 m_pos;
    mat3 m_mat;

    vec3 m_rot;
    vec3 m_scale;

    bool m_selected;
    vec3 m_pickdiff;

    static TypeInfo ObjectTypeInfo;

private:
    static std::vector<SpriteData> m_sprites;
};

class MeshObject : public Object
{
public:
    MeshObject();
    MeshObject(Model* model);

    bool pick(const vec3& origin, const vec3& ray, vec3& point, float& dist) const override;
    bool pick2d(float x, float y, float scale, float& depth, int i, int j, int k) const override;

    void display(RenderContext& rc) const override;
    void displayOrtho(Render::CommandList& commandList) const override;

protected:
    ResourcePtr<Model> m_model;
};

class SpriteObject : public Object
{
public:
    SpriteObject();
    SpriteObject(ImageHandle image, float size);

    bool pick(const vec3& origin, const vec3& ray, vec3& point, float& dist) const override;
    bool pick2d(float x, float y, float scale, float& depth, int i, int j, int k) const override;

    void display(RenderContext& rc) const override;
    void displayOrtho(Render::CommandList& commandList) const override;

private:
    const ImageHandle m_handle;
    const float m_size;
};

class BoxObject : public Object
{
public:
    BoxObject();
    BoxObject(const vec3& size, const vec3& color);

    bool pick(const vec3& origin, const vec3& ray, vec3& point, float& dist) const override;
    bool pick2d(float x, float y, float scale, float& depth, int i, int j, int k) const override;

    void display(RenderContext& rc) const override;
    void displayOrtho(Render::CommandList& commandList) const override;

protected:
    static void InitGeometry();

protected:
    vec3 m_size;
    vec3 m_color;

    static Render::VertexBuffer m_vbuffer;
    static Render::IndexBuffer m_ibuffer;
    static Render::IndexBuffer m_wireibuffer;
};

class OrientedBoxObject : public BoxObject
{
public:
    OrientedBoxObject();
    OrientedBoxObject(const vec3& size, const vec3& color, const vec3& frontColor);

    void setAngle(float ang);

    bool pick(const vec3& origin, const vec3& ray, vec3& point, float& dist) const override;
    bool pick2d(float x, float y, float scale, float& depth, int i, int j, int k) const override;

    void display(RenderContext& rc) const override;
    void displayOrtho(Render::CommandList& commandList) const override;

private:
    void updateMat();
    void applyRotation() override;

protected:

    vec3 m_frontColor;
    float m_ang;
};