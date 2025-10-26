#include "Button.h"
#include "Game/UI/Widgets/Style.h"

namespace GameLogic
{
namespace Widgets
{
    
Button::Button(UI::Widget* parent, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const std::string& caption)
    : UI::Widget(parent, x, y, width, height, flag_visible, UI::Widget::Alignment::Center)
    , m_caption(caption)
{
    m_polygon = { { float(m_left) + width * 0.05f, float(m_top) },
                  { float(m_left) + width * 0.9f, float(m_top) },
                  { float(m_right), float(m_top) + height * 0.65f },
                  { float(m_right), float(m_bottom) },
                  { float(m_left) + width * 0.1f, float(m_bottom) },
                  { float(m_left), float(m_top) + height * 0.4f } };

    setBorder(m_polygon);

    m_canvas.setFont("Tahoma");
    m_canvas.setTextColor(Style::WidgetTextColor);
    m_canvas.setFillColor(Style::WidgetColor);

    m_captionWidth = m_canvas.getFont()->textWidth(m_caption);
}

void Button::display()
{
    m_canvas.polygon(m_polygon);
    m_canvas.text(-m_captionWidth * 0.5f, -8, m_caption);
}

void Button::onMouseButtonUp(int button, short x, short y)
{
    if (button == 0) OnClick();
}

void Button::onMouseOver()
{
    if (!isEnabled()) return;

    m_canvas.setTextColor(Style::WidgetTextColorHighlight);
    refresh();
}

void Button::onMouseLeave()
{
    if (!isEnabled()) return;

    m_canvas.setTextColor(Style::WidgetTextColor);
    refresh();
}

} // namespace Widgets
} // namespace GameLogic