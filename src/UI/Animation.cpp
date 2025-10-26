#include "Animation.h"
#include "UI/UiLayer.h"

namespace UI
{

Animation::Animation(Widget* widget, bool running)
: m_widget(widget)
, m_active(running)
{
    UiLayer::GetInstance().addAnimation(this);
}

Animation::~Animation()
{
    UiLayer::GetInstance().removeAnimation(this);
}

FadeInAnimation::FadeInAnimation(Widget* widget, float& value, float speed)
: Animation(widget)
, m_value(value)
, m_speed(fabs(speed))
{
}

void FadeInAnimation::run(float startValue, float endValue)
{
    m_start = startValue;
    m_end = endValue;

    m_value = startValue;

    Animation::run();
}

void FadeInAnimation::update(float dt)
{
    m_value += m_speed * dt;

    if (m_value >= m_end)
    {
        m_value = m_end;
        stop();

        OnFinished();
    }
}

FadeOutAnimation::FadeOutAnimation(Widget* widget, float& value, float speed)
: Animation(widget)
, m_value(value)
, m_speed(fabs(speed))
{
}

void FadeOutAnimation::run(float startValue, float endValue)
{
    m_start = startValue;
    m_end = endValue;

    m_value = startValue;

    Animation::run();
}

void FadeOutAnimation::update(float dt)
{
    m_value -= m_speed * dt;

    if (m_value <= m_end)
    {
        m_value = m_end;
        stop();

        OnFinished();
    }
}

} //namespace UI