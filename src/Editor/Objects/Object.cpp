#include "Object.h"
#include "Geometry/Geometry.h"

TypeInfo Object::ObjectTypeInfo = { "Object",
                                    nullptr,
                                    {{"pos", TypeInfoMember::Type::Vec3, 0, offsetof(Object, m_pos), 0},
                                     {"rot", TypeInfoMember::Type::Orientation, 0, offsetof(Object, m_mat), 0}}
                                  };

std::vector<SpriteData> Object::m_sprites;

void Object::calculateMoveDiff(const vec3& point)
{
    m_pickdiff = m_pos - point;
}

void Object::pickMove(const vec3& newpt)
{
    m_pos = newpt + m_pickdiff;
}

void Object::pickMove2d(float x, float y, int i, int k)
{
    m_pos[i] = x + m_pickdiff[i];
    m_pos[k] = y + m_pickdiff[k];
}

void Object::pickRotateX(float Cos, float Sin, const vec3& center)
{
    float y = Cos * m_pickdiff.y + Sin * m_pickdiff.z;
    float z = -Sin * m_pickdiff.y + Cos * m_pickdiff.z;

    m_pos.y = center.y + y;
    m_pos.z = center.z + z;
}

void Object::pickRotateY(float Cos, float Sin, const vec3& center)
{
    float x = Cos * m_pickdiff.x - Sin * m_pickdiff.z;
    float z = Sin * m_pickdiff.x + Cos * m_pickdiff.z;

    m_pos.x = center.x + x;
    m_pos.z = center.z + z;
}

void Object::pickRotateZ(float Cos, float Sin, const vec3& center)
{
    float x = Cos * m_pickdiff.x + Sin * m_pickdiff.y;
    float y = -Sin * m_pickdiff.x + Cos * m_pickdiff.y;

    m_pos.x = center.x + x;
    m_pos.y = center.y + y;
}

void Object::applyRotation()
{
    m_mat = mat3::Rotate(m_rot.x, m_rot.y, m_rot.z) * m_mat;
    m_rot = { 0, 0, 0 };
}

ObjectType Object::type() const 
{ 
    return ObjectType::Invalid; 
}

const TypeInfo& Object::getTypeInfo() const
{
    return ObjectTypeInfo;
}

void Object::AddSprite(const vec3& position, ImageHandle id, float size, const vec3& color)
{
    m_sprites.push_back({ position, id, color, size });
}

void Object::ResetObjects()
{
    m_sprites.clear();
}

void Object::DisplayObjects(Render::CommandList& commandList)
{
    if (m_sprites.empty()) return;

    commandList.setRenderMode(Render::RenderingPipeline::RenderMode::rm_sprite_simple);
    commandList.setTopology(Render::topology_trianglestrip);

    for (const SpriteData& data : m_sprites)
    {
        commandList.setConstant(1, data);
        commandList.bind(2, data.image);
        commandList.draw(4);
    }
}

MeshObject::MeshObject()
{
}

MeshObject::MeshObject(Model* model)
: m_model(model)
{
}

bool MeshObject::pick(const vec3& origin, const vec3& ray, vec3& point, float& dist) const
{
    bool result = false;

    vec3 localOrigin = (origin - m_pos) * m_mat;
    vec3 localRay = ray * m_mat;

    const std::vector<Render::Vertex>& verts = m_model->vertices();
    const std::vector<uint16_t>& indices = m_model->indices();

    size_t trinum = indices.size() / 3;

    for (size_t i = 0; i < trinum; i++)
    {
        uint16_t i0 = indices[i * 3];
        uint16_t i1 = indices[i * 3 + 1];
        uint16_t i2 = indices[i * 3 + 2];

        const vec3& v0 = verts[i0].position;
        const vec3& v1 = verts[i1].position;
        const vec3& v2 = verts[i2].position;

        float d;

        if (TriangleIntersect(v0, v1, v2, localOrigin, localRay, d))
        {
            if (!result || d < dist)
            {
                dist = d;
                point = origin + ray * d;

                result = true;
            }
        }
    }

    return result;
}

