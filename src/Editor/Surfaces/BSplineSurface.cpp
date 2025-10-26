#include "BSplineSurface.h"

BSplineSurface::BSplineSurface(Block* block, BlockPolygon* polygon,
                               uint32_t xpower, uint32_t ypower,
                               size_t xsize, size_t ysize,
                               size_t xres, size_t yres,
                               TextureMapping mapping)
: m_controlMesh(xsize + 1, ysize + 1)
, m_xpower(xpower)
, m_ypower(ypower)
, m_xsize(xsize)
, m_ysize(ysize)
, m_xres(xres)
, m_yres(yres)
, m_xendpoint(false)
, m_yendpoint(false)
, m_cyclicx(false)
, m_cyclicy(false)
{
    m_pos = block->pos();
    m_mapMode = mapping;

    m_material = polygon->material;

    m_cp.resize((xsize + 1) * (ysize + 1));

    m_s = polygon->s;
    m_t = polygon->t;

    initControlPoints(block, polygon);

    int xintervals = m_xsize + m_xpower * 2;
    int yintervals = m_ysize + m_ypower * 2;

    m_xknots.resize(xintervals);
    m_yknots.resize(yintervals);

    setupKnotVectors();

    m_xpatches = m_xsize - m_xpower + 1;
    m_ypatches = m_ysize - m_ypower + 1;

    m_patches.reserve((m_xpatches + m_xpower) * (m_ypatches + m_ypower));

    for (int k = 0; k < m_ypatches + m_ypower; k++)
    {
        for (int i = 0; i < m_xpatches + m_xpower; i++)
        {
            m_patches.emplace_back(m_xpower, m_ypower, m_xres, m_yres);
            transformPatch(i, k);
        }
    }

    setupTCoords();

    update();
}

BSplineSurface::BSplineSurface(const vec3& pos,
                               uint32_t xpower, uint32_t ypower,
                               size_t xsize, size_t ysize,
                               size_t xres, size_t yres,
                               const std::vector<vec4>& cp,
                               const mat3& texMat,
                               const vec3& s, const vec3& t, const vec2& uv,
                               uint32_t flags,
                               TextureMapping mapping)
: m_controlMesh(xsize + 1, ysize + 1)
, m_xpower(xpower)
, m_ypower(ypower)
, m_xsize(xsize)
, m_ysize(ysize)
, m_xres(xres)
, m_yres(yres)
, m_xendpoint(false)
, m_yendpoint(false)
, m_cyclicx(false)
, m_cyclicy(false)
, m_texMat(texMat)
{
    m_pos = pos;
    m_flags = flags;
    m_mapMode = mapping;

    size_t cpsize = (xpower + 1) * (ypower + 1);

    m_cp = cp;

    m_s = s;
    m_t = t;
    m_uv = uv;

    int xintervals = m_xsize + m_xpower * 2;
    int yintervals = m_ysize + m_ypower * 2;

    m_xknots.resize(xintervals);
    m_yknots.resize(yintervals);

    setupKnotVectors();

    m_xpatches = m_xsize - m_xpower + 1;
    m_ypatches = m_ysize - m_ypower + 1;

    m_patches.reserve((m_xpatches + m_xpower) * (m_ypatches + m_ypower));

    for (int k = 0; k < m_ypatches + m_ypower; k++)
    {
        for (int i = 0; i < m_xpatches + m_xpower; i++)
        {
            m_patches.emplace_back(m_xpower, m_ypower, m_xres, m_yres);
            transformPatch(i, k);
        }
    }

    setupTCoords();

    update();
}

void BSplineSurface::update()
{
    size_t size = (m_xsize + 1) * (m_ysize + 1);

    for (int i = 0; i < size; i++) m_controlMesh.vertex(i) = m_cp[i].xyz;

    int xpatches = (m_flags & surf_cyclic_x) ? m_xpatches + m_xpower : m_xpatches;
    int ypatches = (m_flags & surf_cyclic_y) ? m_ypatches + m_ypower : m_ypatches;

    for (int k = 0; k < ypatches; k++)
    {
        for (int i = 0; i < xpatches; i++)
        {
            transformPatch(i, k);
        }
    }

    if (m_mapMode == TextureMapping::TSpace)
        for (BezierPatch& patch : m_patches) patch.calcTexCoords(m_s, m_t, m_uv);
}

