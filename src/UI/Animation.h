#pragma once

#include "Utils/LinkedList.h"
#include "Utils/EventHandler.h"

namespace UI
{

class Widget;

class Animation : public ListNode<Animation>
{
public:
    using OnFinishedEvent = Event<void()>;

public:
    explicit Animation(Widget* widget, bool active = false);
    virtual ~Animation();

    void run() { m_active = true; }
    void stop() { m_active = false; }

    bool isRunning() { return m_active; }

    OnFinishedEvent OnFinished;

private:
    virtual void update(float dt) = 0;

private:
    Widget* m_widget;
    bool m_active;

    friend class UiLayer;
};

class FadeInAnimation : public Animation
{
public:
    FadeInAnimation(Widget* widget, float& value, float speed);

    void run(float startValue, float endValue);

private:
    void update(float dt) override;

private:
    float& m_value;
    float m_start;
    float m_end;

    float m_speed;
};

class FadeOutAnimation : public Animation
{
public:
    FadeOutAnimation(Widget* widget, float& value, float speed);

    void run(float startValue, float endValue);

private:
    void update(float dt) override;

private:
    float& m_value;
    float m_start;
    float m_end;

    float m_speed;
};

class CustomAnimation : public Animation
{
public:
    template <typename T>
    CustomAnimation(Widget* widget, const T& obj)
    : Animation(widget)
    {
        m_handler = new ObjectEventHandler<T, float>(obj);
    }

    template <typename T>
    CustomAnimation(Widget* widget, T* obj, void (T::* method)(float))
    : Animation(widget)
    {
        m_handler = new MethodEventHandler<T, float>(obj, method);
    }

private:
    void update(float dt) override { m_handler->invoke(dt); }

private:
    IEventHandler<float>* m_handler;
};

} //namespace UI