#pragma once

#include "Game/Camera/CameraController.h"

namespace Physics
{
    class RigidBody;
}

namespace GameLogic
{

class FirstPersonCamera : public CameraController
{
public:
    explicit FirstPersonCamera(Render::Camera& camera);

    void setObject(Physics::RigidBody* object);
    void setViewOffset(const vec3& offset);

    float verticalAngle() const { return m_vrot; }
    float horizontalAngle() const { return m_hrot; }

    void setAngles(float v, float h)
    {
        m_vrot = v;
        m_hrot = h;
    }

    void onMouseMove(int x, int y) override;
    void onScroll(int delta) override {} // ignore
    void update() override;

private:
    Physics::RigidBody* m_object;

    float m_vrot;
    float m_hrot;

    vec3 m_offset;
};

} // namespace GameLogic