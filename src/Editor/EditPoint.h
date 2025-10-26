#pragma once

#include "math/math3d.h"

template<class T>
struct EditPoint : public ListNode<EditPoint<T>>
{
    static constexpr float PickDist = 0.015;

    vec3* point;
    vec3 diff;

    T* owner;

    EditPoint(vec3* point, T* owner)
    : point(point)
    , owner(owner)
    {
    }

    void rotateX(float Cos, float Sin, const vec3& center)
    {
        float y = Cos * diff.y + Sin * diff.z;
        float z = -Sin * diff.y + Cos * diff.z;

        const vec3& pos = owner->pos();

        point->y = center.y + y - pos.y;
        point->z = center.z + z - pos.z;
    }

    void rotateY(float Cos, float Sin, const vec3& center)
    {
        float x = Cos * diff.x - Sin * diff.z;
        float z = Sin * diff.x + Cos * diff.z;

        const vec3& pos = owner->pos();

        point->x = center.x + x - pos.x;
        point->z = center.z + z - pos.z;
    }

    void rotateZ(float Cos, float Sin, const vec3& center)
    {
        float x = Cos * diff.x + Sin * diff.y;
        float y = -Sin * diff.x + Cos * diff.y;

        const vec3& pos = owner->pos();

        point->x = center.x + x - pos.x;
        point->y = center.y + y - pos.y;
    }

    void scale(const vec3& center, const vec3& scale)
    {
        vec3 scaledDiff = { diff.x * scale.x, diff.y * scale.y, diff.z * scale.z };
        *point = center + scaledDiff - owner->pos();
    }
};