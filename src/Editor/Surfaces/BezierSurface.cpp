#include "BezierSurface.h"
#include "Block.h"
#include "Geometry/Geometry.h"

BezierSurface::BezierSurface(Block* block, BlockPolygon* polygon, uint32_t xpower, uint32_t ypower, size_t xsize, size_t ysize, TextureMapping mapping)
: m_controlMesh(xpower + 1, ypower + 1)
, m_bezierPatch(xpower, ypower, xsize, ysize)
, m_xpower(xpower)
, m_ypower(ypower)
{
    m_pos = block->pos();
    m_mapMode = mapping;

    m_material = polygon->material;

    m_s = polygon->s;
    m_t = polygon->t;

    initControlPoints(block, polygon);
    if (m_mapMode == TextureMapping::TCoords) initTCoords(block, polygon);
    
    update();
}

BezierSurface::BezierSurface(const vec3& pos, 
                             uint32_t xpower, uint32_t ypower,
                             size_t xsize, size_t ysize,
                             const std::vector<vec4>& cp,
                             const std::vector<vec2>& tcoord,
                             const vec3& s, const vec3& t, const vec2& uv,
                             uint32_t flags)
: m_controlMesh(xpower + 1, ypower + 1)
, m_bezierPatch(xpower, ypower, xsize, ysize)
, m_xpower(xpower)
, m_ypower(ypower)
{
    m_pos = pos;
    m_flags = flags;
    m_mapMode = TextureMapping::TCoords;

    size_t cpsize = (xpower + 1) * (ypower + 1);

    for (size_t i = 0; i < cpsize; i++) m_bezierPatch.cp(i) = cp[i];

    m_s = s;
    m_t = t;
    m_uv = uv;

    if (!tcoord.empty())
    {
        size_t size = xsize * ysize;
        for (size_t i = 0; i < size; i++) m_bezierPatch.vertex(i).tcoord = tcoord[i];

        m_mapMode = TextureMapping::TCoords;
    }
    else
        m_mapMode = TextureMapping::TSpace;

    update();
}

void BezierSurface::update()
{
    size_t size = (m_xpower + 1) * (m_ypower + 1);

    for (int i = 0; i < size; i++) m_controlMesh.vertex(i) = m_bezierPatch.cp(i).xyz;

    m_bezierPatch.update();

    if (m_mapMode == TextureMapping::TSpace) m_bezierPatch.calcTexCoords(m_s, m_t, m_uv);
}

void BezierSurface::updateTCoord()
{
    m_bezierPatch.calcTexCoords(m_s, m_t, m_uv);
}

uint32_t BezierSurface::getXResolution()
{
    return m_bezierPatch.xsize();
}

uint32_t BezierSurface::getYResolution()
{
    return m_bezierPatch.ysize();
}

void BezierSurface::setResolution(uint32_t x, uint32_t y)
{
    vec2 t1, t2, t3, t4;

    if (m_mapMode == TextureMapping::TCoords)
    {
        size_t x = m_bezierPatch.xsize() - 1;
        size_t y = m_bezierPatch.ysize() - 1;

        t1 = m_bezierPatch.vertex(0, 0).tcoord;
        t2 = m_bezierPatch.vertex(x, 0).tcoord;
        t3 = m_bezierPatch.vertex(x, y).tcoord;
        t4 = m_bezierPatch.vertex(0, y).tcoord;
    }

    m_bezierPatch.setResolution(x, y);
    m_bezierPatch.update();

    if (m_mapMode == TextureMapping::TCoords) m_bezierPatch.calcTexCoords(t1, t2, t3, t4);
    else m_bezierPatch.calcTexCoords(m_s, m_t, m_uv);
}

bool BezierSurface::pick(const vec3& origin, const vec3& ray, vec3& point, float& dist) const
{
    vec3 localOrigin = origin - m_pos;

    if (m_bezierPatch.pick(localOrigin, ray, dist))
    {
        point = origin + ray * dist;
        return true;
    }

    return false;
}

