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
    explicit ThirdPersonCamera(Render::Camera& camera);

    void setObject(Physics::RigidBody* object);
    void setTilt(float tilt) { m_tilt = tilt; }

    void onMouseMove(int x, int y) override;
    void onScroll(int delta) override;
    void update() override;

private:
    Physics::RigidBody* m_object;
    
    float m_dist;
    float m_tilt;
    float m_rot;
};

} // namespace GameLogic