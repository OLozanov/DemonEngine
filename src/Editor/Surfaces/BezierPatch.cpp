#include "BezierPatch.h"

int fact(int n)
{
    if (n == 0) return 1;

    int ret = 1;

    while (n != 1)
    {
        ret *= n;
        n--;
    }

    return ret;
}

int binominal(int n, int k)
{
    int nf = fact(n);
    int kf = fact(k);
    int nkf = fact(n - k);

    return nf / (kf * nkf);
}

BezierPatch::BezierPatch(int powx, int powy, int resx, int resy)
: SurfaceMesh(resx, resy)
, m_powx(powx)
, m_powy(powy)
{
    m_cp.resize((powx + 1) * (powy + 1));

    blendPrecalc();
}

void BezierPatch::blendPrecalc(int pow, int res, std::vector<float>& blend) const
{
    std::vector<float> uPow(pow + 1);
    std::vector<float> revuPow(pow + 1);

    blend.resize(res * (pow + 1));

    float dif = 1.0 / (res - 1);
    float u = 0;

    for (int i = 0; i < res; i++, u += dif)
    {
        float revu = 1.0 - u;

        uPow[0] = 1;
        revuPow[0] = 1;

        for (int p = 1; p <= pow; p++)
        {
            uPow[p] = uPow[p - 1] * u;
            revuPow[p] = revuPow[p - 1] * revu;
        }

        int ind = i * (pow + 1);
        float* brow = blend.data() + ind;

        for (int k = 0; k <= pow; k++)
        {
            int b = binominal(pow, k);
            brow[k] = b * uPow[k] * revuPow[pow - k];
        }
    }
}

void BezierPatch::blendDerivativePrecalc(int pow, int res, std::vector<float>& blend) const
{
    std::vector<float> uPow(pow + 1);
    std::vector<float> revuPow(pow + 1);

    blend.resize(res * (pow + 1));

    float dif = 1.0 / (res - 1);
    float u = 0;

    for (int i = 0; i < res; i++, u += dif)
    {
        float revu = 1.0 - u;

        uPow[0] = 1;
        revuPow[0] = 1;

        for (int p = 1; p <= pow; p++)
        {
            uPow[p] = uPow[p - 1] * u;
            revuPow[p] = revuPow[p - 1] * revu;
        }

        int ind = i * (pow + 1);
        float* brow = blend.data() + ind;

        for (int k = 0; k <= pow; k++)
        {
            int revk = pow - k;

            float a = 0;
            float b = 0;

            int binom = binominal(pow, k);

            if (k != 0) a = binom * k * uPow[k - 1] * revuPow[revk];
            if (revk != 0) b = binom * revk * uPow[k] * revuPow[revk - 1];

            brow[k] = a - b;
        }
    }
}

void BezierPatch::blendPrecalc()
{
    blendPrecalc(m_powx, SurfaceMesh::m_xsize, m_blendX);
    blendPrecalc(m_powy, SurfaceMesh::m_ysize, m_blendY);

    blendDerivativePrecalc(m_powx, SurfaceMesh::m_xsize, m_dblendX);
    blendDerivativePrecalc(m_powy, SurfaceMesh::m_ysize, m_dblendY);
}

vec3 BezierPatch::blend(int u, int v, const std::vector<float>& blendX, const std::vector<float>& blendY) const
{
    vec3 pt(0, 0, 0);
    float w = 0;

    const float* browX = blendX.data() + u * (m_powx + 1);
    const float* browY = blendY.data() + v * (m_powy + 1);

    for (int i = 0; i <= m_powx; i++)
    {
        for (int k = 0; k <= m_powy; k++)
        {
            const vec4& cpoint = m_cp[k * (m_powx + 1) + i];
            pt += vec3(cpoint.x, cpoint.y, cpoint.z) * cpoint.w * browX[i] * browY[k];
            w += cpoint.w * browX[i] * browY[k];
        }
    }

    return vec3(pt.x / w, pt.y / w, pt.z / w);
}

vec3 BezierPatch::xtangent(int u, int v) const
{
    vec3 pt = {};
    vec3 dpt = {};

    float w = 0;
    float dw = 0;

    const float* browX = m_blendX.data() + u * (m_powx + 1);
    const float* derBrowX = m_dblendX.data() + u * (m_powx + 1);
    const float* browY = m_blendY.data() + v * (m_powy + 1);

    for (int i = 0; i <= m_powx; i++)
    {
        for (int k = 0; k <= m_powy; k++)
        {
            const vec4& cpoint = m_cp[k * (m_powx + 1) + i];

            pt += vec3(cpoint.x, cpoint.y, cpoint.z) * cpoint.w * browX[i] * browY[k];
            dpt += vec3(cpoint.x, cpoint.y, cpoint.z) * cpoint.w * derBrowX[i] * browY[k];

            w += cpoint.w * browX[i] * browY[k];
            dw += cpoint.w * derBrowX[i] * browY[k];
        }
    }

    return (dpt * w - pt * dw) * (1.0 / (w * w));
}