bool BezierSurface::pick2d(float x, float y, float scale, float& depth, int i, int j, int k) const
{
    float localx = x - m_pos[i];
    float localy = y - m_pos[j];

    if (m_bezierPatch.pick2d(localx, localy, scale, depth, i, j, k))
    {
        depth += m_pos[k];
        return true;
    }

    return false;
}

vec3* BezierSurface::pickCp(const vec3& origin, const vec3& ray, float& dist)
{
    vec3 localOrigin = origin - m_pos;

    return m_bezierPatch.pickCp(localOrigin, ray, dist);
}

bool BezierSurface::pickCp2d(float x, float y, float scale, int i, int k, CpList& cpList)
{
    size_t cpnum = (m_xpower + 1) * (m_ypower + 1);

    bool result = false;

    for (int v = 0; v < cpnum; v++)
    {
        vec3 pt = m_bezierPatch.cp(v).xyz + m_pos;

        float dist = vec2(x - pt[i], y - pt[k]).length();

        if (dist < Cp::PickDist / scale)
        {
            Cp* controlPt = new Cp(&m_bezierPatch.cp(v).xyz, this);
            cpList.append(controlPt);

            result = true;
        }
    }

    return result;
}

void BezierSurface::moveTexCoordS(float val)
{
    m_bezierPatch.moveTexCoordS(val);
}

void BezierSurface::moveTexCoordT(float val)
{
    m_bezierPatch.moveTexCoordT(val);
}

void BezierSurface::scaleTexCoordS(float val)
{
    m_bezierPatch.scaleTexCoordS(val);
}

void BezierSurface::scaleTexCoordT(float val)
{
    m_bezierPatch.scaleTexCoordT(val);
}

void BezierSurface::rotateTexCoord(float ang)
{
    m_bezierPatch.rotateTexCoord(ang);
}

void BezierSurface::applyTransform(const mat3& mat)
{
    size_t size = (m_xpower + 1) * (m_ypower + 1);

    for (int i = 0; i < size; i++)
    {
        m_bezierPatch.cp(i).xyz = mat * m_bezierPatch.cp(i).xyz;
        m_controlMesh.vertex(i) = m_bezierPatch.cp(i).xyz;
    }

    m_bezierPatch.update();
}

void BezierSurface::applyScaleInternal()
{
    size_t size = (m_xpower + 1) * (m_ypower + 1);

    for (int i = 0; i < size; i++)
    {
        m_bezierPatch.cp(i).x *= m_scale.x;
        m_bezierPatch.cp(i).y *= m_scale.y;
        m_bezierPatch.cp(i).z *= m_scale.z;
        m_controlMesh.vertex(i) = m_bezierPatch.cp(i).xyz;
    }

    m_bezierPatch.update();
}

void BezierSurface::display(Render::CommandList& commandList) const
{
    commandList.setConstant(1, mat4::Translate(m_pos) * mat4::Rotate(m_rot.x, m_rot.y, m_rot.z) * mat4::Scale(m_scale));   
    commandList.bind(3, m_material->maps[Material::map_diffuse]);
    m_bezierPatch.display(commandList);
}

void BezierSurface::displayControlMesh(Render::CommandList& commandList) const
{
    commandList.setConstant(1, mat4::Translate(m_pos) * mat4::Rotate(m_rot.x, m_rot.y, m_rot.z) * mat4::Scale(m_scale));
    m_controlMesh.display(commandList);
}

void BezierSurface::displayControlPoints(Render::CommandList& commandList) const
{
    commandList.setConstant(1, mat4::Translate(m_pos));
    m_controlMesh.displayVertices(commandList);
}

