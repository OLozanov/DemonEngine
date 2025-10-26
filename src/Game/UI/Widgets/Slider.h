#pragma once

#include "UI/Widget.h"
#include "Utils/EventHandler.h"

namespace GameLogic
{
namespace Widgets
{

class Slider : public UI::Widget
{
public:
    using OnChangeEvent = Event<void()>;

public:
    Slider(UI::Widget* parent, uint16_t x, uint16_t y, uint16_t width, Alignment alignment = Alignment::Center);
    ~Slider() {}

    void setPosition(float pos);
    float position();

    OnChangeEvent OnChange;

private:
    void onMouseButtonDown(int button, short x, short y) override;
    void onMouseButton(int button, bool down);
    void onCursorMoved(short x, short y);
    void onMouseOver() override;
    void onMouseLeave() override;

    void display() override;

private:
    uint16_t m_range;
    uint16_t m_position;

    short m_screenx;

    bool m_move;

    SubscriptionHandle m_mouseButtonSubscription;
    SubscriptionHandle m_cursorMoveSubscription;

    std::vector<vec2> m_leftPoly;
    std::vector<vec2> m_rightPoly;

    vec2 m_bar[2];

    static constexpr uint16_t MoverWidth = 3;
};

} // namespace Widgets
} // namespace GameLogic
