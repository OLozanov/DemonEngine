#pragma once

#include "Render/Render.h"
#include "Geometry/Geometry.h"
#include "EditorResources.h"

#include "Utils/EditList.h"

class Decal : public EditListNode<Decal>
{
public:
    Decal(Material* material);

    void select() { m_selected = true; }
    void deselect() { m_selected = false; }
    bool isSelected() const { return m_selected; }

    void setPos(const vec3& pos) { m_pos = pos; }
    void setOrientation(const mat3& mat) { m_orientation = mat; }

    void buildGeometry();
    void buildGeometry(const std::vector<EditPolygon*>& polys, bool negative);

    void display(Render::CommandList& commandList) const;
    void displayWire(Render::CommandList& commandList) const;

private:
    static void InitBBoxGeometry();

    void buildGeometry(const vec4* planes, const EditPolygon* poly, bool negative);

private:
    ResourcePtr<Material> m_material;

    vec3 m_pos;
    vec3 m_size;
    mat3 m_orientation;

    bool m_selected;

    Render::PushBuffer<Vertex> m_geometry;

    // BBox geometry
    static Render::VertexBuffer m_wireVBuffer;
    static Render::IndexBuffer m_wireIBuffer;
};