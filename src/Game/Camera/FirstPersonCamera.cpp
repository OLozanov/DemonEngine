#include "FirstPersonCamera.h"
#include "Physics/RigidBody.h"

namespace GameLogic
{

FirstPersonCamera::FirstPersonCamera(Render::Camera& camera) 
: CameraController(camera) 
, m_offset(0.0f)
{
}

void FirstPersonCamera::setObject(Physics::RigidBody* object)
{
    m_object = object;
}

void FirstPersonCamera::setViewOffset(const vec3& offset)
{
    m_offset = offset;
}

void FirstPersonCamera::onMouseMove(int x, int y)
{
    m_vrot += y;
    if (m_vrot > 90) m_vrot = 90;
    if (m_vrot < -90) m_vrot = -90;

    m_hrot += x;
    if (m_hrot > 360) m_hrot -= 360;
    if (m_hrot < 0) m_hrot += 360;
}

void FirstPersonCamera::update()
{
    mat3 cammat = mat3::RotateY(m_hrot / 180 * math::pi) * mat3::RotateX(m_vrot / 180 * math::pi);

    m_camera.update(m_object->location() + m_object->orientation() * m_offset, m_object->orientation() * cammat);
}

} // namespace GameLogic