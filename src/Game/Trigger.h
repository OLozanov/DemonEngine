#pragma once

#include "Utils/EventHandler.h"
#include "math/math3d.h"

namespace GameLogic
{

class Trigger
{
public:
    using TriggerEvent = Event<void()>;

    vec3 m_pos;
    vec3 m_bbox;

    bool m_triggered;

public:
    Trigger(float size, const vec3& pos);
    Trigger(const vec3& bbox, const vec3& pos);
    Trigger(const BBox& bbox, const vec3& pos);
    ~Trigger() {}

    Trigger(const Trigger&) = default;
    Trigger& operator=(const Trigger&) = default;

    // Need move semantics for std::vector reallocations
    // otherwise its unlikely use scenario
    Trigger(Trigger&& trigger) noexcept = default;
    Trigger& operator=(Trigger&& trigger) noexcept = default;

    void test(const vec3& pos, const vec3& bbox);

    TriggerEvent OnTrigger;
    TriggerEvent OnRelease;
};

} // namespace gamelogic