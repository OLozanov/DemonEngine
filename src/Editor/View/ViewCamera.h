#pragma once

#include "math\math3d.h"

class ViewCamera
{
private:
	float m_vrot;
	float m_hrot;

	vec3 m_pos;

	mat4 m_rotMat;
	mat4 m_viewMat;
	mat3 m_basis;

public:
	ViewCamera();
	ViewCamera(float x, float y, float z);

	const vec3& direction() const;

	float verticalAngle() const { return m_vrot; }
	float horizontalAngle() const { return m_hrot; }

	void setAngles(float v, float h)
	{
		m_vrot = v;
		m_hrot = h;
	}

	const mat4& rotMat() const { return m_rotMat; }
	const mat4& viewMat() const { return m_viewMat; }
	const mat3& basis() const { return m_basis; }

	void setOrientation(const mat3& mat) { m_rotMat = mat; }

	const vec3 & pos() const { return m_pos; }
	void setPos(const vec3 & pos) { m_pos = pos; }
	void move(const vec3 & dir) { m_pos += dir; }

	void rotate(float v, float h);

	void update();
};
