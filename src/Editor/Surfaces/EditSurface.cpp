#include "EditSurface.h"
#include "Surface.h"

#include "Block.h"

#include <algorithm>

#undef max
#undef min

EditSurface::EditSurface(Block* block, BlockPolygon* polygon, size_t size)
: SurfaceMesh(size, size)
, m_owner(block)
, m_polygon(polygon)
{
    m_material = polygon->material;

    m_normals.resize(m_xsize * m_ysize);

    m_tempVertices.resize(m_xsize * m_ysize);
    m_tempNormals.resize(m_xsize * m_ysize);

    tesselate(block, polygon);
    initNormals(block, polygon);
}

EditSurface::EditSurface(BlockPolygon* polygon, size_t size,
                        const std::vector<TexturedVertex>& vertices,
                        std::vector<vec3>& normals)
{
    m_xsize = size;
    m_ysize = size;

    initIndices();

    size_t datasize = m_xsize * m_ysize;

    m_vertexBuffer.resize(datasize);
    memcpy(m_vertexBuffer.data(), vertices.data(), datasize * sizeof(TexturedVertex));
    
    m_normals = std::move(normals);

    m_tempVertices.resize(m_xsize * m_ysize);
    m_tempNormals.resize(m_xsize * m_ysize);

    m_polygon = polygon;
    m_material = polygon->material;

    updateBBox();
}

void EditSurface::tesselate(const Block* block, const BlockPolygon* poly)
{
    unsigned long i1 = block->index(poly->offset);
    unsigned long i2 = block->index(poly->offset + 1);
    unsigned long i3 = block->index(poly->offset + 2);
    unsigned long i4 = block->index(poly->offset + 3);

    vec3 a = block->vertex(i2) - block->vertex(i1);
    vec3 b = block->vertex(i4) - block->vertex(i1);
    vec3 c = block->vertex(i3) - block->vertex(i2);

    float fx = 1 / (float)(m_xsize - 1);
    float fy = 1 / (float)(m_ysize - 1);

    a = a * fy;
    b = b * fy;
    c = c * fy;

    //Tesselate
    vec3 e1 = block->vertex(i1);
    vec3 e2 = block->vertex(i2);

    size_t vptr = 0;

    for (int i = 0; i < m_ysize; i++)
    {
        vec3 edge = e2 - e1;
        edge = edge * fx;

        vec3 vert = e1;

        for (int k = 0; k < m_xsize; k++)
        {
            m_vertexBuffer[vptr].position = vert;
            m_vertexBuffer[vptr].tcoord.x = vert * poly->s + poly->tcoord.x;
            m_vertexBuffer[vptr].tcoord.y = vert * -poly->t + poly->tcoord.y;
            vptr++;

            vert += edge;
        }

        e1 = e1 + b;
        e2 = e2 + c;
    }

    updateBBox();
}

void EditSurface::initNormals(const Block* block, const BlockPolygon* poly)
{
    unsigned long i1 = block->index(poly->offset);
    unsigned long i2 = block->index(poly->offset + 1);
    unsigned long i3 = block->index(poly->offset + 2);

    vec3 v1 = block->vertex(i1);
    vec3 v2 = block->vertex(i2);
    vec3 v3 = block->vertex(i3);

    vec3 a = v2 - v1;
    vec3 b = v3 - v1;

    vec3 norm = a ^ b;
    norm.normalize();

    size_t vptr = 0;

    for (int i = 0; i < m_ysize; i++)
    {
        for (int k = 0; k < m_xsize; k++)
        {
            m_normals[vptr] = norm;
            vptr++;
        }
    }
}

void EditSurface::link(Block* block, BlockPolygon* polygon)
{
    m_owner = block;
    m_polygon = polygon;
}

void EditSurface::updateTempBuffers()
{
    for (size_t i = 0; i < m_xsize * m_ysize; i++)
    {
        m_tempVertices[i] = m_vertexBuffer[i].position;
        m_tempNormals[i] = m_normals[i];
    }
}

void EditSurface::applyChanges()
{
    for (size_t i = 0; i < m_xsize * m_ysize; i++) m_vertexBuffer[i].position = m_tempVertices[i];
    std::swap(m_normals, m_tempNormals);
}

void EditSurface::applyTransform(const mat4& mat)
{
    for (int i = 0; i < m_vertexBuffer.size(); i++)
    {
        vec3& vert = m_vertexBuffer[i].position;
        vec3& norm = m_normals[i];

        vert = mat * vert;
        norm = mat * norm;
    }

    updateBBox();
}

