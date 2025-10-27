#include "BezierTriangleSurface.h"
#include "Geometry/Block.h"
#include "Geometry/Geometry.h"

template<class T>
T barycentric(const T& A,
    const T& B,
    const T& C,
    float a,
    float b,
    float c)
{
    return A * a + B * b + C * c;
}

int fact(int n);

BezierTriangleSurface::BezierTriangleSurface(Block* block, BlockPolygon* polygon, uint32_t power, size_t size, TextureMapping mapping)
: m_controlMesh(power + 1)
, m_triMesh(size)
, m_power(power)
{
    m_pos = block->pos();
    m_mapMode = mapping;

    m_cp.resize((m_power + 1) * (m_power + 2) / 2);

    m_material = polygon->material;

    m_s = polygon->s;
    m_t = polygon->t;

    initControlPoints(block, polygon);
    if (m_mapMode == TextureMapping::TCoords) initTCoords(block, polygon);

    blendPrecalc();

    update();
}

BezierTriangleSurface::BezierTriangleSurface(const vec3& pos,
                                             uint32_t power, size_t size,
                                             const std::vector<vec4>& cp,
                                             const std::vector<vec2>& tcoord,
                                             const vec3& s, const vec3& t, const vec2& uv,
                                             uint32_t flags)
: m_controlMesh(power + 1)
, m_triMesh(size)
, m_power(power)
{
    m_pos = pos;
    m_flags = flags;
    m_mapMode = TextureMapping::TCoords;

    m_cp = cp;

    m_s = s;
    m_t = t;
    m_uv = uv;

    if (!tcoord.empty())
    {
        for (size_t i = 0; i < m_triMesh.size(); i++) m_triMesh.vertex(i).tcoord = tcoord[i];

        m_mapMode = TextureMapping::TCoords;
    }
    else
        m_mapMode = TextureMapping::TSpace;

    blendPrecalc();

    update();
}

void BezierTriangleSurface::blendPrecalc(int calcpow, float u, float v, float w, float* brow) const
{
    m_uPow[0] = 1;
    m_vPow[0] = 1;
    m_wPow[0] = 1;

    for (size_t p = 1; p <= calcpow; p++)
    {
        m_uPow[p] = m_uPow[p - 1] * u;
        m_vPow[p] = m_vPow[p - 1] * v;
        m_wPow[p] = m_wPow[p - 1] * w;
    }

    size_t cp = 0;
    size_t num = calcpow;

    for (size_t y = 0; y <= calcpow; y++, num--)
    {
        int rowi = calcpow - y;
        int rowk = calcpow - y;

        for (size_t x = 0; x <= num; x++, cp++)
        {
            float i = rowi - x;
            float j = y;
            float k = rowk - (num - x);

            int nfact = fact(calcpow);
            int ifact = fact(i);
            int jfact = fact(j);
            int kfact = fact(k);

            int factor = nfact / (ifact * jfact * kfact);

            brow[cp] = factor * m_uPow[i] * m_vPow[j] * m_wPow[k];
        }
    }
}

void BezierTriangleSurface::blendPrecalc(int res, std::vector<float>& blendvec) const
{
    size_t cpnum = (m_power + 1) * (m_power + 2) / 2;
    size_t vertnum = res * (res + 1) / 2;

    blendvec.resize(cpnum * vertnum);

    size_t cp = 0;
    size_t num = res;

    float fx = 1.0 / (res - 1);
    float y = 0;

    float* brow = blendvec.data();

    for (size_t k = 0; k < res; k++, y += fx, num--)
    {
        float rowf = 1.0 / (num - 1);
        float x = 0;

        float rowa = 1.0 - y;
        float rowc = 1.0 - y;

        for (int i = 0; i < num; i++, x += rowf, cp++)
        {
            float a = rowa * x;
            float b = y;
            float c = rowc * (1.0 - x);

            blendPrecalc(m_power, a, b, c, brow);

            brow += cpnum;
        }
    }
}

void BezierTriangleSurface::blendPrecalc()
{
    m_uPow.resize(m_power + 1);
    m_vPow.resize(m_power + 1);
    m_wPow.resize(m_power + 1);

    size_t res = m_triMesh.res();

    size_t cpnum = (m_power + 1) * (m_power + 2) / 2;
    size_t vertnum = res * (res + 1) / 2;

    size_t lowp = m_power - 1;
    size_t lowcpnum = (lowp + 1) * (lowp + 2) / 2;

    m_blend.resize(cpnum * vertnum);
    m_blendLow.resize(lowcpnum * vertnum);

    int cp = 0;
    int num = res;

    float fx = 1.0 / (res - 1);
    float y = 0;

    float* brow = m_blend.data();
    float* browl = m_blendLow.data();

    for (size_t k = 0; k < res; k++, y += fx, num--)
    {
        float rowf = 1.0 / (num - 1);
        float x = 0;

        float rowa = 1.0 - y;
        float rowc = 1.0 - y;

        for (size_t i = 0; i < num; i++, x += rowf, cp++)
        {
            float a = rowa * x;
            float b = y;
            float c = rowc * (1.0 - x);

            blendPrecalc(m_power, a, b, c, brow);
            blendPrecalc(m_power - 1, a, b, c, browl);

            brow += cpnum;
            browl += lowcpnum;
        }
    }
}

