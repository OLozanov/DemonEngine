#include "Block.h"
#include "Geometry/Geometry.h"

#include "stdio.h"
#include <set>

static constexpr float dts = 0.5;  // Default Texture Scale

void Block::Triangle(float width, float height, PlaneType ptype, Block& block)
{
    width *= 0.5;
    height *= 0.5;

    std::vector<vec3> vertices;
    std::vector<uint16_t> indices;
    std::vector<BlockPolygon> polygons;

    vertices.resize(3);
    polygons.resize(1);

    switch (ptype)
    {
    case PlaneType::XY:
        vertices[0] = vec3(-width, -height, 0);
        vertices[1] = vec3(0, height, 0);
        vertices[2] = vec3(width, -height, 0);

        polygons[0].s = vec3(dts, 0, 0);
        polygons[0].t = vec3(0, dts, 0);
    break;

    case PlaneType::XZ:
        vertices[0] = vec3(-width, 0, -height);
        vertices[1] = vec3(0, 0, height);
        vertices[2] = vec3(width, 0, -height);

        polygons[0].s = vec3(dts, 0, 0);
        polygons[0].t = vec3(0, 0, dts);
    break;

    case PlaneType::YZ:
        vertices[0] = vec3(0, -height, -width);
        vertices[1] = vec3(0, height, 0);
        vertices[2] = vec3(0, -height, width);

        polygons[0].s = vec3(0, 0, dts);
        polygons[0].t = vec3(0, dts, 0);
    break;
    }

    indices = { 0, 1, 2 };

    polygons[0].offset = 0;
    polygons[0].vertnum = 3;

    polygons[0].tcoord.x = 0;
    polygons[0].tcoord.y = 0;

    block.reset(vertices, indices, polygons);
}

void Block::Plane(float width, float height, PlaneType ptype, Block& block)
{
    width *= 0.5;
    height *= 0.5;

    std::vector<vec3> vertices;
    std::vector<uint16_t> indices;
    std::vector<BlockPolygon> polygons;

    vertices.resize(4);
    polygons.resize(1);

    switch (ptype)
    {
    case PlaneType::XY:
        vertices[0] = vec3(-width, -height, 0);
        vertices[1] = vec3(-width, height, 0);
        vertices[2] = vec3(width, height, 0);
        vertices[3] = vec3(width, -height, 0);

        polygons[0].s = vec3(dts, 0, 0);
        polygons[0].t = vec3(0, dts, 0);
    break;

    case PlaneType::XZ:
        vertices[0] = vec3(-width, 0, -height);
        vertices[1] = vec3(-width, 0, height);
        vertices[2] = vec3(width, 0, height);
        vertices[3] = vec3(width, 0, -height);

        polygons[0].s = vec3(dts, 0, 0);
        polygons[0].t = vec3(0, 0, dts);
    break;

    case PlaneType::YZ:
        vertices[0] = vec3(0, -width, -height);
        vertices[1] = vec3(0, -width, height);
        vertices[2] = vec3(0, width, height);
        vertices[3] = vec3(0, width, -height);

        polygons[0].s = vec3(0, 0, dts);
        polygons[0].t = vec3(0, dts, 0);
    break;
    }

    indices = { 0, 1, 2, 3 };

    polygons[0].offset = 0;
    polygons[0].vertnum = 4;

    polygons[0].tcoord.x = 0;
    polygons[0].tcoord.y = 0;

    block.reset(vertices, indices, polygons);
}

void Block::Box(float width, float length, float height, Block& block)
{
    width *= 0.5;
    length *= 0.5;
    height *= 0.5;

    std::vector<vec3> vertices;
    std::vector<uint16_t> indices;
    std::vector<BlockPolygon> polygons;

    vertices = { {-width, -height, -length},
                 {width, -height, -length},
                 {width, -height, length},
                 {-width, -height, length},
                 {-width, height, -length},
                 {width, height, -length},
                 {width, height, length},
                 {-width, height, length} };

    indices = { { 0, 1, 2, 3,
                  7, 6, 5, 4,
                  0, 4, 5, 1,
                  2, 6, 7, 3,
                  0, 3, 7, 4,
                  5, 6, 2, 1 } };

    polygons.resize(6);

    //0:
    polygons[0].offset = 0;
    polygons[0].vertnum = 4;

    polygons[0].s = vec3(0, 0, dts);
    polygons[0].t = vec3(dts, 0, 0);

    polygons[0].tcoord.x = 0;
    polygons[0].tcoord.y = 0;

    //1:
    polygons[1].offset = 4;
    polygons[1].vertnum = 4;

    polygons[1].s = vec3(0, 0, dts);
    polygons[1].t = vec3(dts, 0, 0);

    polygons[1].tcoord.x = 0;
    polygons[1].tcoord.y = 0;

    //2:
    polygons[2].offset = 8;
    polygons[2].vertnum = 4;

    polygons[2].s = vec3(dts, 0, 0);
    polygons[2].t = vec3(0, dts, 0);

    polygons[2].tcoord.x = 0;
    polygons[2].tcoord.y = 0;

    //3:
    polygons[3].offset = 12;
    polygons[3].vertnum = 4;

    polygons[3].s = vec3(dts, 0, 0);
    polygons[3].t = vec3(0, dts, 0);

    polygons[3].tcoord.x = 0;
    polygons[3].tcoord.y = 0;

    //4:
    polygons[4].offset = 16;
    polygons[4].vertnum = 4;

    polygons[4].s = vec3(0, 0, dts);
    polygons[4].t = vec3(0, dts, 0);

    polygons[4].tcoord.x = 0;
    polygons[4].tcoord.y = 0;

    //5:
    polygons[5].offset = 20;
    polygons[5].vertnum = 4;

    polygons[5].s = vec3(0, 0, dts);
    polygons[5].t = vec3(0, dts, 0);

    polygons[5].tcoord.x = 0;
    polygons[5].tcoord.y = 0;

    block.reset(vertices, indices, polygons);
}

