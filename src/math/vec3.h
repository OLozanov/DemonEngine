#pragma once

#include "Math.h"
#include "constants.h"

struct vec3
{
	float x;
	float y;
	float z;

	//

	constexpr vec3() : x(0), y(0), z(0) {}

	constexpr vec3(float fx, float fy, float fz) : x(fx), y(fy), z(fz) {}

	explicit constexpr vec3(float num) : x(num), y(num), z(num) {}

	const float & operator[](int i) const
	{
		return (&x)[i];
	}

	float & operator[](int i)
	{
		return (&x)[i];
	}

	vec3 operator+(const vec3 & vect) const
	{
		return vec3( x + vect.x, y + vect.y, z + vect.z );
	}

	const vec3 & operator+=(const vec3 & vect)
	{
		x += vect.x;
		y += vect.y;
		z += vect.z;

		return *this;
	}

	vec3 operator-(const vec3 & vect) const
	{
		return vec3( x - vect.x, y - vect.y, z - vect.z );
	}

	const vec3 & operator-=(const vec3 & vect)
	{
		x -= vect.x;
		y -= vect.y;
		z -= vect.z;

		return *this;
	}

	vec3 operator-() const
	{
		return vec3(-x, -y, -z );
	}

	float operator*(const vec3 & vect) const 		//Dot product
	{
		return x*vect.x + y*vect.y + z*vect.z;
	}

	vec3 operator^(const vec3 & vect) const	//Cross product
	{
		return vec3(y*vect.z - z*vect.y, z*vect.x - x*vect.z, x*vect.y - y*vect.x);
	}

	vec3 operator*(float a) const
	{
		return vec3( x*a, y*a, z*a );
	}

	const vec3& operator*=(float a)
	{
		x *= a;
		y *= a;
		z *= a;

		return *this;
	}

	float length() const
	{
		return sqrtf(x*x + y*y + z*z);
	}

	float normalize()
	{
		float len = sqrtf(x*x + y*y + z*z);

		if(len > math::eps)
		{
			x /= len;
			y /= len;
			z /= len;
		}

		return len;
	}

	operator const float*() const
	{
		return &x;
	}
	
	operator vec2() const
	{
		return vec2(x, y);
	}

	void Rotate(vec3 & axis, float ang);

	vec3 normalized() const
	{
		float len = sqrtf(x * x + y * y + z * z);

		if (len > math::eps) return vec3(x / len, y / len, z / len);

		return vec3(x, y, z);
	}

	inline static vec3 Orthogonal(const vec3& vec)
	{
		int i, j, k;

		for (i = 0; i < 3; i++)
		{
			if (fabs(vec[i]) > math::eps) break;
		}

		if (i >= 3) return vec3(0);

		switch (i)
		{
		case 0: j = 1; k = 2; break;
		case 1: j = 0; k = 2; break;
		case 2: j = 0; k = 1; break;
		}

		bool jzero = fabs(vec[j]) < math::eps;
		bool kzero = fabs(vec[k]) < math::eps;

		float factor = (jzero || kzero) ? 1.0f : 0.5f;

		vec3 ortho;

		ortho[i] = (jzero && kzero) ? 0.0f : 1.0f;
		ortho[j] = jzero ? 1.0f : -factor * vec[i] / vec[j];
		ortho[k] = kzero ? 1.0f : -factor * vec[i] / vec[k];

		return ortho.normalized();
	}
};