#pragma once

#include "Surface.h"
#include "ControlMesh.h"
#include "BezierPatch.h"

#include <vector>

class Block;
class BlockPolygon;

class BezierSurface : public Surface
{
public:
    BezierSurface(Block* block, BlockPolygon* polygon, uint32_t xpower, uint32_t ypower, size_t xsize, size_t ysize, TextureMapping mapping);
    BezierSurface(const vec3& pos,
                  uint32_t xpower, uint32_t ypower, 
                  size_t xsize, size_t ysize,
                  const std::vector<vec4>& cp, 
                  const std::vector<vec2>& tcoord,
                  const vec3& s, const vec3& t, const vec2& uv,
                  uint32_t flags);

    SurfaceType type() const override { return SurfaceType::BezierPatch; }
    Surface* clone() const override { return new BezierSurface(*this); }

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

    void initTCoords(Block* block, BlockPolygon* poly);
    void initControlPoints(Block* block, BlockPolygon* poly);

    void buildVertices(std::vector<Vertex>& verices) const;

private:
    uint32_t m_xpower;
    uint32_t m_ypower;

    ControlMesh m_controlMesh;
    BezierPatch m_bezierPatch;
};