#pragma once

#include "Game/Camera/CameraController.h"

namespace Physics
{
    class RigidBody;
}

namespace GameLogic
{

class ThirdPersonCamera : public CameraController
{
public:
    explicit ThirdPersonCamera(Render::Camera& camera, float dist = 3.0f, float mindist = 2.5f, float maxdist = 5.0f);

    void setObject(Physics::RigidBody* object);
    void setTilt(float tilt) { m_tilt = tilt; }

    void onMouseMove(int x, int y) override;
    void onScroll(int delta) override;
    void update(float dt) override;

private:
    Physics::RigidBody* m_object;
    
    float m_mindist;
    float m_maxdist;

    float m_dist;
    float m_tilt;
    float m_rot;

    float m_viewdist;
};

} // namespace GameLogic