#pragma once

#include "math/math3d.h"

namespace Physics
{

vec3 BoxInertiaTensor(const vec3& box, float mass);
vec3 SphereInertiaTensor(float radius, float mass);

} //namespace Physics
