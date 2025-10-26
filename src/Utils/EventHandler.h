#pragma once

#include <tuple>
#include "Utils\AsyncCall.h"

#include <list>

template <typename T> class Event;
template <typename T> class AsyncEvent;

template <typename T> class Notification;

template <typename ... Arg>
class IEventHandler
{
public:

    virtual ~IEventHandler() {}
    virtual void invoke(Arg ... args) = 0;
};

template <typename T, typename ... Arg>
class ObjectEventHandler : public IEventHandler<Arg ...>
{
    T m_handler;

public:

    ObjectEventHandler(const T & obj)
    : m_handler(obj)
    {}

    ~ObjectEventHandler() {}

    void invoke(Arg ... args) override
    {
        m_handler(args ...);
    }
};

template <typename T, typename ... Arg>
class ObjectEventAsyncHandler : public IEventHandler<Arg ...>
{
    AsyncCallQueue& m_callQueue;
    T m_handler;

public:

    ObjectEventAsyncHandler(AsyncCallQueue& callQueue, const T & obj)
    : m_callQueue(callQueue)
    , m_handler(obj)
    {}

    ~ObjectEventAsyncHandler() {}

    void invoke(Arg ... args) override
    {
        m_callQueue.invoke(m_handler, args ...);
    }
};

template <typename T, typename ... Arg>
class MethodEventHandler : public IEventHandler<Arg ...>
{
    typedef void (T::*MethodPtr)(Arg ... args);

    T * m_receiver;
    MethodPtr m_handler;

public:

    MethodEventHandler(T * obj, MethodPtr func)
    : m_receiver(obj)
    , m_handler(func)
    {}

    ~MethodEventHandler() {}

    void invoke(Arg ... args) override
    {
        (m_receiver->*m_handler)(args ...);
    }
};

template <typename T, typename ... Arg>
class MethodEventAsyncHandler : public IEventHandler<Arg ...>
{
    typedef void (T::*MethodPtr)(Arg ... args);

    AsyncCallQueue& m_callQueue;
    T * m_receiver;
    MethodPtr m_handler;

public:

    MethodEventAsyncHandler(AsyncCallQueue& callQueue, T * obj, MethodPtr func)
    : m_callQueue(callQueue)
    , m_receiver(obj)
    , m_handler(func)
    {}

    ~MethodEventAsyncHandler() {}

    void invoke(Arg ... args) override
    {
        m_callQueue.invoke(m_receiver, m_handler, args ...);
    }
};

template <typename ... Arg>
class Event<void (Arg...)>
{
    IEventHandler<Arg...> * m_handler;

public:
    Event() : m_handler(nullptr) {}
    ~Event() { if(m_handler) delete m_handler; }

    Event(const Event&) = default;
    Event& operator=(const Event&) = default;

    Event(Event&& event) noexcept
    {
        m_handler = event.m_handler;
        event.m_handler = nullptr;
    }

    Event& operator=(Event&& event) noexcept 
    { 
        m_handler = event.m_handler;
        event.m_handler = nullptr;

        return *this;
    }

    void operator()(Arg ... args)
    {
        if(m_handler) m_handler->invoke(args ...);
    }

    template <typename T>
    void bind(const T & obj)
    {
        if(m_handler) delete m_handler;

        m_handler = new ObjectEventHandler<T, Arg...>(obj);
    }

    template <typename T>
    void bind_async(AsyncCallQueue& callQueue, const T & obj)
    {
        if(m_handler) delete m_handler;

        m_handler = new ObjectEventAsyncHandler<T, Arg...>(callQueue, obj);
    }

    template <typename T>
    void bind(T * obj, void (T::*method)(Arg...))
    {
        if(m_handler) delete m_handler;

        m_handler = new MethodEventHandler<T, Arg...>(obj, method);
    }

    template <typename T>
    void bind_async(AsyncCallQueue& callQueue, T * obj, void (T::*method)(Arg...))
    {
        if(m_handler) delete m_handler;

        m_handler = new MethodEventAsyncHandler<T, Arg...>(callQueue, obj, method);
    }
};

using SubscriptionHandle = std::list<void*>::iterator;

template <typename ... Arg>
class Notification<void(Arg...)>
{
    using HandlerType = IEventHandler<Arg...>*;
    using ListType = std::list<void*>;

private:
    ListType m_subscriptions;

public:
    Notification() = default;

    ~Notification()
    {
        for (void* handler : m_subscriptions) delete reinterpret_cast<HandlerType>(handler);
    }

    Notification(const Notification&) = delete;
    Notification& operator=(const Notification&) = delete;
    Notification(Notification&& event) = delete;
    Notification& operator=(Notification&& event) = delete;

    void operator()(Arg ... args)
    {
        if (m_subscriptions.empty()) return;

        // Handler can potentially unsubscribe itself
        for (auto it = m_subscriptions.begin(); it != m_subscriptions.end(); )
        {
            auto next = std::next(it);
            reinterpret_cast<HandlerType>(*it)->invoke(args ...);
        
            it = next;
        }
    }

    template <typename T>
    SubscriptionHandle subscribe(const T& obj)
    {
        HandlerType handler = new ObjectEventHandler<T, Arg...>(obj);

        m_subscriptions.push_back(handler);
        return std::prev(m_subscriptions.end());
    }

    template <typename T>
    SubscriptionHandle subscribe_async(AsyncCallQueue& callQueue, const T& obj)
    {
        HandlerType handler = new ObjectEventAsyncHandler<T, Arg...>(callQueue, obj);

        m_subscriptions.push_back(handler);
        return std::prev(m_subscriptions.end());
    }

    template <typename T>
    SubscriptionHandle subscribe(T* obj, void (T::* method)(Arg...))
    {
        HandlerType handler = new MethodEventHandler<T, Arg...>(obj, method);

        m_subscriptions.push_back(handler);
        return std::prev(m_subscriptions.end());
    }

    template <typename T>
    SubscriptionHandle subscribe_async(AsyncCallQueue& callQueue, T* obj, void (T::* method)(Arg...))
    {
        HandlerType handler = new MethodEventAsyncHandler<T, Arg...>(callQueue, obj, method);

        m_subscriptions.push_back(handler);
        return std::prev(m_subscriptions.end());
    }

    void unsubscribe(SubscriptionHandle subscription)
    {
        m_subscriptions.erase(subscription);
    }
};
