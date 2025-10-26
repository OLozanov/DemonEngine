#pragma once

#include "Render/Camera.h"

namespace GameLogic
{

class CameraController
{
public:
    explicit CameraController(Render::Camera& camera) : m_camera(camera) {}
    virtual ~CameraController() {}

    const vec3& direction() const { return m_camera.direction(); }

    virtual void onMouseMove(int x, int y) = 0;
    virtual void onScroll(int delta) = 0;
    virtual void update() = 0;

protected:
    Render::Camera& m_camera;
};

} // namespace GameLogic