vec3 BezierPatch::ytangent(int u, int v) const
{
    vec3 pt = {};
    vec3 dpt = {};

    float w = 0;
    float dw = 0;

    const float* browX = m_blendX.data() + u * (m_powx + 1);
    const float* browY = m_blendY.data() + v * (m_powy + 1);
    const float* derBrowY = m_dblendY.data() + v * (m_powy + 1);

    for (int i = 0; i <= m_powx; i++)
    {
        for (int k = 0; k <= m_powy; k++)
        {
            const vec4& cpoint = m_cp[k * (m_powx + 1) + i];

            pt += vec3(cpoint.x, cpoint.y, cpoint.z) * cpoint.w * browX[i] * browY[k];
            dpt += vec3(cpoint.x, cpoint.y, cpoint.z) * cpoint.w * browX[i] * derBrowY[k];

            w += cpoint.w * browX[i] * browY[k];
            dw += cpoint.w * browX[i] * derBrowY[k];
        }
    }

    return (dpt * w - pt * dw) * (1.0 / (w * w));
}

void BezierPatch::buildCollisionMesh(std::vector<vec3>& cverts, uint16_t& cresX, uint16_t& cresY) const
{
    cresX = m_xsize <= 5 ? 3 : m_xsize / 4;
    cresY = m_ysize <= 5 ? 3 : m_ysize / 4;

    cverts.resize(cresX * cresY);

    std::vector<float> cblendX;
    std::vector<float> cblendY;

    blendPrecalc(m_powx, cresX, cblendX);
    blendPrecalc(m_powy, cresY, cblendY);

    vec3* pt = cverts.data();

    for (int k = 0; k < cresY; k++)
    {
        for (int i = 0; i < cresX; i++)
        {
            *pt = blend(i, k, cblendX, cblendY);
            pt++;
        }
    }
}

void BezierPatch::setResolution(uint32_t resx, uint32_t resy)
{
    resize(resx, resy);

    blendPrecalc();
}

void BezierPatch::calcTexCoords(const vec2& t1, const vec2& t2, const vec2& t3, const vec2& t4)
{
    float fx = 1.0 / (m_xsize - 1);
    float fy = 1.0 / (m_ysize - 1);

    vec2 ta, tb, tc;

    ta.x = (t2.x - t1.x) * fy;
    ta.y = (t2.y - t1.y) * fy;

    tb.x = (t4.x - t1.x) * fy;
    tb.y = (t4.y - t1.y) * fy;

    tc.x = (t3.x - t2.x) * fy;
    tc.y = (t3.y - t2.y) * fy;

    vec2 te1, te2;

    te1 = t1;
    te2 = t2;

    size_t vptr = 0;

    for (int i = 0; i < m_ysize; i++)
    {
        vec2 tedge = (te2 - te1) * fx;
        vec2 tvert = te1;

        for (int k = 0; k < m_xsize; k++)
        {
            m_vertexBuffer[vptr].tcoord = tvert;
            vptr++;

            tvert += tedge;
        }

        te1 += tb;
        te2 += tc;
    }
}

void BezierPatch::calcTexCoords(const vec3& s, const vec3& t, const vec2& uv)
{
    for (int i = 0; i < m_vertexBuffer.size(); i++)
    {
        const vec3& vert = m_vertexBuffer[i].position;
        m_vertexBuffer[i].tcoord = vec2(vert * s, vert * t) + uv;
    }
}

bool BezierPatch::pickLine2d(size_t aind, size_t bind, 
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

bool BezierPatch::pick2d(float x, float y, float scale, float& depth, int l, int m, int k) const
{
    for (int k = 0; k <= m_powy; k++)
    {
        for (int i = 0; i < m_powx; i++)
        {
            int hind = k * (m_powx + 1) + i;

            if (pickLine2d(hind, hind + 1, x, y, scale, depth, l, m, k)) return true;
        }
    }

    for (int k = 0; k < m_powy; k++)
    {
        for (int i = 0; i <= m_powx; i++)
        {
            int vind = k * (m_powx + 1) + i;

            if (pickLine2d(vind, vind + m_powx + 1, x, y, scale, depth, l, m, k)) return true;
        }
    }

    return false;
}

vec3* BezierPatch::pickCp(const vec3& origin, const vec3& ray, float& pickDist)
{
    vec3* pickPt = nullptr;

    for (int i = 0; i < m_cp.size(); i++)
    {
        vec3 vert = m_cp[i];
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

void BezierPatch::update()
{
    for (int k = 0; k < m_ysize; k++)
    {
        for (int i = 0; i < m_xsize; i++)
        {
            size_t ind = k * m_xsize + i;
            m_vertexBuffer[ind].position = blend(i, k, m_blendX, m_blendY);
        }
    }

    updateBBox();
}