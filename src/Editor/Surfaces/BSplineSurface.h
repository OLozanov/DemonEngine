#pragma once

#include "Geometry/Block.h"
#include "Surface.h"
#include "ControlMesh.h"
#include "BezierPatch.h"

#include <vector>

class BSplineSurface : public Surface
{
public:
    BSplineSurface(Block* block, BlockPolygon* polygon, 
                   uint32_t xpower, uint32_t ypower, 
                   size_t xsize, size_t ysize,
                   size_t xres, size_t yres,
                   TextureMapping mapping);

    BSplineSurface(const vec3& pos,
                   uint32_t xpower, uint32_t ypower,
                   size_t xsize, size_t ysize,
                   size_t xres, size_t yres,
                   const std::vector<vec4>& cp,
                   const mat3& texMat,
                   const vec3& s, const vec3& t, const vec2& uv,
                   uint32_t flags,
                   TextureMapping mapping);

    SurfaceType type() const override { return SurfaceType::BSpline; }
    Surface* clone() const override { return new BSplineSurface(*this); }

    void update() override;
    void reconfig() override;

    void updateTCoord() override;

    uint32_t getXResolution() override;
    uint32_t getYResolution() override;

    void setResolution(uint32_t x, uint32_t y) override;

    bool pick(const vec3& origin, const vec3& ray, vec3& point, float& dist) const override;
    bool pick2d(float x, float y, float scale, float& depth, int i, int j, int k) const override;

    vec3* pickCp(const vec3& origin, const vec3& ray, float& dist) override;
    bool pickCp2d(float x, float y, float scale, int i, int k, CpList& cpList) override;

    void display(Render::CommandList& commandList) const override;
    void displayControlMesh(Render::CommandList& commandList) const override;
    void displayControlPoints(Render::CommandList& commandList) const override;

    void writeEditorInfo(FILE* file) const override;
    void writeGameInfo(FILE* file) const override;

private:
    void moveTexCoordS(float val) override;
    void moveTexCoordT(float val) override;
    void scaleTexCoordS(float val) override;
    void scaleTexCoordT(float val) override;
    void rotateTexCoord(float ang) override;

    void applyTransform(const mat3& mat) override;
    void applyScaleInternal() override;

    void initControlPoints(Block* block, BlockPolygon* poly);

    bool pickLine2d(size_t aind, size_t bind,
                    float x, float y,
                    float scale,
                    float& depth,
                    int i, int j, int k) const;

    static vec4 AffineCombination(float u, float a, float b, const vec4& aval, const vec4& bval);
    static vec4 CalcBlossom(const float* indexes, const float* knots, const vec4* cp, int startind, int order);

    void setupKnotVectors();

    void transformPatch(int x, int y);
    void setupTCoords();

    void buildVertices(const BezierPatch& patch, std::vector<Vertex>& verices) const;

private:
    uint32_t m_xpower;
    uint32_t m_ypower;
    uint32_t m_xsize;
    uint32_t m_ysize;
    uint32_t m_xres;
    uint32_t m_yres;

    uint32_t m_xpatches;
    uint32_t m_ypatches;

    //knot vectors
    std::vector<float> m_xknots;
    std::vector<float> m_yknots;

    bool m_xendpoint;
    bool m_yendpoint;
    bool m_cyclicx;
    bool m_cyclicy;

    mat3 m_texMat;

    std::vector<vec4> m_cp;

    ControlMesh m_controlMesh;
    std::vector<BezierPatch> m_patches;

    static constexpr float PickDist2d = 0.015;
};