#pragma once

class mat4
{
	vec4 axis[4];

public:

	constexpr mat4() : axis{{1, 0, 0, 0},
							{0, 1, 0, 0},
							{0, 0, 1, 0},
							{0, 0, 0, 1}} {}

	constexpr mat4(const vec4 & x, const vec4 & y, const vec4 & z, const vec4 & w) : axis{x, y, z, w} {}

	constexpr mat4(const mat3& mat) : axis{ {mat[0], 0},
											{mat[1], 0},
											{mat[2], 0},
										    {0, 0, 0, 1}} {}

	constexpr mat4(const mat3& mat, const vec3& offset) : axis{ {mat[0], 0},
															    {mat[1], 0},
															    {mat[2], 0},
															    {offset, 1} } {}

	vec4 & operator[](int i)
	{
		return axis[i];
	}

	const vec4 & operator[](int i) const
	{
		return axis[i];
	}

	vec4 operator *(const vec4 & vect) const
	{
		vec4 X = axis[0]*vect.x;
		vec4 Y = axis[1]*vect.y;
		vec4 Z = axis[2]*vect.z;
		vec4 W = axis[3]*vect.w;

		return X + Y + Z + W;
	}

	mat4 operator *(const mat4 & mat) const
	{
		vec4 xAxis = (*this)*mat[0];
		vec4 yAxis = (*this)*mat[1];
		vec4 zAxis = (*this)*mat[2];
		vec4 wAxis = (*this)*mat[3];

		return mat4(xAxis, yAxis, zAxis, wAxis);
	}

	mat4 Rotation() const
	{
		return mat4(axis[0], axis[1], axis[2], vec4());
	}

	operator const float*() const
	{
		return &axis[0][0];
	}

	float minor(int i, int k) const;

	operator mat3() const
	{
		return mat3({axis[0].x, axis[0].y, axis[0].z},
					{axis[1].x, axis[1].y, axis[1].z},
					{axis[2].x, axis[2].y, axis[2].z});
	}

	float det() const
	{
		return axis[0][0] * minor(0, 0) - axis[1][0] * minor(1, 0) + axis[2][0] * minor(2, 0) - axis[3][0] * minor(3, 0);
	}

	mat4 transpose() const
	{
		return mat4({axis[0].x, axis[1].x, axis[2].x, axis[3].x},
					{axis[0].y, axis[1].y, axis[2].y, axis[3].y},
					{axis[0].z, axis[1].z, axis[2].z, axis[3].z},
					{axis[0].w, axis[1].w, axis[2].w, axis[3].w});
	}

	mat4 inverse() const
	{
		float rdet = 1.0f / det();

		return mat4({ minor(0,0) * rdet, -minor(1,0) * rdet, minor(2,0) * rdet, -minor(3,0) * rdet },
					{ -minor(0,1) * rdet, minor(1,1) * rdet, -minor(2,1) * rdet, minor(3,1) * rdet },
					{ minor(0,2) * rdet, -minor(1,2) * rdet, minor(2,2) * rdet, -minor(3,2) * rdet },
					{ -minor(0,3) * rdet, minor(1,3) * rdet, -minor(2,3) * rdet, minor(3,3) * rdet } );
	}

	//Helper functions
	static mat4 Translate(const vec3 & pos);
	static mat4 RotateX(float rad);
	static mat4 RotateY(float rad);
	static mat4 RotateZ(float rad);
	static mat4 Rotate(float x, float y, float z);
	static mat4 Rotate(const vec3& axis, float ang);
	static mat4 Scale(const vec3 & pos);
	static mat4 Scale(float sz);
	static mat4 Reflection(const vec4& pl);
	static mat4 Projection(float fov, float aspect, float near, float far);
	static mat4 OrthoProjection(float l, float r, float b, float t, float n, float f);
};

inline vec4 operator *(const vec4 & vect, const mat4 & mat)
{
	float x = vect*mat[0];
	float y = vect*mat[1];
	float z = vect*mat[2];
	float w = vect*mat[3];

	return vec4(x, y, z, w);
}