void BezierSurface::writeEditorInfo(FILE* file) const
{
    uint32_t xpow = m_bezierPatch.xpower();
    uint32_t ypow = m_bezierPatch.ypower();

    uint32_t xres = m_bezierPatch.xresolution();
    uint32_t yres = m_bezierPatch.yresolution();

    fwrite(&m_pos, sizeof(vec3), 1, file);
    fwrite(&xpow, sizeof(uint32_t), 1, file);
    fwrite(&ypow, sizeof(uint32_t), 1, file);
    fwrite(&xres, sizeof(uint32_t), 1, file);
    fwrite(&yres, sizeof(uint32_t), 1, file);
    fwrite(&m_flags, sizeof(uint32_t), 1, file);
    fwrite(&m_mapMode, sizeof(TextureMapping), 1, file);

    int cpnum = (xpow + 1) * (ypow + 1);

    fwrite(&m_bezierPatch.cp(0), sizeof(vec4), cpnum, file);

    fwrite(&m_s, sizeof(vec3), 1, file);
    fwrite(&m_t, sizeof(vec3), 1, file);
    fwrite(&m_uv, sizeof(vec2), 1, file);

    if (m_mapMode == TextureMapping::TCoords)
    {
        int vertnum = xres * yres;

        for (size_t i = 0; i < vertnum; i++)
            fwrite(&m_bezierPatch.vertex(i).tcoord, sizeof(vec2), 1, file);
    }

    //Material
    if (m_material)
    {
        uint16_t tlen = m_material->name.size();
        fwrite(&tlen, sizeof(uint16_t), 1, file);
        fwrite(m_material->name.c_str(), 1, tlen, file);
    }
    else
    {
        uint16_t tlen = 0;
        fwrite(&tlen, sizeof(uint16_t), 1, file);
    }
}

void BezierSurface::writeGameInfo(FILE* file) const
{
    std::vector<Vertex> vertices;

    buildVertices(vertices);

    uint16_t xsize = m_bezierPatch.xsize();
    uint16_t ysize = m_bezierPatch.ysize();

    const SurfaceType type = SurfaceType::BezierPatch;

    fwrite(&type, 1, sizeof(SurfaceType), file);
    fwrite(&xsize, 1, sizeof(uint16_t), file);
    fwrite(&ysize, 1, sizeof(uint16_t), file);
    fwrite(&m_pos, 1, sizeof(vec3), file);
    fwrite(vertices.data(), sizeof(Vertex), vertices.size(), file);

    uint16_t tlen = m_material->name.size();
    fwrite(&tlen, 1, sizeof(uint16_t), file);
    fwrite(m_material->name.c_str(), 1, tlen, file);

    uint8_t layers = 0;
    fwrite(&layers, sizeof(uint8_t), 1, file);
    fwrite(&layers, sizeof(uint8_t), 1, file);

    uint16_t cresx = 0;
    uint16_t cresy = 0;
    std::vector<vec3> clvertices;
    
    if (m_flags && surf_collision) m_bezierPatch.buildCollisionMesh(clvertices, cresx, cresy);

    fwrite(&cresx, 1, sizeof(uint16_t), file);
    fwrite(&cresy, 1, sizeof(uint16_t), file);

    if (m_flags && surf_collision) fwrite(clvertices.data(), sizeof(vec3), clvertices.size(), file);
}

void BezierSurface::initTCoords(Block* block, BlockPolygon* poly)
{
    unsigned long i1 = block->index(poly->offset);
    unsigned long i2 = block->index(poly->offset + 1);
    unsigned long i3 = block->index(poly->offset + 2);
    unsigned long i4 = block->index(poly->offset + 3);

    vec2 t1, t2, t3, t4;

    const vec3& s = poly->s;
    const vec3& t = poly->t;

    t1.x = block->vertex(i1) * s;
    t1.y = block->vertex(i1) * -t;
    t1.x += poly->tcoord.x;
    t1.y += poly->tcoord.y;

    t2.x = block->vertex(i2) * s;
    t2.y = block->vertex(i2) * -t;
    t2.x += poly->tcoord.x;
    t2.y += poly->tcoord.y;

    t3.x = block->vertex(i3) * s;
    t3.y = block->vertex(i3) * -t;
    t3.x += poly->tcoord.x;
    t3.y += poly->tcoord.y;

    t4.x = block->vertex(i4) * s;
    t4.y = block->vertex(i4) * -t;
    t4.x += poly->tcoord.x;
    t4.y += poly->tcoord.y;

    m_bezierPatch.calcTexCoords(t1, t2, t3, t4);
}