void Block::Cylinder(float height, float radius, int n, bool smooth, bool half, Block& block)
{
    height *= 0.5;

    float dang = (half ? 180.0f : 360.0f) / n;
    float ang = 0;

    if (half) n = n + 1;

    std::vector<vec3> vertices(n * 2);
    std::vector<uint16_t> indices(n * 6);
    std::vector<BlockPolygon> polygons(n + 2);

    for (int i = 0; i < n; i++, ang += dang)
    {
        float a = ang / 180.0 * math::pi;

        float x = radius * cos(a);
        float z = radius * sin(a);

        vertices[i] = vec3(x, height, z);
        vertices[i + n] = vec3(x, -height, z);
    }

    //Top and bottom indices
    for (int i = 0; i < n; i++)
    {
        indices[i] = n - i - 1;
        indices[n + i] = n + i;
    }

    //Top polygon;
    polygons[0].smoothGroop = 0;
    polygons[0].offset = 0;
    polygons[0].vertnum = n;

    polygons[0].s = vec3(dts, 0, 0);
    polygons[0].t = vec3(0, 0, dts);

    polygons[0].tcoord.x = 0;
    polygons[0].tcoord.y = 0;

    //Bottom polygon
    polygons[1].smoothGroop = 0;
    polygons[1].offset = n;
    polygons[1].vertnum = n;

    polygons[1].s = vec3(dts, 0, 0);
    polygons[1].t = vec3(0, 0, dts);

    polygons[1].tcoord.x = 0;
    polygons[1].tcoord.y = 0;

    uint8_t smgroop = smooth ? 1 : 0;

    ang = 0;
    float scale = math::pi * radius;
    
    if (scale < 1) scale = 1;
    else scale = int(scale);

    dang = (dang / 360.0) * scale;

    //side polygons
    for (int i = 0, v = n * 2; i < n; i++, v += 4)
    {
        int k = (i == (n - 1)) ? 0 : i + 1;

        indices[v] = i;
        indices[v + 1] = k;
        indices[v + 2] = k + n;
        indices[v + 3] = i + n;

        polygons[2 + i].smoothGroop = (half && i == (n - 1)) ? 0 : smgroop;
        polygons[2 + i].offset = v;
        polygons[2 + i].vertnum = 4;

        if (half && i == (n - 1))
        {
            polygons[2 + i].s = vec3(dts, 0, 0);
        }
        else
        {
            polygons[2 + i].s = vertices[k] - vertices[i];
            float len = polygons[2 + i].s.normalize();
            polygons[2 + i].s = polygons[2 + i].s * (dang / len);
        }

        polygons[2 + i].t = vec3(0, dts, 0);

        polygons[2 + i].tcoord.x = ang;
        polygons[2 + i].tcoord.y = 0;

        ang += dang;
    }

    block.reset(vertices, indices, polygons);
}

void Block::Cone(float height, float radius, int n, bool smooth, Block& block)
{
    height *= 0.5;

    std::vector<vec3> vertices(n + 1);
    std::vector<uint16_t> indices(n * 4);
    std::vector<BlockPolygon> polygons(n + 1);

    float dang = 360.0 / n;
    float ang = 0;

    for (int i = 0; i < n; i++, ang += dang)
    {
        float a = ang / 180.0 * math::pi;

        float x = radius * cos(a);
        float z = radius * sin(a);

        vertices[i] = vec3(x, -height, z);
    }

    vertices[n] = vec3(0, height, 0);

    //Top and bottom indices
    for (int i = 0; i < n; i++) indices[i] = i;

    //Bottom polygon
    polygons[0].smoothGroop = 0;
    polygons[0].offset = 0;
    polygons[0].vertnum = n;

    polygons[0].s = vec3(dts, 0, 0);
    polygons[0].t = vec3(0, 0, dts);

    polygons[0].tcoord.x = 0;
    polygons[0].tcoord.y = 0;

    //
    uint8_t smgroop = smooth ? 1 : 0;

    ang = 0;
    float scale = math::pi * radius;

    if (scale < 1) scale = 1;
    else scale = int(scale);
    
    dang = (dang / 360) * scale;

    for (int i = 0; i < n; i++)
    {
        int v = n + i * 3;

        int k = i == (n - 1) ? 0 : i + 1;

        indices[v] = n;
        indices[v + 1] = k;
        indices[v + 2] = i;

        polygons[i + 1].smoothGroop = smgroop;
        polygons[i + 1].offset = v;
        polygons[i + 1].vertnum = 3;

        vec3 edge = vertices[k] - vertices[i];
        float len = edge.normalize();

        len *= 0.5;

        vec3 mid = edge * len;
        mid = mid + vertices[i];

        polygons[i + 1].s = edge * (dang / len / 2);
        polygons[i + 1].t = vertices[n] - mid;
        polygons[i + 1].t.normalize();

        polygons[i + 1].tcoord.x = ang;
        polygons[i + 1].tcoord.y = 0;

        ang += dang;
    }

    block.reset(vertices, indices, polygons);
}

