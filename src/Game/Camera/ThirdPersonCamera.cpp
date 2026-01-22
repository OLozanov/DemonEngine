#include "ThirdPersonCamera.h"
#include "Physics/PhysicsManager.h"
#include "Game/Constants.h"

#include <algorithm>

namespace GameLogic
{

ThirdPersonCamera::ThirdPersonCamera(Render::Camera& camera, float dist, float mindist, float maxdist)
: CameraController(camera)
, m_mindist(mindist)
, m_maxdist(maxdist)
, m_dist(dist)
, m_viewdist(dist)
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
	m_dist = std::clamp(m_dist, m_mindist, m_maxdist);
}

void ThirdPersonCamera::update(float dt)
{
	vec3 yaxis = { 0.0f, 1.0f, 0.0f };

	const mat3& objmat = m_object->orientation();
	vec3 xaxis = objmat[0] - yaxis * (yaxis * objmat[0]);
	xaxis.normalize();
	
	vec3 zaxis = xaxis ^ yaxis;

	mat3 cammat = mat3(xaxis, yaxis, zaxis) * mat3::RotateY(m_rot / 180.0f * math::pi) * mat3::RotateX(m_tilt / 180.0f * math::pi);
	
	const vec3& objpos = m_object->location();

	bool adjustDist = true;

	Physics::PhysicsManager& physics = Physics::PhysicsManager::GetInstance();

	Collision::TraceRayInfo tinfo;
	if (physics.traceRay(objpos, -cammat[2], collision_raycast, tinfo, m_dist))
	{
		float ncos = cammat[2] * tinfo.norm;

		if (ncos > math::eps)
		{
			m_viewdist = std::max(0.0f, tinfo.dist - 0.5f / ncos);
			adjustDist = false;
		}
	}

	if (adjustDist)
	{
		if (fabs(m_viewdist - m_dist) > math::eps)
		{
			float speed = dt * 3.0f;

			if (m_viewdist < m_dist)
				m_viewdist = std::min(m_dist, m_viewdist + speed);
			else
				m_viewdist = std::max(m_dist, m_viewdist - speed);
		}
	}
	
	vec3 pos = objpos - cammat[2] * m_viewdist;

	m_camera.update(pos, cammat);
}

} // namespace GameLogic