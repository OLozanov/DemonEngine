#include "System/Timer.h"

std::list<Timer*> Timer::Timers;

Timer::Timer()
: m_active(false)
{
}

Timer::~Timer()
{
    if(m_active) Timers.remove(this);
}

void Timer::start(float period, bool do_once)
{
    m_period = period;
    m_time = period;

    m_once = do_once;

    if(!m_active)
    {
        m_active = true;
        Timers.push_back(this);
    }
}

void Timer::stop()
{
    if(m_active)
    {
        Timers.remove(this);
        m_active = false;
    }
}

void Timer::update(float dt)
{
    if(m_time > 0)
    {
        m_time -= dt;
        if(m_time <= 0)
        {
            OnFire();

            if(m_once) m_active = false;
            else m_time = m_period - m_time;
        }
    }
}

void Timer::UpdateTimers(float dt)
{
    auto it = Timers.begin();

    while(it != Timers.end())
    {
        auto timer = it++;

        (*timer)->update(dt);
        if(!(*timer)->m_active) Timers.erase(timer);
    }
}