void Block::Hemisphere(float radius, int n, bool smooth, Block& block)
{
    int sidepnum = n * (n - 1);

    std::vector<vec3> vertices(sidepnum + 1);
    std::vector<uint16_t> indices(n * n * 4 + n);
    std::vector<BlockPolygon> polygons(sidepnum + n + 1);

    float danga = 90.0 / n;
    float dangb = 360.0 / n;
    float anga = 0;

    for (int i = 0, v = 0; i < n - 1; i++, anga += danga)
    {
        float a = anga / 180.0 * math::pi;

        float r = cos(a);
        float y = sin(a);

        float angb = 0;

        for (int k = 0; k < n; k++, angb += dangb)
        {
            float b = angb / 180.0 * math::pi;

            float x = cos(b) * r;
            float z = sin(b) * r;

            vertices[i * n + k] = vec3(x, y, z) * radius;
        }
    }

    vertices[sidepnum] = vec3(0, radius, 0);

    uint8_t smgroop = smooth ? 1 : 0;

    float ang = 0;
    float scale = math::pi * radius;

    if (scale < 1) scale = 1;
    else scale = int(scale);

    danga = (danga / 180.0) * scale;
    dangb = (dangb / 360.0) * scale;

    int v = 0;

    // side polygons
    for (int l = 0; l < n - 2; l++)
    {
        int base = l * n;

        for (int i = 0; i < n; i++, v += 4)
        {
            int k = (i == (n - 1)) ? 0 : i + 1;

            indices[v] = base + i + n;
            indices[v + 1] = base + k + n;
            indices[v + 2] = base + k;
            indices[v + 3] = base + i;

            int pind = base + i;

            polygons[pind].smoothGroop = smgroop;
            polygons[pind].offset = v;
            polygons[pind].vertnum = 4;

            polygons[pind].s = vertices[base + k] - vertices[base + i];
            float len = polygons[pind].s.normalize();
            polygons[pind].s = polygons[pind].s * (dangb / len);

            vec3 mida = (vertices[base + k] + vertices[base + i]) * 0.5;
            vec3 midb = (vertices[n + base + k] + vertices[n + base + i]) * 0.5;

            polygons[pind].t = midb - mida;
            len = polygons[pind].t.normalize();
            polygons[pind].t = polygons[pind].t * (danga / len);

            polygons[pind].tcoord.x = ang;
            polygons[pind].tcoord.y = mida.y / radius;

            ang += dangb;
        }
    }

    ang = 0;
    int base = n * (n - 2);

    for (int i = 0; i < n; i++, v += 3)
    {
        int k = (i == (n - 1)) ? 0 : i + 1;

        indices[v] = sidepnum;
        indices[v + 1] = base + k;
        indices[v + 2] = base + i;

        int pind = sidepnum + i;

        polygons[pind].smoothGroop = smgroop;
        polygons[pind].offset = v;
        polygons[pind].vertnum = 3;

        vec3 edge = vertices[base + k] - vertices[base + i];
        float len = edge.normalize();

        len *= 0.5;

        vec3 mid = edge * len;
        mid = mid + vertices[base + i];

        polygons[pind].s = edge * (dangb / len / 2);
        polygons[pind].t = vertices[sidepnum] - mid;
        polygons[pind].t.normalize();

        polygons[pind].tcoord.x = ang;
        polygons[pind].tcoord.y = 0;

        ang += dangb;
    }

    for (int i = 0; i < n; i++, v++) indices[v] = i;

    int pind = sidepnum + n;

    //Bottom polygon
    polygons[pind].smoothGroop = 0;
    polygons[pind].offset = v - n;
    polygons[pind].vertnum = n;

    polygons[pind].s = vec3(dts, 0, 0);
    polygons[pind].t = vec3(0, 0, dts);

    polygons[pind].tcoord.x = 0;
    polygons[pind].tcoord.y = 0;

    block.reset(vertices, indices, polygons);
}

void Block::Sphere(float radius, int n, bool smooth, Block& block)
{
    int hn = n * 2;
    int sidepnum = hn * (n - 1);

    std::vector<vec3> vertices(sidepnum + 2);
    std::vector<uint16_t> indices(sidepnum * 4 + hn * 6);
    std::vector<BlockPolygon> polygons(sidepnum + hn * 2);

    float dang = 180.0 / n;
    float anga = -90 + dang;

    for (int i = 0, v = 0; i < n - 1; i++, anga += dang)
    {
        float a = anga / 180.0 * math::pi;

        float r = cos(a);
        float y = sin(a);

        float angb = 0;

        for (int k = 0; k < hn; k++, angb += dang)
        {
            float b = angb / 180.0 * math::pi;

            float x = cos(b) * r;
            float z = sin(b) * r;

            vertices[i*hn + k] = vec3(x, y, z) * radius;
        }
    }

    int topInd = hn * (n - 1);
    int bottomInd = hn * (n - 1) + 1;

    vertices[topInd] = vec3(0, radius, 0);
    vertices[bottomInd] = vec3(0, -radius, 0);

    uint8_t smgroop = smooth ? 1 : 0;

    float ang = 0;
    float scale = math::pi * radius;

    if (scale < 1) scale = 1;
    else scale = int(scale);

    dang = (dang / 180) * scale;

    int v = 0;

    // side polygons
    for (int l = 0; l < n - 2; l++)
    {
        int base = l * hn;

        for (int i = 0; i < hn; i++, v += 4)
        {
            int k = (i == (hn - 1)) ? 0 : i + 1;

            indices[v] = base + i + hn;
            indices[v + 1] = base + k + hn;
            indices[v + 2] = base + k;
            indices[v + 3] = base + i;

            int pind = base + i;

            polygons[pind].smoothGroop = smgroop;
            polygons[pind].offset = v;
            polygons[pind].vertnum = 4;

            polygons[pind].s = vertices[base + k] - vertices[base + i];
            float len = polygons[pind].s.normalize();
            polygons[pind].s = polygons[pind].s * (dang / len);

            vec3 mida = (vertices[base + k] + vertices[base + i]) * 0.5;
            vec3 midb = (vertices[hn + base + k] + vertices[hn + base + i]) * 0.5;

            polygons[pind].t = midb - mida;
            len = polygons[pind].t.normalize();
            polygons[pind].t = polygons[pind].t * (dang / len);

            polygons[pind].tcoord.x = ang;
            polygons[pind].tcoord.y = mida.y / radius;

            ang += dang;
        }
    }

    // bottom cap
    ang = 0;

    for (int i = 0; i < hn; i++, v += 3)
    {
        int k = (i == (hn - 1)) ? 0 : i + 1;

        indices[v] = bottomInd;
        indices[v + 1] = i;
        indices[v + 2] = k;

        int pind = sidepnum + i;

        polygons[pind].smoothGroop = smgroop;
        polygons[pind].offset = v;
        polygons[pind].vertnum = 3;

        vec3 edge = vertices[k] - vertices[i];
        float len = edge.normalize();

        len *= 0.5;

        vec3 mid = edge * len;
        mid = mid + vertices[i];

        polygons[pind].s = edge * (dang / len / 2);
        polygons[pind].t = vertices[bottomInd] - mid;
        polygons[pind].t.normalize();

        polygons[pind].tcoord.x = ang;
        polygons[pind].tcoord.y = 0;

        ang += dang;
    }

    ang = 0;
    int base = hn * (n - 2);

    for (int i = 0; i < hn; i++, v += 3)
    {
        int k = (i == (hn - 1)) ? 0 : i + 1;

        indices[v] = topInd;
        indices[v + 1] = base + k;
        indices[v + 2] = base + i;

        int pind = sidepnum + hn + i;

        polygons[pind].smoothGroop = smgroop;
        polygons[pind].offset = v;
        polygons[pind].vertnum = 3;

        vec3 edge = vertices[base + k] - vertices[base + i];
        float len = edge.normalize();

        len *= 0.5;

        vec3 mid = edge * len;
        mid = mid + vertices[base + i];

        polygons[pind].s = edge * (dang / len / 2);
        polygons[pind].t = vertices[topInd] - mid;
        polygons[pind].t.normalize();

        polygons[pind].tcoord.x = ang;
        polygons[pind].tcoord.y = 0;

        ang += dang;
    }

    block.reset(vertices, indices, polygons);
}

