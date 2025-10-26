#include "ViewCamera.h"

ViewCamera::ViewCamera()
: m_vrot(0)
, m_hrot(0)
{
}

ViewCamera::ViewCamera(float x, float y, float z)
: m_pos(x, y, z)
, m_vrot(0)
, m_hrot(0)
{
}

const vec3& ViewCamera::direction() const
{
	return m_basis[2];
}

void ViewCamera::rotate(float v, float h)
{
	m_vrot += v;
	if(m_vrot > 90) m_vrot = 90;
	if(m_vrot < -90) m_vrot = -90;

	m_hrot += h;
	if(m_hrot > 360) m_hrot -= 360;
	if(m_hrot < 0) m_hrot += 360;
}

void ViewCamera::update()
{
	m_rotMat = mat4::RotateX(-m_vrot/180*math::pi)*mat4::RotateY(-m_hrot/180*math::pi);
	m_viewMat = m_rotMat*mat4::Translate({-m_pos.x, -m_pos.y, -m_pos.z});
	
	m_basis = m_rotMat.transpose();
}