bool MeshObject::pick2d(float x, float y, float scale, float& depth, int i, int j, int k) const
{
    bool result = false;

    const std::vector<Render::Vertex>& verts = m_model->vertices();
    const std::vector<uint16_t>& indices = m_model->indices();

    size_t trinum = indices.size() / 3;

    for (size_t f = 0; f < trinum; f++)
    {
        uint16_t i0 = indices[f * 3];
        uint16_t i1 = indices[f * 3 + 1];
        uint16_t i2 = indices[f * 3 + 2];

        const vec3& v0 = m_mat * verts[i0].position;
        const vec3& v1 = m_mat * verts[i1].position;
        const vec3& v2 = m_mat * verts[i2].position;

        vec2 e1 = { v0[i] - v2[i], v0[j] - v2[j] };
        vec2 e2 = { v1[i] - v2[i], v1[j] - v2[j] };

        float det = e1 ^ e2;

        if (fabs(det) < math::eps) continue;

        vec2 p = { x - v2[i] - m_pos[i], y - v2[j] - m_pos[j] };

        float a = p ^ e2;
        float u = a / det;

        if (u < 0.0f || u > 1.0f) continue;

        float b = e1 ^ p;
        float v = b / det;

        if (v < 0.0f || u + v > 1.0f) continue;

        float w = 1.0 - u - v;

        float d = v0[k] * u + v1[k] * v + v2[k] * w;

        if (!result || d > depth) depth = d;

        result = true;
    }

    return result;
}

void MeshObject::display(RenderContext& rc) const
{
    Render::CommandList& commandList = rc.commandList(rc_regular);

    const std::vector<Render::DisplayData>& displayList = m_model->meshes();

    commandList.setConstant(1, mat4::Translate(m_pos) * mat4::Rotate(m_rot.x, m_rot.y, m_rot.z) * m_mat);
    commandList.bindVertexBuffer(m_model->vertexBuffer());
    commandList.bindIndexBuffer(m_model->indexBuffer());

    if (m_selected) commandList.setConstant(2, vec4(0.0, 0.8, 0.0, 1.0));
    else commandList.setConstant(2, vec4(1.0, 1.0, 1.0, 1.0));

    for (const Render::DisplayData& elem : displayList)
    {
        commandList.bind(3, elem.material->maps[Material::map_diffuse]);
        commandList.drawIndexed(elem.vertexnum, elem.offset);
    }
}

void MeshObject::displayOrtho(Render::CommandList& commandList) const
{
    const std::vector<Render::DisplayData>& displayList = m_model->meshes();

    commandList.setConstant(1, mat4::Translate(m_pos) * mat4::Rotate(m_rot.x, m_rot.y, m_rot.z) * m_mat);
    commandList.bindVertexBuffer(m_model->vertexBuffer());
    commandList.bindIndexBuffer(m_model->indexBuffer());

    if (m_selected) commandList.setConstant(2, vec4(0.0, 0.8, 0.0, 1.0));
    else commandList.setConstant(2, vec4(0.0, 0.4, 0.0, 1.0));

    for (const Render::DisplayData& elem : displayList)
        commandList.drawIndexed(elem.vertexnum, elem.offset);
}

SpriteObject::SpriteObject()
: m_handle(1)
, m_size(0.2)
{
}

SpriteObject::SpriteObject(ImageHandle image, float size)
: m_handle(image)
, m_size(size)
{
}

bool SpriteObject::pick(const vec3& origin, const vec3& ray, vec3& point, float& dist) const
{
    vec3 dir = m_pos - origin;
    float len = dir.normalize();

    float Cos = ray * dir;
    
    if (Cos < math::eps) return false;
    
    float Sin = sqrt(1.0 - Cos * Cos);

    if (Sin * len < m_size)
    {
        dist = len;
        point = origin + ray * len;

        return true;
    }

    return false;
}

bool SpriteObject::pick2d(float x, float y, float scale, float& depth, int i, int j, int k) const
{
    float dx = m_pos[i] - x;
    float dy = m_pos[j] - y;

    if ((dx > -m_size) && (dx < m_size) && (dy > -m_size) && (dy < m_size))
    {
        depth = m_pos[k];
        return true;
    
    } else return false;
}