Block::Block()
: m_type(BlockType::Edit)
, m_pos(0, 0, 0)
, m_scale(1, 1, 1)
, m_selected(false)
{
}

Block::Block(const std::vector<vec3>& vertices, const std::vector<uint16_t>& indices, const std::vector<BlockPolygon>& polygons, BlockType type)
: m_vertices(vertices)
, m_indices(indices)
, m_polygons(polygons)
, m_type(type)
, m_scale(1, 1, 1)
, m_selected(false)
{
    init();
}

Block::Block(std::vector<vec3>&& vertices, std::vector<uint16_t>&& indices, std::vector<BlockPolygon>&& polygons, BlockType type)
: m_vertices(vertices)
, m_indices(indices)
, m_polygons(polygons)
, m_type(type)
, m_scale(1, 1, 1)
, m_selected(false)
, m_needUpdate(false)
{
    init();
}

Block::Block(const Block& block)
: m_vertices(block.m_vertices)
, m_indices(block.m_indices)
, m_polygons(block.m_polygons)
, m_type(block.m_type)
, m_pos(block.m_pos)
, m_scale(1, 1, 1)
, m_selected(false)
, m_needUpdate(false)
{
    init();
}

Block::Block(const Block& block, BlockType type)
: m_vertices(block.m_vertices)
, m_indices(block.m_indices)
, m_polygons(block.m_polygons)
, m_type(type)
, m_pos(block.m_pos)
, m_scale(1, 1, 1)
, m_selected(false)
, m_needUpdate(false)
{
    init();
}

void Block::copy(const Block& block)
{
    m_vertices = block.m_vertices;
    m_indices = block.m_indices;
    m_polygons = block.m_polygons;

    init();
}

void Block::reset(std::vector<vec3>& vertices, std::vector<uint16_t>& indices, std::vector<BlockPolygon>& polygons)
{
    m_vertices = vertices;
    m_indices = std::move(indices);
    m_polygons = std::move(polygons);

    init();
}

void Block::setMaterial(Material* material)
{
    for (BlockPolygon& poly : m_polygons) poly.material = material;
}

void Block::updateBBox()
{
    if (m_vertices.empty())
    {
        m_bbox = { {}, {} };
        return;
    }

    vec3 min = m_vertices[0];
    vec3 max = m_vertices[0];

    for (int i = 1; i < m_vertices.size(); i++)
    {
        if (m_vertices[i].x < min.x) min.x = m_vertices[i].x;
        if (m_vertices[i].y < min.y) min.y = m_vertices[i].y;
        if (m_vertices[i].z < min.z) min.z = m_vertices[i].z;

        if (m_vertices[i].x > max.x) max.x = m_vertices[i].x;
        if (m_vertices[i].y > max.y) max.y = m_vertices[i].y;
        if (m_vertices[i].z > max.z) max.z = m_vertices[i].z;
    }

    m_bbox = { min - vec3(math::eps), max + vec3(math::eps) };
}

void Block::init()
{
    std::vector<uint16_t> displayIndices;

    for (BlockPolygon& poly : m_polygons)
    {
        for (int i = 0; i < poly.vertnum; i++)
        {
            uint16_t current = poly.offset + i;
            uint16_t next = i < (poly.vertnum - 1) ? current + 1 : poly.offset;

            displayIndices.push_back(m_indices[current]);
            displayIndices.push_back(m_indices[next]);
        }

        if (poly.surface)
        {
            poly.surface->link(this, &poly);
            m_surfaces.push_back(poly.surface.get());
        }
    }

    m_indexBuffer.setData(displayIndices.data(), displayIndices.size());

    m_displayVerts = displayIndices.size();

    updateBBox();
}

