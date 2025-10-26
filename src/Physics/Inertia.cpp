#include "Inertia.h"

namespace Physics
{

vec3 BoxInertiaTensor(const vec3& box, float mass)
{
    float m = (1.0/12.0)*mass;

    return { m*(box.y*box.y + box.z*box.z), 
             m*(box.x*box.x + box.z*box.z),
             m*(box.x*box.x + box.y*box.y) };
}

vec3 SphereInertiaTensor(float radius, float mass)
{
    float m = (2.0/5.0)*mass*radius*radius;

    return { m, m, m };
}

} //namespace physics