void BSplineSurface::reconfig()
{
    bool endx = (m_flags & surf_endpoint_x) != 0 && (m_flags & surf_cyclic_x) == 0;
    bool endy = (m_flags & surf_endpoint_y) != 0 && (m_flags & surf_cyclic_y) == 0;

    if (m_xendpoint != endx || m_yendpoint != endy)
    {
        setupKnotVectors();
        setupTCoords();

        m_xendpoint = endx;
        m_yendpoint = endy;

        update();
    }

    bool cyclicx = (m_flags & surf_cyclic_x);
    bool cyclicy = (m_flags & surf_cyclic_y);

    if (m_cyclicx != cyclicx || m_cyclicy != cyclicy)
    {
        setupTCoords();

        m_cyclicx = cyclicx;
        m_cyclicy = cyclicy;

        update();
    }
}

void BSplineSurface::updateTCoord()
{
    for (BezierPatch& patch : m_patches) patch.calcTexCoords(m_s, m_t, m_uv);
}

uint32_t BSplineSurface::getXResolution()
{
    return m_xres;
}

uint32_t BSplineSurface::getYResolution()
{
    return m_yres;
}

void BSplineSurface::setResolution(uint32_t x, uint32_t y)
{
    if (m_xres == x && m_yres == y) return;

    m_xres = x;
    m_yres = y;

    for (BezierPatch& patch : m_patches)
    {
        patch.setResolution(x, y);
        patch.update();

        setupTCoords();

        if (m_mapMode == TextureMapping::TSpace) patch.calcTexCoords(m_s, m_t, m_uv);
    }
}

bool BSplineSurface::pickLine2d(size_t aind, size_t bind,
                                float x, float y,
                                float scale,
                                float& depth,
                                int i, int j, int k) const
{
    vec2 a = { m_cp[aind][i], m_cp[aind][j] };
    vec2 b = { m_cp[bind][i], m_cp[bind][j] };

    float adepth = m_cp[aind][k];
    float bdepth = m_cp[bind][k];

    vec2 edge = b - a;
    vec2 dir = vec2(x, y) - a;

    float dist;

    if (edge.length() < math::eps)
    {
        dist = dir.length();
        depth = adepth > bdepth ? adepth : bdepth;
    }
    else
    {
        float len = edge.normalize();
        float proj = edge * dir;

        if (proj < 0 || proj > len) return false;

        dist = edge ^ dir;

        float factor = proj / len;
        depth = adepth * (1.0 - factor) + bdepth * factor;
    }

    if (fabs(dist) < PickDist2d / scale) return true;

    return false;
}

bool BSplineSurface::pick(const vec3& origin, const vec3& ray, vec3& point, float& dist) const
{
    vec3 localOrigin = origin - m_pos;

    int xpatches = (m_flags & surf_cyclic_x) ? m_xpatches + m_xpower : m_xpatches;
    int ypatches = (m_flags & surf_cyclic_y) ? m_ypatches + m_ypower : m_ypatches;

    for (int k = 0; k < ypatches; k++)
    {
        for (int i = 0; i < xpatches; i++)
        {
            const BezierPatch& patch = m_patches[k * (m_xpatches + m_xpower) + i];
            if (patch.pick(localOrigin, ray, dist))
            {
                point = origin + ray * dist;
                return true;
            }
        }
    }

    return false;
}

bool BSplineSurface::pick2d(float x, float y, float scale, float& depth, int l, int m, int n) const
{
    float localx = x - m_pos[l];
    float localy = y - m_pos[m];

    for (int k = 0; k <= m_ysize; k++)
    {
        for (int i = 0; i < m_xsize; i++)
        {
            int hind = k * (m_xsize + 1) + i;

            if (pickLine2d(hind, hind + 1, localx, localy, scale, depth, l, m, n)) return true;
        }
    }

    for (int k = 0; k < m_ysize; k++)
    {
        for (int i = 0; i <= m_xsize; i++)
        {
            int vind = k * (m_xsize + 1) + i;

            if (pickLine2d(vind, vind + m_xsize + 1, localx, localy, scale, depth, l, m, n)) return true;
        }
    }

    return false;
}