bool Block::pick(const vec3& origin, const vec3& ray, vec3& point, float& dist) const
{
    for (const BlockPolygon& poly : m_polygons)
    {
        for (int i = 0; i < poly.vertnum; i++)
        {
            uint16_t begin = poly.offset + i;
            uint16_t end = i < (poly.vertnum - 1) ? begin + 1 : poly.offset;

            const vec3& a = m_vertices[m_indices[begin]] + m_pos;
            const vec3& b = m_vertices[m_indices[end]] + m_pos;

            vec3 edge = b - a;
            float len = edge.normalize();

            if (fabs(1.0 - fabs(ray * edge)) < math::eps) continue;

            float s, t;

            RayIntersect(origin, a, ray, edge, s, t);

            if (s < 0) continue;
            if (t < 0) continue;
            if (t > len) continue;
        
            vec3 rayPt = origin + ray * s;
            vec3 edgePt = a + edge * t;

            if ((edgePt - rayPt).length() < 0.01 * s)
            {              
                point = rayPt;
                dist = s;

                return true;
            }
        }
    }

    return false;
}

bool Block::pick2d(float x, float y, float scale, float& depth, int i, int j, int k)
{
    for (const BlockPolygon& poly : m_polygons)
    {
        for (int v = 0; v < poly.vertnum; v++)
        {
            uint16_t begin = poly.offset + v;
            uint16_t end = v < (poly.vertnum - 1) ? begin + 1 : poly.offset;

            vec2 a = { m_vertices[m_indices[begin]][i] + m_pos[i], m_vertices[m_indices[begin]][j] + m_pos[j] };
            vec2 b = { m_vertices[m_indices[end]][i] + m_pos[i], m_vertices[m_indices[end]][j] + m_pos[j] };

            float adepth = m_vertices[m_indices[begin]][k];
            float bdepth = m_vertices[m_indices[end]][k];

            vec2 edge = b - a;
            vec2 dir = vec2(x, y) - a;

            float dist;

            if (edge.length() < math::eps)
            {
                dist = dir.length();
                depth = adepth > bdepth ? adepth : bdepth;
                depth += m_pos[k];
            }
            else
            {
                float len = edge.normalize();
                float proj = edge * dir;
                
                if (proj < 0 || proj > len) 
                    continue;

                dist = edge ^ dir;

                float factor = proj / len;
                depth = adepth * (1.0 - factor) + bdepth * factor + m_pos[k];
            }
            
            if (fabs(dist) < PickDist2d / scale) return true;
        }
    }

    return false;
}

vec3* Block::pickVertex(const vec3& origin, const vec3& ray, float& pickDist)
{
    vec3* pickPt = nullptr;

    for (int i = 0; i < m_vertices.size(); i++)
    {
        vec3 vert = m_vertices[i] + m_pos;
        vec3 pt = vert - origin;
        float raydist = pt * ray;

        if (raydist < 0) continue;

        vec3 projPt = origin + ray * raydist;

        float dist = (projPt - vert).length();

        if (dist < 0.01 * raydist)
        {
            if (!pickPt || raydist < pickDist)
            {
                pickPt = &m_vertices[i];
                pickDist = raydist;
            }
        }
    }

    return pickPt;
}

bool Block::pickVertex2d(float x, float y, float scale, int i, int k, PointList& pointList)
{
    bool result = false;

    for (int v = 0; v < m_vertices.size(); v++)
    {
        vec3 vert = m_vertices[v] + m_pos;

        float dist = vec2(x - vert[i], y - vert[k]).length();

        if (dist < BlockPoint::PickDist / scale)
        {
            BlockPoint* blockPt = new BlockPoint(&m_vertices[v], this);
            pointList.append(blockPt);

            result = true;
        }
    }

    return result;
}

void Block::calculateMoveDiff(const vec3& point)
{
    m_pickdiff = m_pos - point;
}

void Block::pickMove(const vec3& newpt)
{
    m_pos = newpt + m_pickdiff;
}

void Block::pickMove2d(float x, float y, int i, int k)
{
    m_pos[i] = x + m_pickdiff[i];
    m_pos[k] = y + m_pickdiff[k];
}

void Block::pickRotateX(float Cos, float Sin, const vec3& center)
{
    float y = Cos * m_pickdiff.y + Sin * m_pickdiff.z;
    float z = -Sin * m_pickdiff.y + Cos * m_pickdiff.z;

    m_pos.y = center.y + y;
    m_pos.z = center.z + z;
}

void Block::pickRotateY(float Cos, float Sin, const vec3& center)
{
    float x = Cos * m_pickdiff.x - Sin * m_pickdiff.z;
    float z = Sin * m_pickdiff.x + Cos * m_pickdiff.z;

    m_pos.x = center.x + x;
    m_pos.z = center.z + z;
}

void Block::pickRotateZ(float Cos, float Sin, const vec3& center)
{
    float x = Cos * m_pickdiff.x + Sin * m_pickdiff.y;
    float y = -Sin * m_pickdiff.x + Cos * m_pickdiff.y;

    m_pos.x = center.x + x;
    m_pos.y = center.y + y;
}

void Block::pickScale(const vec3& center, const vec3& scale)
{
    vec3 scaledDiff = { m_pickdiff.x * scale.x, m_pickdiff.y * scale.y, m_pickdiff.z * scale.z };

    m_pos = center + scaledDiff;
    m_scale = scale;
}

EditPolygon* Block::pickPolygon(const vec3& origin, const vec3& ray, float& dist)
{
    EditPolygon* pickPoly = nullptr;

    for (EditPolygon& poly : m_editPolygons)
    {
        if (!poly.origin->surface)
        {
            if (m_type != BlockType::Subtruct && poly.status == VolumeStatus::Inside) continue;
            if (m_type == BlockType::Subtruct && poly.status == VolumeStatus::Outside) continue;
        }
    
        float pdist;

        bool intersect = poly.origin->surface ? poly.origin->surface->pick(origin - m_pos, ray, pdist) :
                                                PolyIntersect(poly, origin, ray, pdist, m_type == BlockType::Subtruct);

        if (intersect)
        {
            if (!pickPoly || pdist < dist)
            {
                pickPoly = &poly;
                dist = pdist;
            }
        }
    }

    return pickPoly;
}