void BezierSurface::initControlPoints(Block* block, BlockPolygon* poly)
{
    unsigned long i1 = block->index(poly->offset);
    unsigned long i2 = block->index(poly->offset + 1);
    unsigned long i3 = block->index(poly->offset + 2);
    unsigned long i4 = block->index(poly->offset + 3);

    vec3 a = block->vertex(i2) - block->vertex(i1);
    vec3 b = block->vertex(i4) - block->vertex(i1);
    vec3 c = block->vertex(i3) - block->vertex(i2);

    float fx = 1 / (float)m_xpower;
    float fy = 1 / (float)m_ypower;

    a = a * fy;
    b = b * fy;
    c = c * fy;

    //Tesselate
    vec3 e1 = block->vertex(i1);
    vec3 e2 = block->vertex(i2);

    size_t vptr = 0;

    for (int i = 0; i < m_ypower + 1; i++)
    {
        vec3 edge = e2 - e1;
        edge = edge * fx;

        vec3 vert = e1;

        for (int k = 0; k < m_xpower + 1; k++)
        {
            m_bezierPatch.cp(vptr) = vert;
            vptr++;

            vert += edge;
        }

        e1 = e1 + b;
        e2 = e2 + c;
    }
}

void BezierSurface::buildVertices(std::vector<Vertex>& verices) const
{
    size_t ptr = 0;

    size_t xsize = m_bezierPatch.xsize();
    size_t ysize = m_bezierPatch.ysize();

    verices.resize(xsize * ysize);

    for (int k = 0; k < ysize; k++)
    {
        for (int i = 0; i < xsize; i++)
        {
            verices[ptr].position = m_bezierPatch.vertex(i, k).position;
            verices[ptr].tcoord = m_bezierPatch.vertex(i, k).tcoord;

            int x = (i == xsize - 1) ? xsize - 2 : i;
            int y = (k == ysize - 1) ? ysize - 2 : k;

            int i1 = y * xsize + x;
            int i2 = (y + 1) * xsize  + x;
            int i3 = y * xsize + x + 1;

            const vec3& a = m_bezierPatch.vertex(i1).position;
            const vec3& b = m_bezierPatch.vertex(i2).position;
            const vec3& c = m_bezierPatch.vertex(i3).position;

            vec3 tangent;
            vec3 binormal;

            if (m_flags & surf_accurate_normals)
            {
                tangent = m_bezierPatch.xtangent(i, k);
                binormal = m_bezierPatch.ytangent(i, k);
            }
            else
            {
                tangent = b - a;
                binormal = a - c;
            }

            vec3 normal = tangent ^ binormal;

            //tangent.normalize();
            //binormal.normalize();
            normal.normalize();

            verices[ptr].normal = normal;

            if (m_mapMode == TextureMapping::TCoords)
            {
                const vec2& ta = m_bezierPatch.vertex(i1).tcoord;
                const vec2& tb = m_bezierPatch.vertex(i2).tcoord;
                const vec2& tc = m_bezierPatch.vertex(i3).tcoord;

                vec3 s;
                vec3 t;

                TriangleTangentSpace(a, b, c, ta, tb, tc, s, t);

                //verices[ptr].tangent = s;
                //verices[ptr].binormal = -t;

                vec3 tang2 = normal ^ tangent;
                vec3 binorm2 = normal ^ binormal;
            
                float t1_s = tangent * s;
                float t2_s = tang2 * s;

                float t1_t = binormal * -t;
                float t2_t = binorm2 * -t;

                verices[ptr].tangent = tangent * t1_s + tang2 * t2_s;
                verices[ptr].binormal = binormal * t1_t + binorm2 * t2_t;

                verices[ptr].tangent.normalize();
                verices[ptr].binormal.normalize();
            }
            else
            {
                verices[ptr].tangent = tangent;
                verices[ptr].binormal = binormal;

                vec3 tang2 = normal ^ tangent;

                float t1_s = tangent * m_s;
                float t2_s = tang2 * m_s;

                float t1_t = tangent * -m_t;
                float t2_t = tang2 * -m_t;

                verices[ptr].tangent = tangent * t1_s + tang2 * t2_s;
                verices[ptr].binormal = tangent * t1_t + tang2 * t2_t;

                verices[ptr].tangent.normalize();
                verices[ptr].binormal.normalize();
            }

            ptr++;
        }
    }
}