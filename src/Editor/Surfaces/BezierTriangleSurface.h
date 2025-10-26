#pragma once

#include "Surface.h"
#include "ControlMesh.h"
#include "TriangularMesh.h"

#include <vector>

class Block;
class BlockPolygon;

class BezierTriangleSurface : public Surface
{
public:
    BezierTriangleSurface(Block* block, BlockPolygon* polygon, uint32_t power, size_t size, TextureMapping mapping);
    BezierTriangleSurface(const vec3& pos,
                          uint32_t power, size_t size,
                          const std::vector<vec4>& cp,
                          const std::vector<vec2>& tcoord,
                          const vec3& s, const vec3& t, const vec2& uv,
                          uint32_t flags);

    SurfaceType type() const override { return SurfaceType::BezierTriangle; }
    Surface* clone() const override { return new BezierTriangleSurface(*this); }

    void update() override;
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

    void calcTexCoords(const vec2& a, const vec2& b, const vec2& c);
    void initTCoords(Block* block, BlockPolygon* poly);
    void initControlPoints(Block* block, BlockPolygon* poly);

    bool pickLine2d(size_t aind, size_t bind,
                    float x, float y,
                    float scale,
                    float& depth,
                    int i, int j, int k) const;

    void blendPrecalc(int calcpow, float u, float v, float w, float* brow) const;
    void blendPrecalc(int res, std::vector<float>& blendvec) const;
    void blendPrecalc();

    void calcDerCp(const vec3& dir, std::vector<vec3>& dcp) const;

    vec3 blend(size_t i, const std::vector<float>& blendvec) const;

    void buildCollisionMesh(std::vector<vec3>& cverts, uint16_t& cres) const;

    void buildVertices(std::vector<Vertex>& verices) const;

private:
    uint32_t m_power;

    std::vector<vec4> m_cp;

    ControlMesh m_controlMesh;
    TriangularMesh m_triMesh;

    //Precalculated blending function values
    std::vector<float> m_blend;
    std::vector<float> m_blendLow;  //blending function for n-1 order patch

    //Temporary storage
    mutable std::vector<float> m_uPow;
    mutable std::vector<float> m_vPow;
    mutable std::vector<float> m_wPow;

    static constexpr float PickDist2d = 0.015;
};