bool Block::pickSurface(const vec3& origin, const vec3& ray, float& dist)
{
    bool pick = false;
    float pickDist;

    vec3 localOrigin = origin - m_pos;

    for (const EditSurface* surface : m_surfaces)
    {
        float surfdist;

        if (surface->pick(localOrigin, ray, surfdist))
        {
            if (!pick || surfdist < dist)
            {
                dist = surfdist;
                pick = true;
            }
        }
    }

    return pick;
}

void Block::displaceSurfaces(const vec3& center, float power, float radius)
{
    for (EditSurface* surface : m_surfaces)
    {
        const BBox& bbox = surface->bbox();

        vec3 spos = (bbox.min + bbox.max) * 0.5;
        vec3 sbox = bbox.max - spos;

        if (AABBTest(spos + m_pos, sbox, center, { radius, radius, radius }))
            surface->displace(center - m_pos, power, radius);
    }
}

void Block::collectSurfaceVerts(const vec3& center, float radius, std::vector<SurfaceVertexLink>& vlist)
{
    for (EditSurface* surface : m_surfaces)
    {
        const BBox& bbox = surface->bbox();

        vec3 spos = (bbox.min + bbox.max) * 0.5;
        vec3 sbox = bbox.max - spos;

        if (AABBTest(spos + m_pos, sbox, center, { radius, radius, radius }))
            surface->collectVertices(center, radius, vlist);
    }
}

void Block::convolveSurfaceVerts(const vec3& center, float radius, float& value, vec3& norm, float& num) const
{
    for (EditSurface* surface : m_surfaces)
    {
        const BBox& bbox = surface->bbox();

        vec3 spos = (bbox.min + bbox.max) * 0.5;
        vec3 sbox = bbox.max - spos;

        if (AABBTest(spos + m_pos, sbox, center, { radius, radius, radius }))
            surface->convolve(center, radius, value, norm, num);
    }
}

void Block::subdividePolygon(BlockPolygon* poly, size_t factor)
{
    if (m_surfaces.empty()) clearPolygons();

    poly->surface.reset(new EditSurface(this, poly, factor));
    m_surfaces.push_back(poly->surface.get());
}

SurfaceGraph* Block::getSurfaceGraph()
{
    if (m_surfaces.empty()) return nullptr;

    return m_surfaces[0]->surfaceGraph();
}

void Block::applyRotation()
{
    mat4 rotMat = mat4::Rotate(m_rot.x, m_rot.y, m_rot.z);

    for (int i = 0; i < m_vertices.size(); i++)
    {
        m_vertices[i] = rotMat * m_vertices[i];
    }

    for (int i = 0; i < m_polygons.size(); i++)
    {
        m_polygons[i].s = rotMat * m_polygons[i].s;
        m_polygons[i].t = rotMat * m_polygons[i].t;
    }

    for (EditSurface* surface : m_surfaces) surface->applyTransform(rotMat);

    m_rot = { 0, 0, 0 };
}

void Block::applyScale()
{
    for (int i = 0; i < m_vertices.size(); i++)
    {
        m_vertices[i].x *= m_scale.x;
        m_vertices[i].y *= m_scale.y;
        m_vertices[i].z *= m_scale.z;
    }

    for (EditSurface* surface : m_surfaces) surface->scale(m_scale);

    m_scale = { 1, 1, 1 };
}

bool Block::hasSubdivision()
{
    return !m_surfaces.empty();
}

bool Block::hasCsg()
{
    return !m_csgTree.empty();
}

void Block::clearGeometry()
{
    m_editPolygons.clear();
    m_csgTree.reset();
}

void Block::clearPolygons()
{
    m_geometry.clear();
    m_displayData.clear();
}

void Block::smoothPolygons(uint8_t smgroop)
{
    size_t vnum = m_vertices.size();

    // Setup buffers
    if (m_tbuffer.empty())
    {
        m_tbuffer.resize(vnum);
        m_vcount.resize(vnum);
    }
    else
    {
        for (int v = 0; v < m_tbuffer.size(); v++)
        {
            m_tbuffer[v].normal = {};
            m_tbuffer[v].tangent = {};
            m_tbuffer[v].binormal = {};
        }
    }

    for (int v = 0; v < m_tbuffer.size(); v++) m_vcount[v] = 0;

    for (int p = 0; p < m_editPolygons.size(); p++)
    {
        BlockPolygon& poly = *m_editPolygons[p].origin;
        VertexList& verts = m_editPolygons[p].vertices;

        if (poly.smoothGroop != smgroop) continue;

        uint16_t index = poly.offset;

        for (int v = 0; v < poly.vertnum; v++, index++)
        {
            uint16_t vind = m_indices[index];

            m_tbuffer[vind].normal += verts[v].normal;
            m_tbuffer[vind].tangent += verts[v].tangent;
            m_tbuffer[vind].binormal += verts[v].binormal;

            m_vcount[vind]++;
        }
    }

    //Find average
    for (int v = 0; v < vnum; v++)
    {
        if (m_vcount[v] == 0) continue;

        float invc = 1.0 / m_vcount[v];

        m_tbuffer[v].normal *= invc;
        m_tbuffer[v].tangent *= invc;
        m_tbuffer[v].binormal *= invc;

        m_tbuffer[v].normal.normalize();
        m_tbuffer[v].tangent.normalize();
        m_tbuffer[v].binormal.normalize();
    }

    for (int p = 0; p < m_editPolygons.size(); p++)
    {
        BlockPolygon& poly = *m_editPolygons[p].origin;
        VertexList& verts = m_editPolygons[p].vertices;

        if (poly.smoothGroop != smgroop) continue;

        uint16_t index = poly.offset;

        for (int v = 0; v < poly.vertnum; v++, index++)
        {
            uint16_t vind = m_indices[index];

            verts[v].normal = m_tbuffer[vind].normal;
            verts[v].tangent = m_tbuffer[vind].tangent;
            verts[v].binormal = m_tbuffer[vind].binormal;
        }
    }
}

