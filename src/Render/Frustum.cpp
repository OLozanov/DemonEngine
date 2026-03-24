#include "Frustum.h"

namespace Render
{

void Frustum::update(const vec3& pos, const vec4& plane, const std::vector<vec3>& portal)
{
	float d = plane.xyz * pos + plane.w;

	m_planes.resize(portal.size() + 1);
	m_planes[0] = d < 0 ? plane : -plane;

	size_t vnum = portal.size();

	for (size_t v = 0; v < vnum; v++)
	{
		int i1, i2;

		i1 = v;
		i2 = ((v + 1) == vnum) ? 0 : v + 1;

		vec3 edge1 = portal[i1] - pos;
		vec3 edge2 = portal[i2] - pos;

		vec3 norm = d < 0 ? edge1 ^ edge2 : edge2 ^ edge1;
		norm.normalize();

		m_planes[v + 1].xyz = norm;
		m_planes[v + 1].w = -pos * norm;
	}
}

void Frustum::update(const vec3& pos, const vec3& dir, const vec3* verts, size_t num)
{
	m_planes.resize(num + 1);

	float d = dir * pos;

	m_planes[0] = { dir, -d };

	for (size_t v = 0; v < num; v++)
	{
		int i1, i2;

		i1 = v;
		i2 = ((v + 1) == num) ? 0 : v + 1;

		vec3 edge1 = verts[i1] - pos;
		vec3 edge2 = verts[i2] - pos;

		vec3 norm = edge1 ^ edge2;
		norm.normalize();
		m_planes[v + 1].xyz = norm;
		m_planes[v + 1].w = -pos * norm;
	}
}

/*void Frustum::update(const Camera& camera, float fovx, float fovy)
{
	vec3 verts[4];

	static const vec2 quadVertices[] =
	{
		{ -1.0f, 1.0f },
		{ 1.0f, 1.0f },
		{ 1.0f, -1.0f },
		{ -1.0f, -1.0f }
	};

	const vec3& pos = camera.pos();
	const vec3& dir = camera.direction();
	float d = dir * pos;

	for (int i = 0; i < 4; i++)
	{
		verts[i] = camera.basis() * vec3(quadVertices[i].x * fovx, quadVertices[i].y * fovy, 1.0) + camera.pos();
	}

	update(pos, dir, verts, 4);
}*/

void Frustum::update(const mat4& mat)
{
	m_planes.resize(6);

	// near
	m_planes[0].x = mat[0].w + mat[0].z;
	m_planes[0].y = mat[1].w + mat[1].z;
	m_planes[0].z = mat[2].w + mat[2].z;
	m_planes[0].w = mat[3].w + mat[3].z;

	// far
	m_planes[1].x = mat[0].w - mat[0].z;
	m_planes[1].y = mat[1].w - mat[1].z;
	m_planes[1].z = mat[2].w - mat[2].z;
	m_planes[1].w = mat[3].w - mat[3].z;

	// left
	m_planes[2].x = mat[0].w + mat[0].x;
	m_planes[2].y = mat[1].w + mat[1].x;
	m_planes[2].z = mat[2].w + mat[2].x;
	m_planes[2].w = mat[3].w + mat[3].x;

	// right
	m_planes[3].x = mat[0].w - mat[0].x;
	m_planes[3].y = mat[1].w - mat[1].x;
	m_planes[3].z = mat[2].w - mat[2].x;
	m_planes[3].w = mat[3].w - mat[3].x;

	// top
	m_planes[4].x = mat[0].w - mat[0].y;
	m_planes[4].y = mat[1].w - mat[1].y;
	m_planes[4].z = mat[2].w - mat[2].y;
	m_planes[4].w = mat[3].w - mat[3].y;
	
	// bottom
	m_planes[5].x = mat[0].w + mat[0].y;
	m_planes[5].y = mat[1].w + mat[1].y;
	m_planes[5].z = mat[2].w + mat[2].y;
	m_planes[5].w = mat[3].w + mat[3].y;
}

bool Frustum::test(const vec3& pos, const vec3& bbox, const mat3& axis) const
{
	for (size_t i = 0; i < m_planes.size(); i++)
	{
		const vec4& plane = m_planes[i];

		float r = fabs(axis[0] * plane.xyz) * bbox.x + fabs(axis[1] * plane.xyz) * bbox.y + fabs(axis[2] * plane.xyz) * bbox.z;
		float dist = plane.xyz * pos + plane.w;

		if (dist < -r) return false;
	}

	return true;
}

bool Frustum::test(const vec3& pos, const vec3& bbox) const
{
    for (size_t i = 0; i < m_planes.size(); i++)
    {
		float r = fabs(m_planes[i].x) * bbox.x + fabs(m_planes[i].y) * bbox.y + fabs(m_planes[i].z) * bbox.z;
		float dist = m_planes[i].xyz * pos + m_planes[i].w;
    
        if (dist < -r) return false;
    }

    return true;
}

bool Frustum::test(const BBox& bbox) const
{
	vec3 pos = (bbox.min + bbox.max) * 0.5f;
	vec3 extent = bbox.max - pos;

	return test(pos, extent);
}

bool Frustum::test(const BBox& bbox, const mat4& mat) const
{
	const mat3 axis = mat;

	vec3 mid = (bbox.min + bbox.max) * 0.5f;
	vec3 pos = mat[3].xyz + axis * mid;
	vec3 extent = bbox.max - mid;

	return test(pos, extent, axis);
}

} // namespace Render