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
, m_fillImage(1)
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
    m_fillImage = img ? img->handle : 1;

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

void Canvas::line(const vec2* points, size_t size, bool loop)
{
    vec4 edgeColor = { m_fillcolor.x, m_fillcolor.y, m_fillcolor.z, 0.0f };

    //vec2 v[4];
    //vec2 ve[4]; //edge vertices

    float width = m_linesz + m_edgesz;
    float length = 0.0f;

    //loop = false;
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

    m_drawCommands.emplace_back(segments * 2, m_fillImage, UiRenderer::topology_triangle_strip, UiRenderer::sdf, m_linesz / width);
}

void Canvas::line(const std::vector<vec2>& points, bool loop)
{
    line(points.data(), points.size(), loop);
}

/*void Canvas::line(const std::vector<vec2>& verts, bool loop)
{
    vec4 edgecolor = vec4(m_linecolor.x, m_linecolor.y, m_linecolor.z, 0.0);

    int segmentsNum = loop ? verts.size() : verts.size() - 1;

    vec2 tangent_prev = {};

    if (loop)
    {
        const vec2& v1 = verts[0];
        const vec2& v2 = verts[verts.size() - 1];

        tangent_prev = v2 - v1;
        tangent_prev.normalize();
    }

    for (int i = 0; i < segmentsNum; i++)
    {
        int k = (i == verts.size() - 1) ? 0 : i + 1;

        const vec2& v1 = verts[i];
        const vec2& v2 = verts[k];

        vec2 tangent = v2 - v1;
        tangent.normalize();

        vec2 norm = -vec2( tangent.y, -tangent.x );

        vec2 a, b, c, d;
        vec2 ae, be, ce, de; //edge vertices

        vec2 tangent_next = {};

        if (i != 0 || loop)
        {
            float rcos = 1.0f / (norm * tangent_prev);
            vec2 jointVec = (tangent + tangent_prev) * rcos;

            a = v1 + jointVec * m_linesz;
            b = v1 - jointVec * m_linesz;

            ae = a + jointVec * edgesz;
            be = b - jointVec * edgesz;
        }
        else
        {
            a = v1 + norm * m_linesz;
            b = v1 - norm * m_linesz;

            ae = a + norm * edgesz;
            be = b - norm * edgesz;
        }

        if (i != verts.size() - 2 || loop)
        {
            int l = (k == verts.size() - 1) ? 0 : k + 1;

            const vec2& v3 = verts[k];
            const vec2& v4 = verts[l];

            vec2 tangent_next = v3 - v4;
            tangent_next.normalize();
        
            float rcos = 1.0f / (norm * tangent_next);
            vec2 jointVec = (tangent + tangent_next) * rcos;

            c = v2 + jointVec * m_linesz;
            d = v2 - jointVec * m_linesz;

            ce = c + jointVec * edgesz;
            de = d - jointVec * edgesz;
        }
        else
        {
            c = v2 + norm * m_linesz;
            d = v2 - norm * m_linesz;

            ce = c + norm * edgesz;
            de = d - norm * edgesz;
        }

        //line
        m_vertexBuffer.push(Vertex2d(a, m_linecolor));
        m_vertexBuffer.push(Vertex2d(b, m_linecolor));
        m_vertexBuffer.push(Vertex2d(c, m_linecolor));

        m_vertexBuffer.push(Vertex2d(b, m_linecolor));
        m_vertexBuffer.push(Vertex2d(d, m_linecolor));
        m_vertexBuffer.push(Vertex2d(c, m_linecolor));

        //uppder edge
        m_vertexBuffer.push(Vertex2d(ae, edgecolor));
        m_vertexBuffer.push(Vertex2d(a, m_linecolor));
        m_vertexBuffer.push(Vertex2d(c, m_linecolor));

        m_vertexBuffer.push(Vertex2d(ce, edgecolor));
        m_vertexBuffer.push(Vertex2d(ae, edgecolor));
        m_vertexBuffer.push(Vertex2d(c, m_linecolor));

        //lower edge
        m_vertexBuffer.push(Vertex2d(b, m_linecolor));
        m_vertexBuffer.push(Vertex2d(be, edgecolor));
        m_vertexBuffer.push(Vertex2d(d, m_linecolor));

        m_vertexBuffer.push(Vertex2d(de, edgecolor));
        m_vertexBuffer.push(Vertex2d(d, m_linecolor));
        m_vertexBuffer.push(Vertex2d(be, edgecolor));

        tangent_prev = -tangent;
    }

    m_drawCommands.emplace_back(segmentsNum*3*6, UiRenderer::topology_triangle_list);
}*/

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

    m_drawCommands.emplace_back(str.size() * 6, m_font->m_image, UiRenderer::topology_triangle_list);
}

} //namespace ui