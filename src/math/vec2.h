#pragma once

struct vec2
{
	float x;
	float y;

	constexpr vec2() :x(0), y(0) {}

	constexpr vec2(float fx, float fy) : x(fx), y(fy) {}

	float & operator[](int i)
	{
		return(&x)[i];
	}

	const float & operator[](int i) const
	{
		return(&x)[i];
	}

	vec2 operator+(const vec2 & vect) const
	{
		return vec2(x + vect.x, y + vect.y);
	}

	const vec2 & operator+=(const vec2 & vect)
	{
		x += vect.x;
		y += vect.y;

		return *this;
	}

	vec2 operator-(const vec2 & vect) const
	{
		return vec2(x - vect.x, y - vect.y);
	}

	const vec2 & operator-=(const vec2 & vect)
	{
		x -= vect.x;
		y -= vect.y;

		return *this;
	}

	vec2 operator-() const
	{
		return vec2(-x, -y);
	}

	float operator*(vec2 & vect) const	//Dot product
	{
		return x*vect.x + y*vect.y;
	}

	float operator^(vec2 & vect) const	//Cross product
	{
		return x*vect.y - y*vect.x;
	}

	vec2 operator*(float a) const
	{
		return vec2(x*a, y*a);
	}
	
	const vec2& operator*=(float a)
	{
		x *= a;
		y *= a;
		
		return *this;
	}

	float length() const
	{
		return sqrtf(x*x + y*y);
	}

	float normalize()
	{
		float len = sqrtf(x*x + y*y);

		x /= len;
		y /= len;

		return len;
	}

	vec2 normalized() const
	{
		float len = sqrtf(x * x + y * y);

		return vec2(x / len, y / len);
	}
};
