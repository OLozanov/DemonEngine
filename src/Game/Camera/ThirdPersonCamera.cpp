#include "ThirdPersonCamera.h"
#include "Physics/RigidBody.h"

namespace GameLogic
{

ThirdPersonCamera::ThirdPersonCamera(Render::Camera& camera) 
: CameraController(camera)
, m_dist(3.0f)
, m_tilt(35.0f)
, m_rot(0.0f)
{
}

void ThirdPersonCamera::setObject(Physics::RigidBody* object)
{
    m_object = object;
}

void ThirdPersonCamera::onMouseMove(int x, int y)
{
	m_tilt += y;
	if (m_tilt > 90.0f) m_tilt = 90.0f;
	if (m_tilt < 20.0f) m_tilt = 20.0f;

	m_rot += x;
	if (m_rot > 60.0f) m_rot = 60.0f;
	if (m_rot < -60.0f) m_rot = -60.0f;
}

void ThirdPersonCamera::onScroll(int delta)
{
	m_dist -= delta * 0.01f;

	if (m_dist < 1.5f) m_dist = 1.5f;
	if (m_dist > 5.0f) m_dist = 5.0f;
}

void ThirdPersonCamera::update()
{
	vec3 yaxis = { 0.0f, 1.0f, 0.0f };

	const mat3& objmat = m_object->orientation();
	vec3 xaxis = objmat[0] - yaxis * (yaxis * objmat[0]);
	xaxis.normalize();
	
	vec3 zaxis = xaxis ^ yaxis;

	mat3 cammat = mat3(xaxis, yaxis, zaxis) * mat3::RotateY(m_rot / 180.0f * math::pi) * mat3::RotateX(m_tilt / 180.0f * math::pi);
	vec3 pos = m_object->location() - cammat[2] * m_dist;

	m_camera.update(pos, cammat);
}

} // namespace GameLogic