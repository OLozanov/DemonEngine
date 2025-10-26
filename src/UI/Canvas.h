#pragma once

#include <stdint.h>
#include <vector>

#include "UI/Render.h"
#include "UI/Font.h"

namespace UI
{

class UiRenderer;

// Internal UI bitmaps like gradients
struct Bitmap
{
    uint32_t handle;
    uint32_t width;
    uint32_t height;
};

enum class JointType
{
    Triangle,
    Flat,
    Round,
};

enum class CapType
{
    Flat,
    Round
};

class Canvas
{
    UiRenderer& m_renderer;

    CommandList m_drawCommands;
    VertexBuffer m_vertexBuffer;

    vec2 m_offset;

    float m_edgesz;
    float m_linesz;

    vec2 m_curpos;

    vec4 m_fillcolor;
    vec4 m_linecolor;
    vec4 m_textcolor;

    uint32_t m_fillImage;
    uint32_t m_fillWidth;
    uint32_t m_fillHeight;

    JointType m_lineJointType;
    CapType m_lineCapType;

    const Font* m_font;

    void reset();

    const CommandList& commandList() { return m_drawCommands; }
    const VertexBuffer& vertexBuffer() { return m_vertexBuffer; }

    void smoothEdge(const vec2* points, size_t size);

    friend class UiLayer;

public:

    Canvas();
    ~Canvas() {}

    void setTranslation(float x, float y) { m_offset = { x, y }; }

    void setFillColor(float r, float g, float b, float a = 1.0f);
    void setLineColor(float r, float g, float b, float a = 1.0f);
    void setTextColor(float r, float g, float b, float a = 1.0f);

    void setFillColor(const vec4& color) { m_fillcolor = color; }
    void setLineColor(const vec4& color) { m_linecolor = color; }
    void setTextColor(const vec4& color) { m_textcolor = color; }

    void setFillImage(Image* img);
    void setFillImage(const Bitmap& bitmap);

    void setSmoothEdgeSize(float size) { m_edgesz = size; }
    void setLineSize(float size) { m_linesz = size * 0.5; }

    void setLineJointType(JointType type) { m_lineJointType = type; }
    void setLineCapType(CapType type) { m_lineCapType = type; }

    void setFont(const Font* font) { m_font = font; }
    void setFont(const std::string& fontName);
    const Font* getFont() { return m_font; }

    void line(const vec2* points, size_t size, bool loop = false);
    void line(const std::vector<vec2>& points, bool loop = false);

    void moveTo(float x, float y);
    void lineTo(float x, float y);

    void rectangle(short left, short top, short right, short bottom);
    void polygon(const vec2* points, size_t size, bool outline = false); // Supposed to be convex
    void polygon(const std::vector<vec2>& points, bool outline = false);
    void image(Image* img, short left, short top, short right, short bottom);

    void text(short x, short y, const std::string& str);
};

} //namespace ui