void BezierTriangleSurface::calcDerCp(const vec3& dir, std::vector<vec3>& dcp) const
{ 
    size_t dp = 0;
    size_t p = 0;
    size_t num = m_power;

    for (size_t k = 0; k < m_power; k++, num--)
    {
        size_t pnext = p + num + 1;
        for (size_t i = 0; i < num; i++)
        {
            vec3 a = m_cp[p + i].xyz;
            vec3 b = m_cp[pnext + i].xyz;
            vec3 c = m_cp[p + i + 1].xyz;

            dcp[dp++] = barycentric(a, b, c, dir.x, dir.y, dir.z);
        }

        p = pnext;
    }
}

vec3 BezierTriangleSurface::blend(size_t i, const std::vector<float>& blendvec) const
{
    size_t cpnum = (m_power + 1) * (m_power + 2) / 2;

    const float* brow = blendvec.data() + i * cpnum;

    vec3 vert = { 0, 0, 0 };
    float w = 0;

    for (size_t c = 0; c < cpnum; c++)
    {
        vert += vec3(m_cp[c]) * m_cp[c].w * brow[c];
        w += m_cp[c].w * brow[c];
    }

    return { vert.x / w, vert.y / w, vert.z / w };
}

void BezierTriangleSurface::update()
{
    for (size_t i = 0; i < m_cp.size(); i++) m_controlMesh.vertex(i) = m_cp[i].xyz;

    size_t res = m_triMesh.res();
    size_t vertnum = res * (res + 1) / 2;

    for (size_t v = 0; v < vertnum; v++)
    {
        m_triMesh.vertex(v).position = blend(v, m_blend);
    }

    if (m_mapMode == TextureMapping::TSpace) updateTCoord();
}

void BezierTriangleSurface::updateTCoord()
{
    for (int i = 0; i < m_triMesh.size(); i++)
    {
        const vec3& vert = m_triMesh.vertex(i).position;
        m_triMesh.vertex(i).tcoord = vec2(vert * m_s, vert * m_t) + m_uv;
    }
}

uint32_t BezierTriangleSurface::getXResolution()
{
    return m_triMesh.res();
}

uint32_t BezierTriangleSurface::getYResolution()
{
    return 0;
}

void BezierTriangleSurface::setResolution(uint32_t x, uint32_t y)
{
    vec2 a, b, c;

    if (m_mapMode == TextureMapping::TCoords)
    {
        a = m_triMesh.vertex(m_triMesh.res() - 1).tcoord;
        b = m_triMesh.vertex(m_triMesh.size() - 1).tcoord;
        c = m_triMesh.vertex(0).tcoord;
    }

    m_triMesh.resize(x);
    blendPrecalc();

    update();

    if (m_mapMode == TextureMapping::TCoords) calcTexCoords(a, b, c);
}

