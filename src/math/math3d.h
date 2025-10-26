#pragma once

#include "Math.h"

#include <initializer_list>

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

#include "mat3.h"
#include "mat4.h"

#include "BBox.h"

#include "constants.h"

inline float sgn(float val)
{
	if(fabs(val) < math::eps) return 0.0f;

	return val > 0 ? 1.0f : -1.0f;
}

template<class M>
vec3 ToEuler(const M& mat)
{
	vec3 euler;

	euler.y = -asin(mat[0][2]);

	float cosY = sqrt(1 - mat[0][2] * mat[0][2]);

	if (fabs(cosY) > math::eps)
	{
		euler.z = atan2(mat[0][1] / cosY, mat[0][0] / cosY);
		euler.x = atan2(mat[1][2] / cosY, mat[2][2] / cosY);
	}
	else
	{
	    euler.x = atan2(mat[1][0], mat[1][1]); // X - Z
		euler.z = 0;
	}

	return euler;
}