void EditSurface::scale(const vec3& scale)
{
    for (int i = 0; i < m_vertexBuffer.size() ; i++)
    {
        vec3& vert = m_vertexBuffer[i].position;
        vert.x *= scale.x;
        vert.y *= scale.y;
        vert.z *= scale.z;

        vec3& norm = m_normals[i];
        norm.x *= scale.x;
        norm.y *= scale.y;
        norm.z *= scale.z;

        norm.normalize();
    }

    updateBBox();
}

void EditSurface::addLayer(Material* material)
{
    m_layers.emplace_back(material, m_xsize * m_ysize);
}

void EditSurface::addLayer(Material* material, LayerOrientation orientation, const std::vector<float>& layerMask)
{
    m_layers.emplace_back(material, orientation, layerMask);
}

void EditSurface::deleteLayer(size_t n)
{
    m_layers.erase(m_layers.begin() + n);
}

void EditSurface::addDetails(size_t layer, const std::string& model, const std::string& material, float density)
{
    m_layerDetails.emplace_back(layer, model, material, density);
}

void EditSurface::removeDetails(size_t n)
{
    m_layerDetails.erase(m_layerDetails.begin() + n);
}

void EditSurface::clearDetails()
{
    m_layerDetails.clear();
}

void EditSurface::displace(const vec3& point, float power, float radius)
{
    for (int i = 0; i < m_vertexBuffer.size(); i++)
    {
        vec3& vert = m_vertexBuffer[i].position;

        float dist = (point - vert).length();

        if (dist < radius)
        {
            float factor = (1.0 - dist / radius) * power;
            vert += m_normals[i] * factor;
        }
    }

    updateBBox();
}

void EditSurface::paintLayer(const vec3& point, float radius, size_t lid)
{
    if (m_layers.size() <= lid) return;

    SurfaceLayer& layer = m_layers[lid];

    for (int i = 0; i < m_vertexBuffer.size(); i++)
    {
        vec3& vert = m_vertexBuffer[i].position;

        float dist = (point - vert).length();

        if (dist < radius)
        {
            constexpr float falloff = 3.0f;

            float rdst = std::max(0.0f, radius - dist);
            float value = rdst > falloff ? 1.0 : rdst / falloff;
            float alpha = std::min(1.0f, layer.vertexBuffer[i] + value * 0.2f);

            layer.vertexBuffer[i] = alpha;
        }
    }
}

void EditSurface::collectVertices(const vec3& center, float radius, std::vector<SurfaceVertexLink>& vlist)
{
    size_t vptr = 0;

    vec3 point = center - m_owner->pos();

    for (int i = 0; i < m_ysize; i++)
    {
        for (int k = 0; k < m_xsize; k++)
        {
            vec3& vert = m_vertexBuffer[vptr].position;

            float dist = (point - vert).length();

            if (dist < radius) vlist.push_back({m_owner->pos(), vert, m_normals[vptr]});

            vptr++;
        }
    }
}

void EditSurface::convolve(const vec3& center, float radius, float& value, vec3& norm, float& num) const
{
    size_t vptr = 0;

    vec3 point = center - m_owner->pos();

    for (int i = 0; i < m_ysize; i++)
    {
        for (int k = 0; k < m_xsize; k++)
        {
            const vec3& vert = m_vertexBuffer[vptr].position;
            const vec3& vnorm = m_normals[vptr];

            float dist = (point - vert).length();

            if (dist < radius)
            {
                value += vert * vnorm;// +m_owner->pos();
                norm += vnorm;
                num++;
            }

            vptr++;
        }
    }
}

void EditSurface::buildVertices()
{
    size_t ptr = 0;

    m_vertices.resize(m_xsize * m_ysize);

    for (int k = 0; k < m_ysize; k++)
    {
        for (int i = 0; i < m_xsize; i++)
        {
            m_vertices[ptr].position = vertex(i, k).position;
            m_vertices[ptr].tcoord = vertex(i, k).tcoord;

            int x = (i == m_xsize - 1) ? m_xsize - 2 : i;
            int y = (k == m_ysize - 1) ? m_ysize - 2 : k;

            int i1 = y * m_xsize + x;
            int i2 = (y + 1) * m_xsize + x;
            int i3 = y * m_xsize + x + 1;

            const vec3& a = vertex(i1).position;
            const vec3& b = vertex(i2).position;
            const vec3& c = vertex(i3).position;

            vec3 tangent;
            vec3 binormal;

            tangent = b - a;
            binormal = a - c;

            vec3 normal = tangent ^ binormal;

            tangent.normalize();
            binormal.normalize();
            normal.normalize();

            m_vertices[ptr].normal = normal;

            const vec2& ta = vertex(i1).tcoord;
            const vec2& tb = vertex(i2).tcoord;
            const vec2& tc = vertex(i3).tcoord;

            vec3 s;
            vec3 t;

            TriangleTangentSpace(a, b, c, ta, tb, tc, s, t);

            m_vertices[ptr].tangent = s;
            m_vertices[ptr].binormal = -t;

            ptr++;
        }
    }
}