bool BezierTriangleSurface::pickLine2d(size_t aind, size_t bind,
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

bool BezierTriangleSurface::pick(const vec3& origin, const vec3& ray, vec3& point, float& dist) const
{
    vec3 localOrigin = origin - m_pos;

    if (m_triMesh.pick(localOrigin, ray, dist))
    {
        point = origin + ray * dist;
        return true;
    }

    return false;
}

bool BezierTriangleSurface::pick2d(float x, float y, float scale, float& depth, int l, int m, int n) const
{
    float localx = x - m_pos[l];
    float localy = y - m_pos[m];

    size_t cp = 0;
    int num = m_power;

    for (size_t k = 0; k < m_power; k++, num--)
    {
        size_t cpnext = cp + num + 1;

        for (size_t i = 0; i < num; i++)
        {
            //horizontal lines
            size_t h1 = cp + i;
            size_t h2 = cp + i + 1;

            if (pickLine2d(h1, h2, localx, localy, scale, depth, l, m, n)) return true;

            //left diagonal lines
            size_t l1 = cp + i;
            size_t l2 = cpnext + i;

            if (pickLine2d(l1, l2, localx, localy, scale, depth, l, m, n)) return true;

            //right diagonal lines
            size_t r1 = cp + i + 1;
            size_t r2 = cpnext + i;

            if (pickLine2d(r1, r2, localx, localy, scale, depth, l, m, n)) return true;
        }

        cp = cpnext;
    }

    return false;
}

vec3* BezierTriangleSurface::pickCp(const vec3& origin, const vec3& ray, float& pickDist)
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

bool BezierTriangleSurface::pickCp2d(float x, float y, float scale, int i, int k, CpList& cpList)
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

void BezierTriangleSurface::moveTexCoordS(float val)
{
    m_triMesh.moveTexCoordS(val);
}

void BezierTriangleSurface::moveTexCoordT(float val)
{
    m_triMesh.moveTexCoordT(val);
}

void BezierTriangleSurface::scaleTexCoordS(float val)
{
    m_triMesh.scaleTexCoordS(val);
}

void BezierTriangleSurface::scaleTexCoordT(float val)
{
    m_triMesh.scaleTexCoordT(val);
}

void BezierTriangleSurface::rotateTexCoord(float ang)
{
    m_triMesh.rotateTexCoord(ang);
}

void BezierTriangleSurface::applyTransform(const mat3& mat)
{
    size_t cpnum = (m_power + 1) * (m_power + 2) / 2;

    for (size_t i = 0; i < cpnum; i++)
    {
        m_cp[i].xyz = mat * m_cp[i].xyz;
        m_controlMesh.vertex(i) = m_cp[i].xyz;
    }

    update();
}

void BezierTriangleSurface::applyScaleInternal()
{
    size_t cpnum = (m_power + 1) * (m_power + 2) / 2;

    for (size_t i = 0; i < cpnum; i++)
    {
        m_cp[i].x *= m_scale.x;
        m_cp[i].y *= m_scale.y;
        m_cp[i].z *= m_scale.z;
        m_controlMesh.vertex(i) = m_cp[i].xyz;
    }

    update();
}

void BezierTriangleSurface::display(Render::CommandList& commandList) const
{
    commandList.setConstant(1, mat4::Translate(m_pos) * mat4::Rotate(m_rot.x, m_rot.y, m_rot.z) * mat4::Scale(m_scale));
    commandList.bind(3, m_material->maps[Material::map_diffuse]);
    m_triMesh.display(commandList);
}

void BezierTriangleSurface::displayControlMesh(Render::CommandList& commandList) const
{
    commandList.setConstant(1, mat4::Translate(m_pos) * mat4::Rotate(m_rot.x, m_rot.y, m_rot.z) * mat4::Scale(m_scale));
    m_controlMesh.display(commandList);
}

void BezierTriangleSurface::displayControlPoints(Render::CommandList& commandList) const
{
    commandList.setConstant(1, mat4::Translate(m_pos));
    m_controlMesh.displayVertices(commandList);
}

void BezierTriangleSurface::writeEditorInfo(FILE* file) const
{
    uint32_t res = m_triMesh.res();

    fwrite(&m_pos, sizeof(vec3), 1, file);
    fwrite(&m_power, sizeof(uint32_t), 1, file);
    fwrite(&res, sizeof(uint32_t), 1, file);
    fwrite(&m_flags, sizeof(uint32_t), 1, file);
    fwrite(&m_mapMode, sizeof(TextureMapping), 1, file);

    fwrite(m_cp.data(), sizeof(vec4), m_cp.size(), file);

    fwrite(&m_s, sizeof(vec3), 1, file);
    fwrite(&m_t, sizeof(vec3), 1, file);
    fwrite(&m_uv, sizeof(vec2), 1, file);

    if (m_mapMode == TextureMapping::TCoords)
    {
        for (size_t i = 0; i < m_triMesh.size(); i++)
            fwrite(&m_triMesh.vertex(i).tcoord, sizeof(vec2), 1, file);
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

void BezierTriangleSurface::writeGameInfo(FILE* file) const
{
    std::vector<Vertex> vertices;

    buildVertices(vertices);

    uint16_t res = m_triMesh.res();

    const SurfaceType type = SurfaceType::BezierTriangle;

    fwrite(&type, 1, sizeof(SurfaceType), file);
    fwrite(&res, 1, sizeof(uint16_t), file);
    fwrite(&m_pos, 1, sizeof(vec3), file);
    fwrite(vertices.data(), sizeof(Vertex), vertices.size(), file);

    uint16_t tlen = m_material->name.size();
    fwrite(&tlen, 1, sizeof(uint16_t), file);
    fwrite(m_material->name.c_str(), 1, tlen, file);

    //Currently not supported for triangles
    //uint8_t layers = 0;
    //fwrite(&layers, sizeof(uint8_t), 1, file);

    uint16_t cres = 0;
    std::vector<vec3> clvertices;

    if (m_flags && surf_collision) buildCollisionMesh(clvertices, cres);

    fwrite(&cres, 1, sizeof(uint16_t), file);

    if (m_flags && surf_collision) fwrite(clvertices.data(), sizeof(vec3), clvertices.size(), file);
}

void BezierTriangleSurface::calcTexCoords(const vec2& A, const vec2& B, const vec2& C)
{
    size_t res = m_triMesh.res();

    size_t tc = 0;
    size_t num = res;

    float fx = 1.0 / (res - 1);
    float y = 0;

    for (size_t k = 0; k < res; k++, y += fx, num--)
    {
        float rowf = 1.0 / (num - 1);
        float x = 0;

        float rowa = 1.0 - y;
        float rowc = 1.0 - y;

        for (size_t i = 0; i < num; i++, x += rowf, tc++)
        {
            float a = rowa * x;
            float b = y;
            float c = rowc * (1.0 - x);

            m_triMesh.vertex(tc).tcoord = barycentric(A, B, C, a, b, c);
        }
    }
}

void BezierTriangleSurface::initTCoords(Block* block, BlockPolygon* poly)
{
    uint16_t i1 = block->index(poly->offset);
    uint16_t i2 = block->index(poly->offset + 1);
    uint16_t i3 = block->index(poly->offset + 2);

    const vec3& t1 = block->vertex(i1);
    const vec3& t2 = block->vertex(i2);
    const vec3& t3 = block->vertex(i3);

    vec2 A = vec2(t1 * poly->s, t1 * -poly->t) + poly->tcoord;
    vec2 B = vec2(t2 * poly->s, t2 * -poly->t) + poly->tcoord;
    vec2 C = vec2(t3 * poly->s, t3 * -poly->t) + poly->tcoord;

    calcTexCoords(A, B, C);
}

void BezierTriangleSurface::initControlPoints(Block* block, BlockPolygon* poly)
{
    unsigned long i1 = block->index(poly->offset);
    unsigned long i2 = block->index(poly->offset + 1);
    unsigned long i3 = block->index(poly->offset + 2);

    vec3 A = block->vertex(i1);
    vec3 B = block->vertex(i2);
    vec3 C = block->vertex(i3);

    size_t cp = 0;
    size_t num = m_power;

    float fx = 1.0 / m_power;
    float y = 0;

    for (size_t k = 0; k <= m_power; k++, y += fx, num--)
    {
        float rowf = 1.0 / num;
        float x = 0;

        float rowa = 1.0 - y;
        float rowc = 1.0 - y;

        for (size_t i = 0; i <= num; i++, x += rowf, cp++)
        {
            float a = rowa * x;
            float b = y;
            float c = rowc * (1.0 - x);

            m_cp[cp] = barycentric(A, B, C, a, b, c);
        }
    }
}

void BezierTriangleSurface::buildCollisionMesh(std::vector<vec3>& cverts, uint16_t& cres) const
{
    std::vector<float> cblend;

    size_t res = m_triMesh.res();
    cres = res <= 5 ? 3 : res / 4;

    int vertnum = cres * (cres + 1) / 2;

    cverts.resize(vertnum);

    blendPrecalc(cres, cblend);

    for (size_t v = 0; v < vertnum; v++)
    {
        cverts[v] = blend(v, cblend);
    }
}

void BezierTriangleSurface::buildVertices(std::vector<Vertex>& verices) const
{
    vec3 ud = { -1, 0, 1 };
    vec3 vd = { -0.5, 1, -0.5 };

    verices.resize(m_triMesh.size());

    size_t dpow = m_power - 1;
    size_t cpnum = (dpow + 1) * (dpow + 2) / 2;

    std::vector<vec3> dercpU(cpnum);
    std::vector<vec3> dercpV(cpnum);

    calcDerCp(ud, dercpU);
    calcDerCp(vd, dercpV);

    for (size_t v = 0; v < m_triMesh.size(); v++)
    {
        verices[v].position = m_triMesh.vertex(v).position;
        verices[v].tcoord = m_triMesh.vertex(v).tcoord;

        const float* brow = m_blendLow.data() + v * cpnum;

        vec3 tang = { 0, 0, 0 };
        vec3 binorm = { 0, 0, 0 };

        for (int cp = 0; cp < cpnum; cp++)
        {
            tang += dercpU[cp] * brow[cp];
            binorm += dercpV[cp] * brow[cp];
        }

        tang.normalize();
        binorm.normalize();

        verices[v].normal = tang ^ binorm;
        verices[v].normal.normalize();

        if (m_mapMode == TextureMapping::TSpace)
        {
            vec3 tang2 = verices[v].normal ^ tang;

            float t1_s = tang * m_s;
            float t2_s = tang2 * m_s;

            float t1_t = tang * -m_t;
            float t2_t = tang2 * -m_t;

            verices[v].tangent = tang * t1_s + tang2 * t2_s;
            verices[v].binormal = tang * t1_t + tang2 * t2_t;

            verices[v].tangent.normalize();
            verices[v].binormal.normalize();
        }
        else
        {
            verices[v].tangent = -tang;
            verices[v].binormal = binorm;
        }
    }
}