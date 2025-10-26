#include "Light.h"
#include "ObjectFactory.h"

const EnumInfo ShadowEnumInfo = {{"None", 0},
                                        {"Static", 1},
                                        {"Dynamic", 2}};

const TypeInfo OmniLight::LightTypeInfo = { "Omni Light",
						                nullptr,
						                {{"pos", TypeInfoMember::Type::Vec3, 0, offsetof(OmniLight, m_pos), 0},
						                 {"radius", TypeInfoMember::Type::Float, 0, offsetof(OmniLight, m_radius), 0},
						                 {"falloff", TypeInfoMember::Type::Float, 0, offsetof(OmniLight, m_falloff), 0},
                                         {"power", TypeInfoMember::Type::Float, 0, offsetof(OmniLight, m_power), 0},
                                         {"color", TypeInfoMember::Type::Color, 0, offsetof(OmniLight, m_color), 0},
                                         {"shadow", TypeInfoMember::Type::Enum, &ShadowEnumInfo, offsetof(OmniLight, m_shadow), 0}}
                                      };

const TypeInfo SpotLight::SpotLightTypeInfo = { "Spot Light",
                                                nullptr,
                                                {{"pos", TypeInfoMember::Type::Vec3, 0, offsetof(SpotLight, m_pos), 0},
                                                 {"radius", TypeInfoMember::Type::Float, 0, offsetof(SpotLight, m_radius), 0},
                                                 {"falloff", TypeInfoMember::Type::Float, 0, offsetof(SpotLight, m_falloff), 0},
                                                 {"outer angle", TypeInfoMember::Type::Float, 0, offsetof(SpotLight, m_outerAngle), 0},
                                                 {"inner angle", TypeInfoMember::Type::Float, 0, offsetof(SpotLight, m_innerAngle), 0},
                                                 {"power", TypeInfoMember::Type::Float, 0, offsetof(SpotLight, m_power), 0},
                                                 {"color", TypeInfoMember::Type::Color, 0, offsetof(SpotLight, m_color), 0},
                                                 {"shadow", TypeInfoMember::Type::Enum, &ShadowEnumInfo, offsetof(SpotLight, m_shadow), 0}}
};

Render::VertexBuffer OmniLight::m_vbuffer;
Render::IndexBuffer OmniLight::m_ibuffer;

Render::VertexBuffer SpotLight::m_vbuffer;
Render::IndexBuffer SpotLight::m_ibuffer;

OmniLight::OmniLight(float radius,
             float falloff,
             float power,
             const vec3& color,
             LightShadow shadow)
: SpriteObject(ResourceManager::GetImage("Editor\\lamp_ico.dds")->handle, 0.5)
, m_radius(radius)
, m_falloff(falloff)
, m_power(power)
, m_color(color)
, m_shadow(shadow)
{
    InitGeometry();
}

void OmniLight::InitGeometry()
{
    if (!m_vbuffer.empty()) return;

    constexpr uint32_t dang = 10;
    constexpr size_t n = 360 / dang;

    std::vector<vec3> vertices;
    std::vector<uint16_t> indices;

    vertices.reserve(360 / dang * 3);
    indices.reserve(360 / dang * 6);

    for (uint32_t ang = 0; ang < 360; ang += dang) vertices.emplace_back(sin(ang / 180.0 * math::pi), cos(ang / 180.0 * math::pi), 0);
    for (uint32_t ang = 0; ang < 360; ang += dang) vertices.emplace_back(sin(ang / 180.0 * math::pi), 0, cos(ang / 180.0 * math::pi));
    for (uint32_t ang = 0; ang < 360; ang += dang) vertices.emplace_back(0, sin(ang / 180.0 * math::pi), cos(ang / 180.0 * math::pi));

    for (size_t i = 0; i < n; i++)
    {
        size_t k = i < (n - 1) ? i + 1 : 0;

        indices.push_back(i);
        indices.push_back(k);

        indices.push_back(i + n);
        indices.push_back(k + n);

        indices.push_back(i + n * 2);
        indices.push_back(k + n * 2);
    }

    m_vbuffer.setData(vertices.data(), vertices.size());
    m_ibuffer.setData(indices.data(), indices.size());
}

ObjectType OmniLight::type() const 
{ 
    return ObjectType::OmniLightObject; 
}

const TypeInfo& OmniLight::getTypeInfo() const
{
    return LightTypeInfo;
}

Object* OmniLight::clone() const
{
    return new OmniLight(*this);
}

void OmniLight::display(RenderContext& rc) const
{
    SpriteObject::display(rc);

    if (m_selected)
    {
        Render::CommandList& commandList = rc.commandList(rc_line);

        float size = m_radius + m_falloff;

        commandList.setConstant(1, mat4::Translate(m_pos) * mat4::Scale(size));
        commandList.bindVertexBuffer(m_vbuffer);
        commandList.bindIndexBuffer(m_ibuffer);

        commandList.setConstant(3, 2.0f);
        commandList.setConstant(2, vec4(0.0f, 0.7f, 0.0f, 1.0f));

        commandList.drawIndexed(m_ibuffer.size());
    }
}