void EditSurface::displayLayers(Render::CommandList& commandList) const
{
    if (m_layers.empty()) return;

    commandList.bindVertexBuffer(m_vertexBuffer);
    commandList.bindIndexBuffer(m_indexBuffer);

    for (size_t i = 0; i < m_layers.size(); i++)
    {
        commandList.bind(3, m_layers[i].material->maps[Material::map_diffuse]);

        commandList.bindVertexBuffer(m_layers[i].vertexBuffer, 1);
        commandList.drawIndexed(m_indexNum);
    }
}

void EditSurface::writeLayers(FILE* file) const
{
    uint8_t layers = m_layers.size();
    fwrite(&layers, sizeof(uint8_t), 1, file);

    for (size_t l = 0; l < m_layers.size(); l++)
    {
        fwrite(&m_layers[l].orientation, sizeof(uint8_t), 1, file);

        uint16_t tlen = m_layers[l].material->name.size();
        fwrite(&tlen, 1, sizeof(uint16_t), file);
        fwrite(m_layers[l].material->name.c_str(), 1, tlen, file);

        fwrite(m_layers[l].vertexBuffer.data(), sizeof(float), m_layers[l].vertexBuffer.size(), file);
    }
}

void EditSurface::writeLayerDetails(FILE* file) const
{
    uint8_t details = m_layerDetails.size();
    fwrite(&details, sizeof(uint8_t), 1, file);

    for (size_t l = 0; l < m_layerDetails.size(); l++)
    {
        uint8_t layer = m_layerDetails[l].layer;

        fwrite(&layer, sizeof(uint8_t), 1, file);
        fwrite(&m_layerDetails[l].density, sizeof(float), 1, file);

        uint16_t tlen = m_layerDetails[l].model.size();
        fwrite(&tlen, 1, sizeof(uint16_t), file);
        fwrite(m_layerDetails[l].model.c_str(), 1, tlen, file);

        tlen = m_layerDetails[l].material.size();
        fwrite(&tlen, 1, sizeof(uint16_t), file);
        fwrite(m_layerDetails[l].material.c_str(), 1, tlen, file);
    }
}

void EditSurface::write(FILE* file) const
{
    uint16_t size = m_xsize;

    uint16_t id = m_surfaceGraph != nullptr ? m_surfaceGraph->id() : 0;

    fwrite(&size, sizeof(uint16_t), 1, file);
    fwrite(&id, sizeof(uint16_t), 1, file);

    fwrite(m_vertexBuffer.data(), sizeof(TexturedVertex), m_vertexBuffer.size(), file);
    fwrite(m_normals.data(), sizeof(vec3), m_normals.size(), file);

    writeLayers(file);
    writeLayerDetails(file);
}

void EditSurface::writeGameInfo(FILE* file) const
{
    const vec3& pos = m_owner->pos();
    uint16_t size = m_xsize;

    const SurfaceType type = SurfaceType::BezierPatch;

    fwrite(&type, 1, sizeof(SurfaceType), file);
    fwrite(&size, 1, sizeof(uint16_t), file);
    fwrite(&size, 1, sizeof(uint16_t), file);
    fwrite(&pos, 1, sizeof(vec3), file);
    fwrite(m_vertices.data(), sizeof(Vertex), m_vertices.size(), file);

    uint16_t tlen = m_material->name.size();
    fwrite(&tlen, 1, sizeof(uint16_t), file);
    fwrite(m_material->name.c_str(), 1, tlen, file);

    writeLayers(file);
    writeLayerDetails(file);

    fwrite(&size, 1, sizeof(uint16_t), file);
    fwrite(&size, 1, sizeof(uint16_t), file);

    for (int k = 0; k < m_ysize; k++)
    {
        for (int i = 0; i < m_xsize; i++)
        {
            size_t ind = k * m_xsize + i;

            fwrite(&m_vertexBuffer[ind].position, 1, sizeof(vec3), file);
        }
    }
}