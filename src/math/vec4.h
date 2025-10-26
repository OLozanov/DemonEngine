#pragma once

struct vec4
{
	union
	{
		struct { float x; float y; float z; float w; };
		vec3 xyz;
		vec2 xy;
	};

	constexpr vec4() : x(0), y(0), z(0), w(1) {}
	constexpr vec4(float fx, float fy, float fz, float fw) : x(fx), y(fy), z(fz), w(fw) {}
	constexpr vec4(const vec3 & v) : x(v.x), y(v.y), z(v.z), w(1) {}
	constexpr vec4(const vec3 & v, float a) : x(v.x), y(v.y), z(v.z), w(a) {}

	vec4 operator+(const vec4 & vect) const
	{
		return vec4(x + vect.x, y + vect.y, z + vect.z, w + vect.w);
	}

	const vec4 & operator+=(const vec4 & vect)
	{
		x += vect.x;
		y += vect.y;
		z += vect.z;
		w += vect.w;

		return *this;
	}

	vec4 operator-(const vec4 & vect) const
	{
		return vec4(x - vect.x, y - vect.y, z - vect.z, w - vect.w);
	}

	const vec4 & operator-=(const vec4 & vect)
	{
		x -= vect.x;
		y -= vect.y;
		z -= vect.z;
		w -= vect.w;

		return *this;
	}

	vec4 operator-() const
	{
		return vec4(-x, -y, -z, -w);
	}

	float operator*(const vec4 & vect) const 	//Dot product
	{
		return x*vect.x + y*vect.y + z*vect.z + w*vect.w;
	}

	vec4 operator*(float a) const
	{
		return vec4(x*a, y*a, z*a, w*a);
	}

	const vec4& operator*=(float a)
	{
		x *= a;
		y *= a;
		z *= a;
		w *= a;

		return *this;
	}

	float & operator[](int i)
	{
		return (&x)[i];
	}

	const float & operator[](int i) const
	{
		return (&x)[i];
	}

	float length() const
	{
		return sqrtf(x*x + y*y + z*z + w*w);
	}

	float normalize()
	{
		float len = sqrtf(x*x + y*y + z*z + w*w);

		x /= len;
		y /= len;
		z /= len;
		w /= len;

		return len;
	}

	operator vec3() const
	{
		return vec3(x, y, z);
	}

	vec4 normalized() const
	{
		float len = sqrtf(x * x + y * y + z * z + w * w);

		return vec4(x / len, y / len, z / len, w / len);
	}
};
