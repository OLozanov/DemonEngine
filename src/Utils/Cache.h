#pragma once

template<class T, size_t S>
class Cache
{
    T* m_objects[S];
    size_t m_pool[S];
};