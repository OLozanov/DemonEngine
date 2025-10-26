#include "CheckBox.h"
#include "Game/UI/Widgets/Style.h"

namespace GameLogic
{
namespace Widgets
{

const vec2 CheckBox::BorderPoly[] = { { -4.0f, -6.0f },
                                      { 6.0f,  -6.0f },
                                      { 6.0f,  4.0f },
                                      { 4.0f,  6.0f },
                                      { -6.0f, 6.0f },
                                      { -6.0f, -4.0f } };

CheckBox::CheckBox(UI::Widget* parent, uint16_t x, uint16_t y, bool checked)
: UI::Widget(parent, x, y, 16, 16, flag_visible | flag_enabled, UI::Widget::Alignment::Center)
, m_checked(checked)
{
    m_canvas.setLineColor(Style::WidgetLineColor);
    m_canvas.setFillColor(Style::WidgetBgColor);

    m_canvas.setLineSize(0.5f);
    m_canvas.setSmoothEdgeSize(1.0f);
}

void CheckBox::onMouseButtonUp(int button, short x, short y)
{
    if (button == mb_left)
    {
        m_checked = !m_checked;
        refresh();

        OnChange(m_checked);
    }
}

void CheckBox::onMouseOver()
{
    if (!isEnabled()) return;

    m_over = true;
    refresh();
}

void CheckBox::onMouseLeave()
{
    if (!isEnabled()) return;

    m_over = false;
    refresh();
}

void CheckBox::display()
{
    if (m_over)
        m_canvas.setFillColor(Style::WidgetBgColorHighlight);
    else
        m_canvas.setFillColor(Style::WidgetBgColor);

    m_canvas.polygon(BorderPoly, _countof(BorderPoly), true);

    if (m_checked)
    {
        m_canvas.setFillColor(Style::WidgetLineColor);
        m_canvas.rectangle(-3, -3, 3, 3);
    }
}

} // namespace Widgets
} // namespace GameLogic