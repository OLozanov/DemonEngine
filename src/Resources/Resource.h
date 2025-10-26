#pragma once

class Resource
{
protected:
    size_t m_refcount;

public:
    Resource() : m_refcount(0) {}

    void addReference() { m_refcount++; }
    void release() { if(m_refcount) m_refcount--; }

    size_t referenceCount() { return m_refcount; }
};

template<class T>
class ResourcePtr
{
    T * m_ptr;
public:

    ResourcePtr() : m_ptr(nullptr) {}
    ResourcePtr(T * resource) : m_ptr(resource) { if(m_ptr) m_ptr->addReference(); }
    //ResourcePtr(const T * resource) : m_ptr(const_cast<T*>(resource)) { if (m_ptr) m_ptr->addReference(); }

    ~ResourcePtr() { if(m_ptr) m_ptr->release(); }

    ResourcePtr(const ResourcePtr<T>& ptr) : m_ptr(ptr.m_ptr) { if (m_ptr) m_ptr->addReference(); }
    ResourcePtr(ResourcePtr&& ptr) noexcept : m_ptr(ptr.m_ptr) { ptr.m_ptr = nullptr; }

    T * operator->() { return m_ptr; }
    T & operator*() { return *m_ptr; }

    const T* operator->() const { return m_ptr; }
    const T& operator*() const { return *m_ptr; }

    bool operator==(T* ptr) const { return m_ptr == ptr; }
    bool operator==(const T* ptr) const { return m_ptr == ptr; }

    bool operator!=(T* ptr) const { return m_ptr != ptr; }
    bool operator!=(const T* ptr) const { return m_ptr != ptr; }

    operator T*() { return m_ptr; }
    operator const T* () const { return m_ptr; }

    T* get() { return m_ptr; }
    T* get() const { return m_ptr; }

    ResourcePtr & operator=(T* ptr)
    {
        if (m_ptr) m_ptr->release();
        m_ptr = ptr;
        if(m_ptr) m_ptr->addReference();

        return *this;
    }

    ResourcePtr & operator=(ResourcePtr & ptr)
    {
        if (m_ptr) m_ptr->release();
        m_ptr = ptr.m_ptr;
        if (m_ptr) m_ptr->addReference();

        return *this;
    }

    ResourcePtr& operator=(const ResourcePtr& ptr)
    {
        if (m_ptr) m_ptr->release();
        m_ptr = ptr.m_ptr;
        if (m_ptr) m_ptr->addReference();

        return *this;
    }

    ResourcePtr& operator=(ResourcePtr&& ptr) noexcept
    {
        if (m_ptr) m_ptr->release();
        m_ptr = ptr.m_ptr;
        ptr.m_ptr = nullptr;

        return *this;
    }

    operator bool() const { return m_ptr != nullptr; }

    void reset(T * resource)
    {
        if(m_ptr) m_ptr->release();
        m_ptr = resource;
        if (m_ptr) m_ptr->addReference();
    }
};
