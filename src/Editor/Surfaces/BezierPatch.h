#pragma once

#include "SurfaceMesh.h"

// Rational Bezier Patch
class BezierPatch : public SurfaceMesh
{
public:
    BezierPatch(int powx, int powy, int resx, int resy);

    uint32_t xpower() const { return m_powx; }
    uint32_t ypower() const { return m_powy; }

    const vec4& cp(size_t i) const { return m_cp[i]; }
    vec4& cp(size_t i) { return m_cp[i]; }

    const vec4& cp(size_t i, size_t k) const { return m_cp[k * (m_powx + 1) + i]; }
    vec4& cp(size_t i, size_t k) { return m_cp[k * (m_powx + 1) + i]; }

    void setResolution(uint32_t resx, uint32_t resy);

    uint32_t xresolution() const { return m_xsize; }
    uint32_t yresolution() const { return m_ysize; }

    void calcTexCoords(const vec2& t1, const vec2& t2, const vec2& t3, const vec2& t4);
    void calcTexCoords(const vec3& s, const vec3& t, const vec2& uv);

    vec3 xtangent(int u, int v) const;
    vec3 ytangent(int u, int v) const;

    void buildCollisionMesh(std::vector<vec3>& cverts, uint16_t& cresX, uint16_t& cresY) const;

    bool pick2d(float x, float y, float scale, float& depth, int i, int j, int k) const;
    vec3* pickCp(const vec3& origin, const vec3& ray, float& dist);

    void update();

private:

    bool pickLine2d(size_t aind, size_t bind,
                    float x, float y, 
                    float scale,
                    float& depth,
                    int i, int j, int k) const;

    void blendPrecalc(int pow, int res, std::vector<float>& blend) const;
    void blendDerivativePrecalc(int pow, int res, std::vector<float>& blend) const;
    void blendPrecalc();
    vec3 blend(int u, int v, const std::vector<float>& blendX, const std::vector<float>& blendY) const;

private:

    uint32_t m_powx;
    uint32_t m_powy;

    std::vector<vec4> m_cp; //control points

    //Precalculated blending function values
    std::vector<float> m_blendX;
    std::vector<float> m_blendY;

    //Derivatives of blending function (for calculating tangents)
    std::vector<float> m_dblendX;
    std::vector<float> m_dblendY;

    static constexpr float PickDist2d = 0.015;
};