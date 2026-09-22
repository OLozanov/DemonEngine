#include "Canvas.h"
#include "UI/UiLayer.h"
#include "Resources/Image.h"

namespace UI
{

Canvas::Canvas()
: m_renderer(UiLayer::GetInstance().renderer())
, m_offset(0.0f, 0.0f)
, m_edgesz(2.0f)
, m_linesz(1.0f)
, m_fillcolor(1.0f, 1.0f, 1.0f, 1.0f)
, m_linecolor(0.0f, 0.0f, 0.0f, 1.0f)
, m_textcolor(0.0f, 0.0f, 0.0f, 1.0f)
, m_fillImage(0)
, m_jointType(JointType::Edge)
, m_capType(CapType::Round)
{

}

void Canvas::reset()
{
    m_vertexBuffer.clear();
    m_drawCommands.clear();
}

void Canvas::setFillColor(float r, float g, float b, float a)
{
    m_fillcolor = { r, g, b, a };
}

void Canvas::setLineColor(float r, float g, float b, float a)
{
    m_linecolor = { r, g, b, a };
}

void Canvas::setTextColor(float r, float g, float b, float a)
{
    m_textcolor = { r, g, b, a };
}

void Canvas::setFillImage(Image* img)
{
    m_fillImage = img ? img->handle : BlankImage;

    if (img)
    {
        m_fillWidth = img->width;
        m_fillHeight = img->height;
    }
}

void Canvas::setFillImage(const Bitmap& bitmap)
{
    m_fillImage = bitmap.handle;
    m_fillWidth = bitmap.width;
    m_fillHeight = bitmap.height;
}

void Canvas::setFont(const std::string& fontName)
{
    m_font = UiLayer::GetInstance().getFont(fontName);
}

void Canvas::rectangle(short left, short top, short right, short bottom)
{
    m_vertexBuffer.push(Vertex2d(right, top, m_fillcolor));
    m_vertexBuffer.push(Vertex2d(right, bottom, m_fillcolor));
    m_vertexBuffer.push(Vertex2d(left, top, m_fillcolor));
    m_vertexBuffer.push(Vertex2d(left, bottom, m_fillcolor));

    m_drawCommands.emplace_back(4);
}

void Canvas::smoothEdge(const vec2* points, size_t size)
{
    if (m_edgesz == 0.0f) return;

    vec4 edgeColor = { m_fillcolor.x, m_fillcolor.y, m_fillcolor.z, 0.0f };

    for (size_t i = 0; i <= size; i++)
    {
        size_t prev = (i == 0) ? size - 1 : i - 1;
        size_t curr = (i == size) ? 0 : i;
        size_t next = (i == size) ? 1 : (i == size - 1) ? 0 : i + 1;

        vec2 a = points[prev];
        vec2 b = points[curr];
        vec2 c = points[next];

        vec2 e1 = (b - a).normalized();
        vec2 e2 = (c - b).normalized();

        float rcos = 1.0f / (e1 ^ e2);
        vec2 n = (e1 - e2) * rcos;

        if ((e1 ^ e2) < 0) n = -n;

        vec2 bn = b + n * m_edgesz;

        vec2 tc1 = { b.x / m_fillWidth, b.y / m_fillHeight };
        vec2 tc2 = { bn.x / m_fillWidth, bn.y / m_fillHeight };

        m_vertexBuffer.push(Vertex2d(b, tc1, m_fillcolor));
        m_vertexBuffer.push(Vertex2d(bn, tc2, edgeColor));
    }

    m_drawCommands.emplace_back((size + 1) * 2, m_fillImage, UiRenderer::topology_triangle_strip);
}

void Canvas::polygon(const vec2* points, size_t size, bool outline)
{
    if (size < 3) return;

    for (size_t i = 0; i < size; i++)
    {
        size_t v = ((i % 2 == 0) ? size - 1 - i / 2 : i / 2);

        vec2 tcoord = { points[v].x / m_fillWidth, points[v].y / m_fillHeight };

        m_vertexBuffer.push(Vertex2d(points[v], tcoord, m_fillcolor));
    }

    m_drawCommands.emplace_back(size, m_fillImage, UiRenderer::topology_triangle_strip);

    if (outline)
        line(points, size, true);
    else
        smoothEdge(points, size);
}

void Canvas::polygon(const std::vector<vec2>& points, bool outline)
{
    polygon(points.data(), points.size(), outline);
}

void Canvas::path(const Path& figure, bool outline)
{
    const std::vector<vec2>& vertices = figure.vertices();
    const std::vector<vec2>& triangles = figure.triangles();

    if (vertices.empty()) return;

    if (!triangles.empty() && m_fillcolor.w > 0.0f)
    {
        for (const vec2& vert : triangles)
        {
            vec2 tcoord = { vert.x / m_fillWidth, vert.y / m_fillHeight };
            m_vertexBuffer.push(Vertex2d(vert, tcoord, m_fillcolor));
        }

        m_drawCommands.emplace_back(triangles.size(), m_fillImage, UiRenderer::topology_triangle_list);
    }

    if (outline)
    {
        for (const Path::Loop& loop : figure.loops())
            line(vertices.data() + loop.first, loop.last - loop.first + 1, figure.closed());
    }
    else
    {
        for (const Path::Loop& loop : figure.loops())
            smoothEdge(vertices.data() + loop.first, loop.last - loop.first + 1);
    }
}

void Canvas::lineCaps(const vec2* points, size_t size, float width)
{
    float length = 0;

    // start cap
    {
        const vec2& a = points[0];
        const vec2& b = points[1];

        vec2 e1 = (b - a).normalized();
        vec2 e2 = { e1.y, -e1.x };

        vec2 v1 = a + e2 * width;
        vec2 v2 = a - e2 * width;

        vec2 v3 = v1 - e1 * width;
        vec2 v4 = v2 - e1 * width;

        vec3 tc1 = { -1.0f, length, 0.0f };
        vec3 tc2 = { 1.0f, length, 0.0f };
        vec3 tc3 = { -1.0f, length, 1.0f };
        vec3 tc4 = { 1.0f, length, 1.0f };

        m_vertexBuffer.push(Vertex2d(v1, tc1, m_linecolor));
        m_vertexBuffer.push(Vertex2d(v2, tc2, m_linecolor));
        m_vertexBuffer.push(Vertex2d(v3, tc3, m_linecolor));

        m_vertexBuffer.push(Vertex2d(v2, tc2, m_linecolor));
        m_vertexBuffer.push(Vertex2d(v3, tc3, m_linecolor));
        m_vertexBuffer.push(Vertex2d(v4, tc4, m_linecolor));
    }

    // end cap
    {
        const vec2& a = points[size - 2];
        const vec2& b = points[size - 1];

        vec2 e1 = (b - a).normalized();
        vec2 e2 = { e1.y, -e1.x };

        vec2 v1 = b - e2 * width;
        vec2 v2 = b + e2 * width;

        vec2 v3 = v1 + e1 * width;
        vec2 v4 = v2 + e1 * width;

        vec3 tc1 = { -1.0f, length, 0.0f };
        vec3 tc2 = { 1.0f, length, 0.0f };
        vec3 tc3 = { -1.0f, length, 1.0f };
        vec3 tc4 = { 1.0f, length, 1.0f };

        m_vertexBuffer.push(Vertex2d(v1, tc1, m_linecolor));
        m_vertexBuffer.push(Vertex2d(v2, tc2, m_linecolor));
        m_vertexBuffer.push(Vertex2d(v3, tc3, m_linecolor));

        m_vertexBuffer.push(Vertex2d(v2, tc2, m_linecolor));
        m_vertexBuffer.push(Vertex2d(v3, tc3, m_linecolor));
        m_vertexBuffer.push(Vertex2d(v4, tc4, m_linecolor));
    }

    m_drawCommands.emplace_back(12, m_fillImage, UiRenderer::topology_triangle_list, UiRenderer::DistanceFunc, m_linesz / width);
}

void Canvas::lineStrip(const vec2 * points, size_t size, bool loop)
{
    vec4 edgeColor = { m_fillcolor.x, m_fillcolor.y, m_fillcolor.z, 0.0f };

    float width = m_linesz + m_edgesz;
    float length = 0.0f;

    size_t segments = loop ? size + 1 : size;

    for (size_t i = 0; i < segments; i++)
    {
        size_t prev = (i == 0) ? size - 1 : i - 1;
        size_t curr = (i == size) ? 0 : i;
        size_t next = (i == size) ? 1 : (i == size - 1) ? 0 : i + 1;

        vec2 a = points[prev];
        vec2 b = points[curr];
        vec2 c = points[next];

        vec2 e1 = (b - a).normalized();
        vec2 e2 = c - b;

        float len = e2.normalize();

        vec2 n;

        if (!loop && i == 0)
        {
            n = { e2.y, -e2.x };
        }
        else if (!loop && i == segments - 1)
        {
            n = { e1.y, -e1.x };
        }
        else
        {
            float rcos = 1.0f / (e1 ^ e2);
            n = (e1 - e2) * rcos;
        }

        vec2 v1 = b - n * width;
        vec2 v2 = b + n * width;

        vec2 tc1 = { 1.0f, length };
        vec2 tc2 = { -1.0f, length };

        m_vertexBuffer.push(Vertex2d(v1, tc1, m_linecolor));
        m_vertexBuffer.push(Vertex2d(v2, tc2, m_linecolor));

        length += len;
    }

    m_drawCommands.emplace_back(segments * 2, m_fillImage, UiRenderer::topology_triangle_strip, UiRenderer::DistanceFunc, m_linesz / width);

    if (!loop && m_capType == CapType::Round) lineCaps(points, size, width);
}

void Canvas::lineList(const vec2* points, size_t size, bool loop)
{
    vec4 edgeColor = { m_fillcolor.x, m_fillcolor.y, m_fillcolor.z, 0.0f };

    float width = m_linesz + m_edgesz;
    float length = 0.0f;

    size_t segments = loop ? size + 1 : size;
    size_t verts = 0;

    vec2 norm;
    vec2 p1, p2;

    vec2 v1, v2, v3, v4, v5;

    for (size_t i = 0; i < segments; i++)
    {
        size_t prev = (i == 0) ? size - 1 : i - 1;
        size_t curr = (i == size) ? 0 : i;
        size_t next = (i == size) ? 1 : (i == size - 1) ? 0 : i + 1;

        vec2 a = points[prev];
        vec2 b = points[curr];
        vec2 c = points[next];

        vec2 e1 = (b - a).normalized();
        vec2 e2 = c - b;

        float len = e2.normalize();

        float rcos = 1.0f / (e1 ^ e2);

        if (!loop && i == 0)
            norm = { e2.y, -e2.x };
        else if (!loop && i == segments - 1)
            norm = { e1.y, -e1.x };
        else
            norm = (e1 - e2) * rcos;

        vec2 tc1 = { 1.0f, length };
        vec2 tc2 = { 0.0f, length };

        vec2 c1 = b - norm * width;
        vec2 c2 = b + norm * width;

        if (rcos > 0.0f)
        {
            p1 = c1 + vec2(e1.y, -e1.x) * width * 2.0f;
            p2 = c1 + vec2(e2.y, -e2.x) * width * 2.0f;
        }
        else
        {
            p1 = c2 - vec2(e2.y, -e2.x) * width * 2.0f;
            p2 = c2 - vec2(e1.y, -e1.x) * width * 2.0f;
        }

        vec3 tc3, tc4, tc5, tc6;

        if (!loop && i == segments - 1)
        {
            v3 = c1;
            v4 = c2;
        }
        else if (rcos > 0.0f)
        {
            v3 = c1;
            v4 = p1;
            v5 = p2;

            vec2 d1 = vec2(e1.x, e1.y) * (0.5f / width);
            vec2 d2 = vec2(e1.y, -e1.x) * (0.5f / width);

            if (m_jointType == JointType::Round)
            {
                tc3 = { 0.0f, length, 0.0f };
                tc4 = { 0.0f, length, 1.0f };
                tc5 = { (v5 - c1) * d1, length, (v5 - c1) * d2 };
                tc6 = { (c2 - c1) * d1, length, (c2 - c1) * d2 };
            }
            else
            {
                tc3 = { 0.0f, length, 0.0f };
                tc4 = { 0.0f, length, 1.0f };
                tc5 = { 0.0f, length, 1.0f };
            }
        }
        else
        {
            v3 = p2;
            v4 = c2;
            v5 = p1;

            vec2 d1 = -vec2(e1.x, e1.y) * (0.5f / width);
            vec2 d2 = -vec2(e1.y, -e1.x) * (0.5f / width);

            if (m_jointType == JointType::Round)
            {
                tc3 = { 0.0f, length, 1.0f };
                tc4 = { 0.0f, length, 0.0f };
                tc5 = { (v5 - c2) * d1, length, (v5 - c2) * d2 };
                tc6 = { (c1 - c2) * d1, length, (c1 - c2) * d2 };
            }
            else
            {
                tc3 = { 1.0f, length, 0.0f };
                tc4 = { 0.0f, length, 0.0f };
                tc5 = { 1.0f, length, 0.0f };
            }
        }

        if (i > 0)
        {
            m_vertexBuffer.push(Vertex2d(v1, tc1, m_linecolor));
            m_vertexBuffer.push(Vertex2d(v2, tc2, m_linecolor));
            m_vertexBuffer.push(Vertex2d(v3, tc1, m_linecolor));

            m_vertexBuffer.push(Vertex2d(v2, tc2, m_linecolor));
            m_vertexBuffer.push(Vertex2d(v3, tc1, m_linecolor));
            m_vertexBuffer.push(Vertex2d(v4, tc2, m_linecolor));

            verts += 6;

            if (i < segments - 1 || loop)
            {
                if (m_jointType == JointType::Flat)
                {
                    m_vertexBuffer.push(Vertex2d(v3, tc3, m_linecolor));
                    m_vertexBuffer.push(Vertex2d(v4, tc4, m_linecolor));
                    m_vertexBuffer.push(Vertex2d(v5, tc5, m_linecolor));

                    verts += 3;
                }
                else
                {
                    const vec2& c = rcos > 0.0f ? c2 : c1;
                    const vec2& v = rcos > 0.0f ? v3 : v4;
                    const vec3& tc = rcos > 0.0f ? tc3 : tc4;

                    m_vertexBuffer.push(Vertex2d(v3, tc3, m_linecolor));
                    m_vertexBuffer.push(Vertex2d(v4, tc4, m_linecolor));
                    m_vertexBuffer.push(Vertex2d(c, tc6, m_linecolor));

                    m_vertexBuffer.push(Vertex2d(v, tc, m_linecolor));
                    m_vertexBuffer.push(Vertex2d(c, tc6, m_linecolor));
                    m_vertexBuffer.push(Vertex2d(v5, tc5, m_linecolor));

                    verts += 6;
                }
            }
        }

        if (!loop && i == 0)
        {
            v1 = c1;
            v2 = c2;
        }
        else if (rcos > 0.0f)
        {
            v1 = v3;
            v2 = v5;
        }
        else
        {
            v1 = v5;
            v2 = v4;
        }

        length += len / m_fillHeight;
    }

    constexpr uint32_t DrawFlags = UiRenderer::DistanceFunc | UiRenderer::NormalRange;

    m_drawCommands.emplace_back(verts, m_fillImage, UiRenderer::topology_triangle_list, DrawFlags, m_linesz / width);

    if (!loop && m_capType == CapType::Round) lineCaps(points, size, width);
}

void Canvas::line(const vec2* points, size_t size, bool loop)
{
    if (m_jointType == JointType::Edge)
        lineStrip(points, size, loop);
    else
        lineList(points, size, loop);
}

void Canvas::line(const std::vector<vec2>& points, bool loop)
{
    line(points.data(), points.size(), loop);
}

void Canvas::moveTo(float x, float y)
{
    m_curpos = vec2(x, y);
}

void Canvas::lineTo(float x, float y)
{
    m_curpos = vec2(x, y);
}

void Canvas::image(Image* img, short left, short top, short right, short bottom)
{
    m_vertexBuffer.push(Vertex2d(right, top, 1.0, 0.0));
    m_vertexBuffer.push(Vertex2d(right, bottom, 1.0, 1.0));
    m_vertexBuffer.push(Vertex2d(left, top, 0.0, 0.0));
    m_vertexBuffer.push(Vertex2d(left, bottom, 0.0, 1.0));

    m_drawCommands.emplace_back(4, img);
}

void Canvas::text(short x, short y, const std::string& str)
{
    if (str.empty()) return;

    y -= Font::GlyphSpace * 2;

    for (size_t i = 0; i < str.size(); i++)
    {
        unsigned char ch = str[i];

        float x1 = x;
        float y1 = y;

        float x2 = x + (int)m_font->m_glyphs[ch].width + 2;
        float y2 = y + (int)m_font->m_height - 1;

        y1 += m_font->m_glyphs[ch].oy;
        y2 += m_font->m_glyphs[ch].oy;

        x1 += m_font->m_glyphs[ch].ox;
        x2 += m_font->m_glyphs[ch].ox;

        float u1 = (float)m_font->m_glyphs[ch].x / m_font->m_imgWidth;
        float v1 = (float)m_font->m_glyphs[ch].y / m_font->m_imgHeight;

        float u2 = u1 + (float)(m_font->m_glyphs[ch].width + 1) / m_font->m_imgWidth;
        float v2 = v1 + (float)(m_font->m_height) / m_font->m_imgHeight;

        vec2 a = { x1, y1 }; vec2 ta = { u1, v1 };
        vec2 b = { x2, y1 }; vec2 tb = { u2, v1 };
        vec2 c = { x1, y2 }; vec2 tc = { u1, v2 };
        vec2 d = { x2, y2 }; vec2 td = { u2, v2 };

        m_vertexBuffer.push(Vertex2d(a, ta, m_textcolor));
        m_vertexBuffer.push(Vertex2d(b, tb, m_textcolor));
        m_vertexBuffer.push(Vertex2d(c, tc, m_textcolor));

        m_vertexBuffer.push(Vertex2d(b, tb, m_textcolor));
        m_vertexBuffer.push(Vertex2d(d, td, m_textcolor));
        m_vertexBuffer.push(Vertex2d(c, tc, m_textcolor));

        x = x2 - 4 + Font::GlyphBorder;
    }

    m_drawCommands.emplace_back(str.size() * 6, m_font->m_image.get(), UiRenderer::topology_triangle_list);
}

} //namespace UI