void SpriteObject::display(RenderContext& rc) const
{
    static const vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
    static const vec4 selectedColor = vec4(0.0, 0.8, 0.0, 1.0);

    AddSprite(m_pos, m_handle, m_size, m_selected ? selectedColor : color);
}

void SpriteObject::displayOrtho(Render::CommandList& commandList) const
{
    static const vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
    static const vec4 selectedColor = vec4(0.0, 0.8, 0.0, 1.0);

    AddSprite(m_pos, m_handle, m_size, m_selected ? selectedColor : color);
}

Render::VertexBuffer BoxObject::m_vbuffer;
Render::IndexBuffer BoxObject::m_ibuffer;
Render::IndexBuffer BoxObject::m_wireibuffer;

BoxObject::BoxObject()
: m_size(1.0f, 1.0f, 1.0f)
, m_color(1.0f, 1.0f, 1.0f)
{
    InitGeometry();
}

BoxObject::BoxObject(const vec3& size, const vec3& color)
: m_size(size)
, m_color(color)
{
    InitGeometry();
}

void BoxObject::InitGeometry()
{
    if (!m_vbuffer.empty()) return;

    std::vector<vec3> vertices = { {-1.0f, -1.0f, -1.0f},
                                   {1.0f, -1.0f, -1.0f},
                                   {1.0f, -1.0f, 1.0f},
                                   {-1.0f, -1.0f, 1.0f},
                                   {-1.0f, 1.0f, -1.0f},
                                   {1.0f, 1.0f, -1.0f},
                                   {1.0f, 1.0f, 1.0f},
                                   {-1.0f, 1.0f, 1.0f} };

    std::vector<uint16_t> indices = { { 0, 1, 2, 0, 2, 3,
                                        7, 6, 5, 7, 5, 4,
                                        0, 3, 7, 0, 7, 4,
                                        5, 6, 2, 5, 2, 1,
                                        2, 6, 7, 2, 7, 3,
                                        0, 4, 5, 0, 5, 1 } };

    std::vector<uint16_t> wireIndices = { { 0, 1, 1, 2, 2, 3, 3, 0,
                                            4, 5, 5, 6, 6, 7, 7, 4,
                                            0, 4, 1, 5, 2, 6, 3, 7 } };

    m_vbuffer.setData(vertices.data(), vertices.size());
    m_ibuffer.setData(indices.data(), indices.size());
    m_wireibuffer.setData(wireIndices.data(), wireIndices.size());
}

bool BoxObject::pick(const vec3& origin, const vec3& ray, vec3& point, float& dist) const
{
    mat3 axis = { {1, 0, 0},
                  {0, 1, 0},
                  {0, 0, 1} };

    bool pick = BBoxRayTest(origin, ray, m_pos, m_size, axis, dist);

    point = origin + ray * dist;

    return pick;
}

bool BoxObject::pick2d(float x, float y, float scale, float& depth, int i, int j, int k) const
{
    if (fabs(x - m_pos[i]) > m_size[i]) return false;
    if (fabs(y - m_pos[j]) > m_size[j]) return false;

    depth = m_pos[k] - m_size[k];

    return true;
}

void BoxObject::display(RenderContext& rc) const
{
    // Color
    {
        Render::CommandList& commandList = rc.commandList(rc_color);

        commandList.setConstant(1, mat4::Translate(m_pos) * mat4::Scale(m_size));
        commandList.bindVertexBuffer(m_vbuffer);
        commandList.bindIndexBuffer(m_ibuffer);

        if (m_selected) commandList.setConstant(2, vec4(0.0, 0.8, 0.0, 1.0));
        else commandList.setConstant(2, vec4(m_color, 1.0));

        commandList.drawIndexed(6 * 3 * 2);
    }

    // Wire
    {
        Render::CommandList& commandList = rc.commandList(rc_line);

        commandList.setConstant(1, mat4::Translate(m_pos) * mat4::Scale(m_size));
        commandList.bindVertexBuffer(m_vbuffer);
        commandList.bindIndexBuffer(m_wireibuffer);

        commandList.setConstant(3, 1.0f);
        commandList.setConstant(2, vec4(m_color * 0.7, 1.0));

        commandList.drawIndexed(8 * 3);
    }
}

