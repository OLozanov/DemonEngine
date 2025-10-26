#include "Camera.h"

namespace Render
{

Camera::Camera()
{
}

Camera::Camera(float x, float y, float z)
: m_pos(x, y, z)
{
}

const vec3& Camera::direction() const
{
	return m_basis[2];
}

void Camera::update(const vec3& pos, const mat3& mat)
{
	m_pos = pos;
	m_basis = mat;

	m_rotMat = m_basis.transpose();
	m_viewMat = m_rotMat * mat4::Translate({ -m_pos.x, -m_pos.y, -m_pos.z });
}

} //namespace Render