void Block::generatePolygons()
{
    std::set<uint8_t> smgroops;

    m_editPolygons.clear();

    for (const BlockPolygon& poly : m_polygons)
    {
        EditPolygon edpoly;

        vec3 v1 = m_vertices[m_indices[poly.offset]] + m_pos;
        vec3 v2 = m_vertices[m_indices[poly.offset + 1]] + m_pos;
        vec3 v3 = m_vertices[m_indices[poly.offset + 2]] + m_pos;

        vec3 a = v2 - v1;
        vec3 b = v3 - v1;

        vec3 norm = a ^ b;
        norm.normalize();
        float dist  = -(v1 * norm);

        edpoly.origin = const_cast<BlockPolygon*>(&poly);
        edpoly.plane = {norm, dist};
        edpoly.status = VolumeStatus::Inside;
        edpoly.splitter = (m_type == BlockType::Solid);
        edpoly.border = false;
        edpoly.flags = poly.flags;
        edpoly.material = poly.material;

        if (poly.smoothGroop != 0) smgroops.insert(poly.smoothGroop);

        for (int v = 0; v < poly.vertnum; v++)
        {
            Vertex vert;

            uint16_t index = poly.offset + v;

            vert.position = m_vertices[m_indices[index]] + m_pos;

            vert.tcoord.x = m_vertices[m_indices[index]] * poly.s + poly.tcoord.x;
            vert.tcoord.y = m_vertices[m_indices[index]] * -poly.t + poly.tcoord.y;

            vert.normal = (m_type == BlockType::Subtruct) ? -norm : norm;

            vert.tangent = poly.s - norm * (norm * poly.s);
            vert.binormal = poly.t - norm * (norm * poly.t);

            vert.tangent.normalize();
            vert.binormal.normalize();

            edpoly.vertices.push_back(vert);
        }

        m_editPolygons.push_back(std::move(edpoly));
    }

    for (uint8_t smgroop : smgroops) smoothPolygons(smgroop);

    m_csgTree.build(m_editPolygons);
}

void Block::buildGeometry()
{
    if (!m_surfaces.empty()) return;

    m_geometry.clear();
    m_displayData.clear();

    cleanEdPolyLists();

    uint32_t offset = 0;
    size_t displayId = 0;

    for (const EditPolygon& poly : m_editPolygons)
    {
        if (m_type != BlockType::Subtruct && poly.status != VolumeStatus::Outside) continue;
        if (m_type == BlockType::Subtruct && poly.status != VolumeStatus::Inside) continue;

        poly.origin->editPolygons.push_back(const_cast<EditPolygon*>(&poly));

        m_displayData.push_back({poly.material, offset, (uint32_t)poly.vertices.size()});
        poly.origin->displayList.push_back(displayId);
        offset += poly.vertices.size();
        displayId++;

        for (int i = 0; i < poly.vertices.size(); i++)
        {
            size_t v = (m_type == BlockType::Subtruct) ? 
                       ((i % 2 == 0) ? i / 2 : poly.vertices.size() - 1 - i / 2) :
                       ((i % 2 == 0) ? poly.vertices.size() - 1 - i / 2 : i / 2);
                       
            m_geometry.push(poly.vertices[v]);
        }
    }

    m_needUpdate = false;
}

void Block::collectPolygons(PolygonList& polygons) const
{
    if (!m_surfaces.empty()) return;

    for (const EditPolygon& poly : m_editPolygons)
    {
        if (poly.origin->surface) continue;

        if (m_type != BlockType::Subtruct && poly.status != VolumeStatus::Outside) continue;
        if (m_type == BlockType::Subtruct && poly.status != VolumeStatus::Inside) continue;

        if (m_type == BlockType::Subtruct) // revert vertices
        {
            std::vector<Vertex> verts;

            for (int i = poly.vertices.size() - 1; i >= 0; i--)
            {
                verts.push_back({ poly.vertices[i].position, 
                                  poly.vertices[i].tcoord,
                                  poly.vertices[i].normal,
                                  poly.vertices[i].tangent,
                                  poly.vertices[i].binormal });
            }

            bool splitter = poly.splitter && ((poly.flags & PolyZonePortal) == 0);

            polygons.push_back({ nullptr, -poly.plane, {}, splitter, false, poly.flags, poly.material, std::move(verts) });
        
        }
        else
        {
            bool splitter = poly.splitter && ((poly.flags & PolyZonePortal) == 0);

            polygons.push_back({ nullptr, poly.plane, {}, splitter, false, poly.flags, poly.material, poly.vertices });
        }
    }
}

void Block::cleanEdPolyLists()
{
    for (BlockPolygon& poly : m_polygons)
    {
        poly.editPolygons.clear();
        poly.displayList.clear();
    }
}

void Block::categorizeBlockPolygons(Block& block) const
{
    PolygonList polygons;

    m_csgTree.categorizePolygons(m_type == BlockType::Subtruct, block.type() != BlockType::Subtruct, block.m_editPolygons, polygons);

    std::swap(block.m_editPolygons, polygons);
}

void Block::clipBlock(Block& block) const
{
    PolygonList polygons;

    m_csgTree.clipPolygons(m_type == BlockType::Subtruct, block.type() != BlockType::Subtruct, block.m_editPolygons, polygons);

    std::swap(block.m_editPolygons, polygons);
}

void Block::setMaterial(Material* mat, std::vector<size_t>& displayList)
{
    for (size_t index : displayList)
    {
        Render::DisplayData& elem = m_displayData[index];
        elem.material = mat;
    }
}

