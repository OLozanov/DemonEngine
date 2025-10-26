#include "Slider.h"
#include "UI/UiLayer.h"

namespace GameLogic
{
namespace Widgets
{

Slider::Slider(UI::Widget* parent, uint16_t x, uint16_t y, uint16_t width, Alignment alignment)
: UI::Widget(parent, x, y, width, 10, flag_visible | flag_enabled, alignment)
, m_range(width - 16 - MoverWidth * 2)
, m_position(m_range / 2)
, m_move(false)
{
    //m_canvas.setLineColor(0.0f, 0.2f, 0.5f);
    m_canvas.setLineColor(1.0f, 0.99f, 0.79f);
    m_canvas.setFillColor(0.0f, 0.2f, 0.5f);

    float mid = (m_bottom + m_top) * 0.5;

    m_bar[0] = { float(m_left) + 5, mid };
    m_bar[1] = { float(m_right) - 5, mid };

    m_leftPoly = { {float(m_left) + 5.0f, float(m_top)},
                   {float(m_left) + 8.0f, float(m_top)},
                   {float(m_left) + 8.0f, float(m_bottom)},
                   {float(m_left) + 5.0f, float(m_bottom)},
                   {float(m_left), m_top + 6.0f},
                   {float(m_left), m_top + 4.0f} };

    m_rightPoly = { {float(m_right) - 5.0f, float(m_top)},
                    {float(m_right) - 8.0f, float(m_top)},
                    {float(m_right) - 8.0f, float(m_bottom)},
                    {float(m_right) - 5.0f, float(m_bottom)},
                    {float(m_right), m_top + 6.0f},
                    {float(m_right), m_top + 4.0f} };

    m_mouseButtonSubscription = UI::UiLayer::GetInstance().OnMouseClick.subscribe(this, &Slider::onMouseButton);
    m_cursorMoveSubscription = UI::UiLayer::GetInstance().OnCursorMoved.subscribe(this, &Slider::onCursorMoved);
}

void Slider::setPosition(float pos)
{
    m_position = m_range * pos;
    refresh();
}

float Slider::position()
{
    return float(m_position) / m_range;
}

void Slider::onMouseButtonDown(int button, short x, short y)
{
    if (button == mb_left)
    {
        uint16_t mpos = 8 + MoverWidth + m_position;
        if (abs(mpos - x) <= MoverWidth)
        {
            short screeny;
            screenPos(m_screenx, screeny);

            m_move = true;
        }
        else if (x > mpos)
        {
            m_position = std::max(0, std::min(int(m_range), m_position + 5));
            refresh();
        }
        else if (x < mpos)
        {
            m_position = std::max(0, std::min(int(m_range), m_position - 5));
            refresh();
        }
    }
}

void Slider::onMouseButton(int button, bool down)
{
    if (button == mb_left && !down && m_move)
    {
        m_move = false;
        OnChange();
    }
}

void Slider::onCursorMoved(short x, short y)
{
    if (!m_move) return;

    m_position = std::max(0, std::min(int(m_range), x - m_screenx - m_left - 8 - MoverWidth));
    refresh();
}

void Slider::onMouseOver()
{

}

void Slider::onMouseLeave()
{
    //m_move = false;
}

void Slider::display()
{
    m_canvas.setSmoothEdgeSize(2.0f);
    m_canvas.line(m_bar, 2);

    m_canvas.setSmoothEdgeSize(1.0f);
    m_canvas.polygon(m_leftPoly);
    m_canvas.polygon(m_rightPoly);

    uint16_t pos = 8 + MoverWidth + m_position;

    m_canvas.rectangle(pos - MoverWidth, m_top, pos + MoverWidth, m_bottom);
}

} // namespace Widgets
} // namespace GameLogic