vec3* BSplineSurface::pickCp(const vec3& origin, const vec3& ray, float& pickDist)
{
    vec3* pickPt = nullptr;

    for (size_t i = 0; i < m_cp.size(); i++)
    {
        vec3 vert = m_cp[i] + m_pos;
        vec3 pt = vert - origin;
        float raydist = pt * ray;

        if (raydist < 0) continue;

        vec3 projPt = origin + ray * raydist;

        float dist = (projPt - vert).length();

        if (dist < 0.01 * raydist)
        {
            if (!pickPt || raydist < pickDist)
            {
                pickPt = &m_cp[i].xyz;
                pickDist = raydist;
            }
        }
    }

    return pickPt;
}

bool BSplineSurface::pickCp2d(float x, float y, float scale, int i, int k, CpList& cpList)
{
    bool result = false;

    for (size_t v = 0; v < m_cp.size(); v++)
    {
        vec3 pt = m_cp[v].xyz + m_pos;

        float dist = vec2(x - pt[i], y - pt[k]).length();

        if (dist < Cp::PickDist / scale)
        {
            Cp* controlPt = new Cp(&m_cp[v].xyz, this);
            cpList.append(controlPt);

            result = true;
        }
    }

    return result;
}

void BSplineSurface::moveTexCoordS(float val)
{
    m_texMat[2][0] += val;

    for (BezierPatch& patch : m_patches) patch.moveTexCoordS(val);
}

void BSplineSurface::moveTexCoordT(float val)
{
    m_texMat[2][1] += val;

    for (BezierPatch& patch : m_patches) patch.moveTexCoordT(val);
}

void BSplineSurface::scaleTexCoordS(float val)
{
    m_texMat[0][0] *= val;
    m_texMat[1][0] *= val;

    for (BezierPatch& patch : m_patches) patch.scaleTexCoordS(val);
}

void BSplineSurface::scaleTexCoordT(float val)
{
    m_texMat[0][1] *= val;
    m_texMat[1][1] *= val;

    for (BezierPatch& patch : m_patches) patch.scaleTexCoordT(val);
}

void BSplineSurface::rotateTexCoord(float ang)
{
    float Cos = cosf(ang / 180.0f * math::pi);
    float Sin = sinf(ang / 180.0f * math::pi);

    if (fabs(Cos) < math::eps) Cos = 0.0f;
    if (fabs(Sin) < math::eps) Sin = 0.0f;

    mat3 rotMat = { {Cos, Sin, 0},
                    {-Sin, Cos, 0},
                    { 0, 0, 1} };

    m_texMat = m_texMat * rotMat;

    for (BezierPatch& patch : m_patches) patch.rotateTexCoord(ang);
}


void BSplineSurface::applyTransform(const mat3& mat)
{
    size_t size = (m_xsize + 1) * (m_ysize + 1);

    for (int i = 0; i < size; i++)
    {
        m_cp[i].xyz = mat * m_cp[i].xyz;
        m_controlMesh.vertex(i) = m_cp[i].xyz;
    }

    update();
}

void BSplineSurface::applyScaleInternal()
{
    size_t size = (m_xsize + 1) * (m_ysize + 1);

    for (int i = 0; i < size; i++)
    {
        m_cp[i].x *= m_scale.x;
        m_cp[i].y *= m_scale.y;
        m_cp[i].z *= m_scale.z;
        m_controlMesh.vertex(i) = m_cp[i].xyz;
    }

    update();
}

void BSplineSurface::display(Render::CommandList& commandList) const
{
    commandList.bind(3, m_material->maps[Material::map_diffuse]);
    commandList.setConstant(1, mat4::Translate(m_pos) * mat4::Rotate(m_rot.x, m_rot.y, m_rot.z) * mat4::Scale(m_scale));

    int xpatches = (m_flags & surf_cyclic_x) ? m_xpatches + m_xpower : m_xpatches;
    int ypatches = (m_flags & surf_cyclic_y) ? m_ypatches + m_ypower : m_ypatches;

    for (int k = 0; k < ypatches; k++)
    {
        for (int i = 0; i < xpatches; i++)
        {
            const BezierPatch& patch = m_patches[k * (m_xpatches + m_xpower) + i];
            patch.display(commandList);
        }
    }
}

void BSplineSurface::displayControlMesh(Render::CommandList& commandList) const
{
    commandList.setConstant(1, mat4::Translate(m_pos) * mat4::Rotate(m_rot.x, m_rot.y, m_rot.z) * mat4::Scale(m_scale));
    m_controlMesh.display(commandList);
}