void BoxObject::displayOrtho(Render::CommandList& commandList) const
{
    commandList.setConstant(1, mat4::Translate(m_pos) * mat4::Scale(m_size));
    commandList.bindVertexBuffer(m_vbuffer);
    commandList.bindIndexBuffer(m_ibuffer);

    if (m_selected) commandList.setConstant(2, vec4(0.0, 0.8, 0.0, 1.0));
    else commandList.setConstant(2, vec4(m_color, 1.0));

    commandList.drawIndexed(6 * 3 * 2);
}

OrientedBoxObject::OrientedBoxObject()
: m_ang(0)
{
}

OrientedBoxObject::OrientedBoxObject(const vec3& size, const vec3& color, const vec3& frontColor)
: BoxObject(size, color)
, m_frontColor(frontColor)
, m_ang(0)
{
}

void OrientedBoxObject::setAngle(float ang)
{
    m_ang = ang;
    updateMat();
}

bool OrientedBoxObject::pick(const vec3& origin, const vec3& ray, vec3& point, float& dist) const
{
    bool pick = BBoxRayTest(origin, ray, m_pos, m_size, m_mat, dist);

    point = origin + ray * dist;

    return pick;
}

bool OrientedBoxObject::pick2d(float x, float y, float scale, float& depth, int i, int j, int k) const
{
    if (i == 0 && j == 2)
    {
        vec3 localPos = vec3(x - m_pos.x, 0, y - m_pos.z);

        float xdist = localPos * m_mat[0];
        float zdist = localPos * m_mat[2];

        if (fabs(xdist) > m_size.x) return false;
        if (fabs(zdist) > m_size.z) return false;

        return true;
    }

    if (fabs(x - m_pos[i]) > m_size[i]) return false;
    if (fabs(y - m_pos[j]) > m_size[j]) return false;

    depth = m_pos[k] - m_size[k];

    return true;
}

void OrientedBoxObject::display(RenderContext& rc) const
{
    // Color
    {
        Render::CommandList& commandList = rc.commandList(rc_color);

        commandList.setConstant(1, mat4::Translate(m_pos) * mat4::RotateY(m_rot.y + m_ang) * mat4::Scale(m_size));
        commandList.bindVertexBuffer(m_vbuffer);
        commandList.bindIndexBuffer(m_ibuffer);

        if (m_selected) commandList.setConstant(2, vec4(0.0, 0.8, 0.0, 1.0));
        else commandList.setConstant(2, vec4(m_color, 1.0));

        commandList.drawIndexed(5 * 3 * 2);

        if (m_selected) commandList.setConstant(2, vec4(0.5, 0.8, 0.5, 1.0));
        else commandList.setConstant(2, vec4(m_frontColor, 1.0));

        commandList.drawIndexed(3 * 2, 5 * 3 * 2);
    }
}

void OrientedBoxObject::displayOrtho(Render::CommandList& commandList) const
{
    commandList.setConstant(1, mat4::Translate(m_pos) * mat4::RotateY(m_rot.y + m_ang) * mat4::Scale(m_size));
    commandList.bindVertexBuffer(m_vbuffer);
    commandList.bindIndexBuffer(m_ibuffer);

    if (m_selected) commandList.setConstant(2, vec4(0.0, 0.8, 0.0, 1.0));
    else commandList.setConstant(2, vec4(m_color, 1.0));

    commandList.drawIndexed(6 * 3 * 2);
}

void OrientedBoxObject::updateMat()
{
    m_mat = { {cosf(m_ang), 0, -sinf(m_ang)},
              {0, 1, 0},
              {sinf(m_ang), 0, cosf(m_ang)} };
}

void OrientedBoxObject::applyRotation()
{
    m_ang += m_rot.y;
    m_rot.y = 0;

    updateMat();
}