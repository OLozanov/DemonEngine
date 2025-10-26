#pragma once

#include "Utils/EventHandler.h"
#include <list>

class Timer
{
private:
    static std::list<Timer*> Timers;

    float m_period;
    float m_time;

    bool m_active;
    bool m_once;

    void update(float dt);

public:

    Timer();
    ~Timer();

    void start(float period, bool do_once = false);
    void stop();

    Event<void ()> OnFire;

    static void UpdateTimers(float dt);
};