void BSplineSurface::displayControlPoints(Render::CommandList& commandList) const
{
    commandList.setConstant(1, mat4::Translate(m_pos));
    m_controlMesh.displayVertices(commandList);
}

void BSplineSurface::writeEditorInfo(FILE* file) const
{
    fwrite(&m_pos, 1, sizeof(vec3), file);
    fwrite(&m_xpower, 1, sizeof(uint32_t), file);
    fwrite(&m_ypower, 1, sizeof(uint32_t), file);
    fwrite(&m_xres, 1, sizeof(uint32_t), file);
    fwrite(&m_yres, 1, sizeof(uint32_t), file);
    fwrite(&m_xsize, 1, sizeof(uint32_t), file);
    fwrite(&m_ysize, 1, sizeof(uint32_t), file);
    fwrite(&m_flags, 1, sizeof(uint32_t), file);
    fwrite(&m_mapMode, 1, sizeof(TextureMapping), file);

    fwrite(&m_texMat, 1, sizeof(mat3), file);

    fwrite(&m_s, 1, sizeof(vec3), file);
    fwrite(&m_t, 1, sizeof(vec3), file);
    fwrite(&m_uv, 1, sizeof(vec2), file);

    int cpnum = (m_xsize + 1) * (m_ysize + 1);

    fwrite(m_cp.data(), cpnum, sizeof(vec4), file);

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

void BSplineSurface::buildVertices(const BezierPatch& patch, std::vector<Vertex>& verices) const
{
    size_t ptr = 0;

    verices.resize(m_xres * m_yres);

    for (int k = 0; k < m_yres; k++)
    {
        for (int i = 0; i < m_xres; i++)
        {
            verices[ptr].position = patch.vertex(i, k).position;
            verices[ptr].tcoord = patch.vertex(i, k).tcoord;

            int x = (i == m_xres - 1) ? m_xres - 2 : i;
            int y = (k == m_yres - 1) ? m_yres - 2 : k;

            int i1 = y * m_xres + x;
            int i2 = (y + 1) * m_xres + x;
            int i3 = y * m_xres + x + 1;

            const vec3& a = patch.vertex(i1).position;
            const vec3& b = patch.vertex(i2).position;
            const vec3& c = patch.vertex(i3).position;

            vec3 tangent;
            vec3 binormal;

            if (m_flags & surf_accurate_normals)
            {
                tangent = patch.xtangent(i, k);
                binormal = patch.ytangent(i, k);
            }
            else
            {
                tangent = b - a;
                binormal = a - c;
            }

            vec3 normal = tangent ^ binormal;

            tangent.normalize();
            binormal.normalize();
            normal.normalize();

            verices[ptr].normal = normal;

            if (m_mapMode == TextureMapping::TCoords)
            {
                const vec2& ta = patch.vertex(i1).tcoord;
                const vec2& tb = patch.vertex(i2).tcoord;
                const vec2& tc = patch.vertex(i3).tcoord;

                vec3 s;
                vec3 t;

                TriangleTangentSpace(a, b, c, ta, tb, tc, s, t);

                verices[ptr].tangent = s;
                verices[ptr].binormal = -t;
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

void BSplineSurface::writeGameInfo(FILE* file) const
{
    int xpatches = (m_flags & surf_cyclic_x) ? m_xpatches + m_xpower : m_xpatches;
    int ypatches = (m_flags & surf_cyclic_y) ? m_ypatches + m_ypower : m_ypatches;

    uint16_t xsize = m_xres;
    uint16_t ysize = m_yres;

    for (int k = 0; k < ypatches; k++)
    {
        for (int i = 0; i < xpatches; i++)
        {
            const BezierPatch& patch = m_patches[k * (m_xpatches + m_xpower) + i];

            std::vector<Vertex> vertices;

            buildVertices(patch, vertices);

            uint16_t xsize = patch.xsize();
            uint16_t ysize = patch.ysize();

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

            if (m_flags && surf_collision) patch.buildCollisionMesh(clvertices, cresx, cresy);

            fwrite(&cresx, 1, sizeof(uint16_t), file);
            fwrite(&cresy, 1, sizeof(uint16_t), file);

            if (m_flags && surf_collision) fwrite(clvertices.data(), sizeof(vec3), clvertices.size(), file);
        }
    }
}

void BSplineSurface::initControlPoints(Block* block, BlockPolygon* poly)
{
    unsigned long i1 = block->index(poly->offset);
    unsigned long i2 = block->index(poly->offset + 1);
    unsigned long i3 = block->index(poly->offset + 2);
    unsigned long i4 = block->index(poly->offset + 3);

    vec3 a = block->vertex(i2) - block->vertex(i1);
    vec3 b = block->vertex(i4) - block->vertex(i1);
    vec3 c = block->vertex(i3) - block->vertex(i2);

    float fx = 1 / (float)m_xsize;
    float fy = 1 / (float)m_ysize;

    a = a * fy;
    b = b * fy;
    c = c * fy;

    //Tesselate
    vec3 e1 = block->vertex(i1);
    vec3 e2 = block->vertex(i2);

    size_t vptr = 0;

    for (int i = 0; i < m_ysize + 1; i++)
    {
        vec3 edge = e2 - e1;
        edge = edge * fx;

        vec3 vert = e1;

        for (int k = 0; k < m_xsize + 1; k++)
        {
            m_cp[vptr] = { vert, 1.0 };
            vptr++;

            vert += edge;
        }

        e1 = e1 + b;
        e2 = e2 + c;
    }
}

void BSplineSurface::setupKnotVectors()
{
    size_t xintervals = m_xknots.size();
    size_t yintervals = m_yknots.size();

    int lastx = m_xsize + m_xpower;
    int lasty = m_ysize + m_ypower;

    bool endx = (m_flags & surf_endpoint_x) != 0 && (m_flags & surf_cyclic_x) == 0;
    bool endy = (m_flags & surf_endpoint_y) != 0 && (m_flags & surf_cyclic_y) == 0;

    for (int i = 0, index = 1; i < xintervals; i++)
    {
        m_xknots[i] = index;
        if (!endx || (i >= m_xpower - 1 && i < (lastx - m_xpower))) index++;
    }

    for (int i = 0, index = 1; i < yintervals; i++)
    {
        m_yknots[i] = index;
        if (!endy || (i >= m_ypower - 1 && i < (lasty - m_ypower))) index++;
    }
}

vec4 BSplineSurface::AffineCombination(float u, float a, float b, const vec4& aval, const vec4& bval)
{
    return (aval * (b - u) + bval * (u - a)) * (1.0 / (b - a));
}

/* Blossom interpolation scheme for B-Spline
*  Example for Cubic case:
*  1 2 3
*  2 3 4		u1 2 3
*  3 4 5		u1 3 4		u1 u2 3
*  4 5 6		u1 4 5		u1 u2 4		u1, u2, u3
*/
vec4 BSplineSurface::CalcBlossom(const float* indexes, const float* knots, const vec4* cp, int startind, int order)
{
    if (order == 1)
    {
        int a = startind;
        int b = a + 1;

        return AffineCombination(indexes[0], knots[a], knots[b], cp[0], cp[1]);
    }

    std::vector<vec4> points(order);

    for (int i = 0; i < order; i++)
    {
        int a = startind + i;
        int b = a + order;

        points[i] = AffineCombination(indexes[0], knots[a], knots[b], cp[i], cp[i + 1]);
    }

    return CalcBlossom(indexes + 1, knots, points.data(), startind + 1, order - 1);
}

/* Bezier control points scheme
*  Example for Cubic case:
*  B-Spline interval [3,4] (first possible interval start equals to order),
*  control polygon --> 333 334 344 444
*/
void BSplineSurface::transformPatch(int x, int y)
{
    BezierPatch& patch = m_patches[y * (m_xpatches + m_xpower) + x];

    const float* xknots = m_xknots.data() + x;
    const float* yknots = m_yknots.data() + y;

    //x, y intervals
    float xstart = xknots[m_xpower - 1];
    float xend = xknots[m_xpower];

    float ystart = yknots[m_ypower - 1];
    float yend = yknots[m_ypower];

    for (int k = 0; k <= m_ypower; k++)
    {
        std::vector<vec4> cp(m_xpower + 1);
        std::vector<float> indexes(m_xpower, xstart);

        for (int i = 0; i <= m_xpower; i++)
        {
            int xoffset = x + i;
            int yoffset = y + k;

            if (xoffset > m_xsize) xoffset -= m_xsize + 1;
            if (yoffset > m_ysize) yoffset -= m_ysize + 1;

            vec4 pt = m_cp[(m_xsize + 1) * yoffset + xoffset];
            cp[i] = { pt.x * pt.w, pt.y * pt.w, pt.z * pt.w, pt.w };
        }

        for (int i = 0; i <= m_xpower; i++)
        {
            vec4 pt = CalcBlossom(indexes.data(), xknots, cp.data(), 0, m_xpower);
            patch.cp(i, k) = { pt.x / pt.w, pt.y / pt.w, pt.z / pt.w, pt.w };

            if (i != m_xpower) indexes[m_xpower - 1 - i] = xend;
        }
    }

    for (int i = 0; i <= m_xpower; i++)
    {
        std::vector<vec4> cp(m_ypower + 1);
        std::vector<float> indexes(m_ypower, ystart);

        for (int k = 0; k <= m_ypower; k++)
        {
            vec4 pt = patch.cp(i, k);
            cp[k] = { pt.x * pt.w, pt.y * pt.w, pt.z * pt.w, pt.w };
        }

        for (int k = 0; k <= m_ypower; k++)
        {
            vec4 pt = CalcBlossom(indexes.data(), yknots, cp.data(), 0, m_ypower);
            patch.cp(i, k) = { pt.x / pt.w, pt.y / pt.w, pt.z / pt.w, pt.w };

            if (k != m_ypower) indexes[m_ypower - 1 - k] = yend;
        }
    }

    patch.update();
}

void BSplineSurface::setupTCoords()
{
    int xpatches = (m_flags & surf_cyclic_x) ? m_xpatches + m_xpower : m_xpatches;
    int ypatches = (m_flags & surf_cyclic_y) ? m_ypatches + m_ypower : m_ypatches;

    std::vector<float> ucoord(xpatches + 1);
    std::vector<float> vcoord(ypatches + 1);

    for (int p = 0; p < xpatches; p++)
    {
        const float* xknots = m_xknots.data() + p;

        float xstart = xknots[m_xpower - 1];

        std::vector<vec4> cp(m_xpower + 1);
        std::vector<float> indexes(m_xpower, xstart);

        for (int i = 0; i <= m_xpower; i++)
        {
            cp[i] = { float(p + i), 0, 0, 1 };
        }

        vec4 coord = CalcBlossom(indexes.data(), xknots, cp.data(), 0, m_xpower);
        ucoord[p] = coord.x;

        if (p == xpatches - 1)
        {
            float xend = xknots[m_xpower];
            std::vector<float> indexes(m_xpower, xend);

            vec4 coord = CalcBlossom(indexes.data(), xknots, cp.data(), 0, m_xpower);
            ucoord[p + 1] = coord.x;
        }
    }

    for (int p = 0; p < ypatches; p++)
    {
        const float* yknots = m_yknots.data() + p;

        float ystart = yknots[m_ypower - 1];

        std::vector<vec4> cp(m_ypower + 1);
        std::vector<float> indexes(m_ypower, ystart);

        for (int i = 0; i <= m_ypower; i++)
        {
            cp[i] = { float(p + i), 0, 0, 1 };
        }

        vec4 coord = CalcBlossom(indexes.data(), yknots, cp.data(), 0, m_ypower);
        vcoord[p] = coord.x;

        if (p == ypatches - 1)
        {
            float yend = yknots[m_ypower];
            std::vector<float> indexes(m_ypower, yend);

            vec4 coord = CalcBlossom(indexes.data(), yknots, cp.data(), 0, m_ypower);
            vcoord[p + 1] = coord.x;
        }
    }

    for (int k = 0; k < ypatches; k++)
    {
        for (int i = 0; i < xpatches; i++)
        {
            BezierPatch& patch = m_patches[k * (m_xpatches + m_xpower) + i];

            float xstart = ucoord[i];
            float xend = ucoord[i + 1];

            float ystart = vcoord[k];
            float yend = vcoord[k + 1];

            vec3 t1 = m_texMat * vec3(ystart, xstart, 1);
            vec3 t2 = m_texMat * vec3(ystart, xend, 1);
            vec3 t3 = m_texMat * vec3(yend, xend, 1);
            vec3 t4 = m_texMat * vec3(yend, xstart, 1);

            patch.calcTexCoords(t1, t2, t3, t4);
        }
    }
}