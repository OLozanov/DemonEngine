#pragma once

#include "StaticGameObject.h"
#include "Render/Surface.h"

namespace GameLogic
{

class SurfaceObject : public StaticGameObject
{
public:
    SurfaceObject(const vec3& pos,
                  const BBox& bbox,
                  Render::Surface* surface,
                  uint32_t xsize,
                  uint32_t ysize,
                  const std::vector<vec3>& verts);

    SurfaceObject(const vec3& pos,
                  const BBox& bbox,
                  Render::Surface* surface,
                  uint32_t res,
                  const std::vector<vec3>& verts);

private:
    std::vector<Collision::CollisionPolygon> m_collisionMesh;
};

} // namespace gamelogic