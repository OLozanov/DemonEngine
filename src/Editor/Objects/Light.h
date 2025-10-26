#pragma once

#include "Object.h"

enum class LightType
{
    Omni,
    Spot
};

enum class LightShadow : uint8_t
{
    None = 0,
    Static = 1,
    Dynamic = 2
};

class OmniLight : public SpriteObject
{
public:
    OmniLight(float radius,
          float falloff,
          float power,
          const vec3& color,
          LightShadow shadow = LightShadow::None);

    ObjectType type() const override;
    const TypeInfo& getTypeInfo() const override;

    Object* clone() const override;

    void display(RenderContext& rc) const override;

    void write(FILE* file) const override;

private:
    static void InitGeometry();

private:
    float m_radius;
    float m_falloff;
    float m_power;
    vec3 m_color;
    LightShadow m_shadow;

    static Render::VertexBuffer m_vbuffer;
    static Render::IndexBuffer m_ibuffer;

    static const TypeInfo LightTypeInfo;
};

class SpotLight : public SpriteObject
{
public:
    SpotLight(float radius,
             float falloff,
             float outerAngle,
             float innerAngle,
             float power,
             const vec3& color,
             LightShadow shadow = LightShadow::None);

    ObjectType type() const override;
    const TypeInfo& getTypeInfo() const override;

    Object* clone() const override;

    void display(RenderContext& rc) const override;

    void write(FILE* file) const override;

private:
    static void InitGeometry();

private:
    float m_radius;
    float m_falloff;
    float m_outerAngle;
    float m_innerAngle;
    float m_power;
    vec3 m_color;
    LightShadow m_shadow;

    static Render::VertexBuffer m_vbuffer;
    static Render::IndexBuffer m_ibuffer;

    static const TypeInfo SpotLightTypeInfo;
};