void OmniLight::write(FILE* file) const
{
    fwrite(&m_pos, sizeof(vec3), 1, file);
    fwrite(&m_radius, sizeof(float), 1, file);
    fwrite(&m_falloff, sizeof(float), 1, file);
    fwrite(&m_power, sizeof(float), 1, file);
    fwrite(&m_shadow, sizeof(LightShadow), 1, file);
    
    uint8_t r = m_color.x * 255;
    uint8_t g = m_color.y * 255;
    uint8_t b = m_color.z * 255;
    
    fwrite(&r, sizeof(uint8_t), 1, file);
    fwrite(&g, sizeof(uint8_t), 1, file);
    fwrite(&b, sizeof(uint8_t), 1, file);
}

SpotLight::SpotLight(float radius,
                     float falloff,
                     float outerAngle,
                     float innerAngle,
                     float power,
                     const vec3& color,
                     LightShadow shadow)
: SpriteObject(ResourceManager::GetImage("Editor\\lamp_ico.dds")->handle, 0.5)
, m_radius(radius)
, m_falloff(falloff)
, m_outerAngle(std::max(outerAngle, 120.0f))
, m_innerAngle(std::min(innerAngle, outerAngle))
, m_power(power)
, m_color(color)
, m_shadow(shadow)
{
    InitGeometry();
}

void SpotLight::InitGeometry()
{
    if (!m_vbuffer.empty()) return;

    constexpr uint32_t dang = 10;
    constexpr size_t n = 360 / dang;

    std::vector<vec3> vertices;
    std::vector<uint16_t> indices;

    vertices.reserve(360 / dang * 3 + 1);
    indices.reserve(360 / dang * 2 + 2 * 4);

    for (uint32_t ang = 0; ang < 360; ang += dang) vertices.emplace_back(sin(ang / 180.0 * math::pi), cos(ang / 180.0 * math::pi), 1);

    vertices.emplace_back(0.0f);

    for (size_t i = 0; i < n; i++)
    {
        size_t k = i < (n - 1) ? i + 1 : 0;

        indices.push_back(i);
        indices.push_back(k);
    }

    uint16_t center = 360 / dang * 3;
    uint16_t q = n / 4;

    for (uint16_t i = 0; i < 4; i++)
    {
        indices.push_back(center);
        indices.push_back(q * i);
    }

    m_vbuffer.setData(vertices.data(), vertices.size());
    m_ibuffer.setData(indices.data(), indices.size());
}

ObjectType SpotLight::type() const
{
    return ObjectType::SpotLightObject;
}

const TypeInfo& SpotLight::getTypeInfo() const
{
    return SpotLightTypeInfo;
}

Object* SpotLight::clone() const
{
    return new SpotLight(*this);
}

void SpotLight::display(RenderContext& rc) const
{
    SpriteObject::display(rc);

    if (m_selected)
    {
        Render::CommandList& commandList = rc.commandList(rc_line);

        float length = m_radius + m_falloff;
        float width = tan(m_outerAngle * 0.5f / 180.0f * math::pi) * length;
        vec3 size = { width, width, length };

        mat4 rot = mat4::Rotate(m_rot.x, m_rot.y, m_rot.z) * m_mat;

        commandList.setConstant(1, mat4::Translate(m_pos) * rot * mat4::Scale(size));
        commandList.bindVertexBuffer(m_vbuffer);
        commandList.bindIndexBuffer(m_ibuffer);

        commandList.setConstant(3, 2.0f);
        commandList.setConstant(2, vec4(0.0f, 0.7f, 0.0f, 1.0f));

        commandList.drawIndexed(m_ibuffer.size());
    }
}

void SpotLight::write(FILE* file) const
{
    fwrite(&m_pos, sizeof(vec3), 1, file);
    fwrite(&m_mat[2], sizeof(vec3), 1, file);
    fwrite(&m_radius, sizeof(float), 1, file);
    fwrite(&m_falloff, sizeof(float), 1, file);
    fwrite(&m_outerAngle, sizeof(float), 1, file);
    fwrite(&m_innerAngle, sizeof(float), 1, file);
    fwrite(&m_power, sizeof(float), 1, file);
    fwrite(&m_shadow, sizeof(LightShadow), 1, file);

    uint8_t r = m_color.x * 255;
    uint8_t g = m_color.y * 255;
    uint8_t b = m_color.z * 255;

    fwrite(&r, sizeof(uint8_t), 1, file);
    fwrite(&g, sizeof(uint8_t), 1, file);
    fwrite(&b, sizeof(uint8_t), 1, file);
}