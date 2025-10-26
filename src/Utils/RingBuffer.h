#pragma once

#include <iostream>

template<class T>
class RingBuffer
{
    T * m_buffer;

    size_t m_size;
    size_t m_first;
    size_t m_last;

    const T& at(size_t ptr) const { return m_buffer[ptr]; }

    size_t advanceForward(size_t ptr) const
    {
        return (ptr + 1 == m_size) ? 0 : ptr + 1;
    }

    size_t advanceBackward(size_t ptr) const
    {
        return (ptr == 0) ? m_size - 1 : ptr - 1;
    }

public:

    class Iterator
    {
        const RingBuffer& m_buffer;
        size_t m_ptr;

    public:
        
        Iterator(const RingBuffer& buffer, size_t ptr) : m_buffer(buffer), m_ptr(ptr) {}

        const T& operator *() { return m_buffer.at(m_ptr); }

        Iterator& operator=(const Iterator& it) { m_ptr = it.m_ptr; return *this; }

        Iterator next() { return Iterator(m_buffer, m_buffer.advanceForward(m_ptr)); }
        Iterator prev() { return Iterator(m_buffer, m_buffer.advanceBackward(m_ptr)); }

        Iterator& operator++() { m_ptr = m_buffer.advanceForward(m_ptr); return *this; }
        Iterator operator++(int) { Iterator it(*this); m_ptr = m_buffer.advanceForward(m_ptr); return it; }

        Iterator& operator--() { m_ptr = m_buffer.advanceBackward(m_ptr); return *this; }
        Iterator operator--(int) { Iterator it(*this); m_ptr = m_buffer.advanceBackward(m_ptr); return it; }

        bool operator==(const Iterator& it) const { return it.m_ptr == m_ptr; }
        bool operator!=(const Iterator& it) const { return !(it.m_ptr == m_ptr); }
    };

    explicit RingBuffer(size_t num = 5)
    : m_size(num)
    , m_first(0)
    , m_last(0)
    {
        m_buffer = new T[m_size];
    }

    ~RingBuffer()
    {
        delete [] m_buffer;
    }

    void resize(size_t size)
    {
        if (size == m_size) return;

        T* newBuffer = new T[size];

        if (size < m_size)
        {
            size_t i = m_last;
            size_t k = size;

            while (i != m_first && k != 1)
            {
                if (i == 0) i = m_size;
                newBuffer[--k] = m_buffer[--i];
            }

            m_first = k;
            m_last = 0;
        }
        else
        {
            size_t i = m_first;
            size_t k = 0;

            while (i != m_last)
            {
                newBuffer[k++] = m_buffer[i++];
                if (i == m_size) i = 0;
            }

            m_first = 0;
            m_last = k;
        }

        delete [] m_buffer;
        m_buffer = newBuffer;

        m_size = size;
    }

    size_t num()
    {
        if (m_last >= m_first) return m_last - m_first;
        else return (m_size - m_first) + m_last;
    }

    void push(const T& obj)
    {
        m_buffer[m_last] = obj;
        m_last++;

        if (m_last == m_size) m_last = 0;
        if (m_last == m_first)
        {
            m_first++;
            if (m_first == m_size) m_first = 0;
        }
    }

    void pop()
    {
        if (m_first == m_last) return;

        m_first++;
        if (m_first == m_size) m_first = 0;
    }

    Iterator begin() const { return { *this, m_first }; }
    Iterator end() const { return { *this, m_last }; }

    friend class RingBuffer::Iterator;
};