void Block::generateUV(const BlockPolygon* poly)
{
    for (size_t index : poly->displayList)
    {
        Render::DisplayData& elem = m_displayData[index];

        for (int i = 0; i < elem.vertexnum; i++)
        {
            size_t vind = elem.offset + i;

            const vec3& position = m_geometry[vind].position - m_pos;
            m_geometry[vind].tcoord.x = position * poly->s + poly->tcoord.x;
            m_geometry[vind].tcoord.y = position * -poly->t + poly->tcoord.y;
        }
    }

    for (EditPolygon* edpoly : poly->editPolygons)
    {
        for (int i = 0; i < edpoly->vertices.size(); i++)
        {
            const vec3& position = edpoly->vertices[i].position;
            edpoly->vertices[i].tcoord.x = position * poly->s + poly->tcoord.x;
            edpoly->vertices[i].tcoord.y = position * -poly->t + poly->tcoord.y;
        }
    }
}

void Block::bindVertexBuffer(Render::CommandList& commandList) const
{
    commandList.bindVertexBuffer(m_geometry);
}

void Block::display(Render::CommandList& commandList) const
{
    float intensity = m_selected ? 0.8 : 0.5;

    vec4 color;

    switch (m_type)
    {
    case BlockType::Edit: color = { intensity, 0.0, 0.0, 1.0 }; break;
    case BlockType::Add: color = { intensity, 0.0, intensity, 1.0 }; break;
    case BlockType::Subtruct: color = { 0.0, intensity, 0.0, 1.0 }; break;
    case BlockType::Solid: color = { intensity, intensity, 0.0, 1.0 }; break;
    }

    commandList.setConstant(2, color);
    commandList.setConstant(1, mat4::Translate(m_pos) * mat4::Rotate(m_rot.x, m_rot.y, m_rot.z) * mat4::Scale(m_scale));

    commandList.bindVertexBuffer(m_vertices);
    commandList.bindIndexBuffer(m_indexBuffer);

    commandList.drawIndexed(m_displayVerts);
}

void Block::displayVertices(Render::CommandList& commandList) const
{
    commandList.setConstant(1, mat4::Translate(m_pos));
    commandList.bindVertexBuffer(m_vertices);

    commandList.draw(m_vertices.size());
}

void Block::displayGeometry(Render::CommandList& commandList) const
{
    commandList.bindVertexBuffer(m_geometry);

    for (const Render::DisplayData& elem : m_displayData)
    {
        commandList.bind(3, elem.material->maps[Material::map_diffuse]);
        commandList.draw(elem.vertexnum, elem.offset);
    }
}

void Block::displayGeometry(Render::CommandList& commandList, const std::vector<size_t>& displayList) const
{
    if (m_displayData.empty()) return;

    for (size_t index : displayList)
    {
        const Render::DisplayData& elem = m_displayData[index];
        commandList.draw(elem.vertexnum, elem.offset);
    }
}

void Block::displaySurfaces(Render::CommandList& commandList) const
{
    if (m_surfaces.empty()) return;

    commandList.setConstant(1, mat4::Translate(m_pos) * mat4::Rotate(m_rot.x, m_rot.y, m_rot.z) * mat4::Scale(m_scale));

    for (const EditSurface* surface : m_surfaces)
    {
        const Material* material = surface->material();
        commandList.bind(3, material->maps[Material::map_diffuse]);
        surface->display(commandList);
    }
}

void Block::displaySurfaceLayers(Render::CommandList& commandList) const
{
    if (m_surfaces.empty()) return;

    commandList.setConstant(1, mat4::Translate(m_pos) * mat4::Rotate(m_rot.x, m_rot.y, m_rot.z) * mat4::Scale(m_scale));

    for (const EditSurface* surface : m_surfaces) surface->displayLayers(commandList);
}

void Block::write(FILE* file) const
{
    fwrite(&m_type, sizeof(BlockType), 1, file);

    fwrite(&m_pos, sizeof(vec3), 1, file);

    uint32_t vnum = m_vertices.size();
    uint32_t inum = m_indices.size();
    uint32_t pnum = m_polygons.size();

    fwrite(&vnum, sizeof(uint32_t), 1, file);
    fwrite(&inum, sizeof(uint32_t), 1, file);
    fwrite(&pnum, sizeof(uint32_t), 1, file);

    fwrite(m_vertices.data(), sizeof(vec3), vnum, file);
    fwrite(m_indices.data(), sizeof(uint16_t), inum, file);

    //Polygons
    for (unsigned long i = 0; i < m_polygons.size(); i++)
    {
        const BlockPolygon& poly = m_polygons[i];

        fwrite(&poly.flags, sizeof(uint8_t), 1, file);

        fwrite(&poly.smoothGroop, sizeof(uint8_t), 1, file);
        fwrite(&poly.offset, sizeof(uint16_t), 1, file);
        fwrite(&poly.vertnum, sizeof(uint16_t), 1, file);

        fwrite(&poly.s, sizeof(vec3), 1, file);
        fwrite(&poly.t, sizeof(vec3), 1, file);
        fwrite(&poly.tcoord, sizeof(vec2), 1, file);

        //texture name
        if (m_type == BlockType::Edit)
        {
            uint16_t tlen = 0;
            fwrite(&tlen, sizeof(uint16_t), 1, file);
        }
        else
        {
            const std::string& tname = poly.material->name;
            uint16_t tlen = tname.size();
            fwrite(&tlen, sizeof(uint16_t), 1, file);
            fwrite(tname.data(), 1, tlen, file);
        }

        if (poly.surface) poly.surface->write(file);
        else
        {
            uint16_t zero = 0;
            fwrite(&zero, sizeof(uint16_t), 1, file);
        }
    }
}

void Block::writeSurfaces(FILE* file) const
{
    for (const EditSurface* surface : m_surfaces) surface->writeGameInfo(file);
}