#pragma once

#include <atomic>

class Signal
{
    std::atomic<bool> m_flag = false;

public:
    Signal() = default;

    void wait() noexcept
    {
        while (!m_flag.exchange(false)) ;
    }

    void notify() noexcept { m_flag.store(true); }
};