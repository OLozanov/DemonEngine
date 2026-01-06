#include "Label.h"
#include "Game/UI/Widgets/Style.h"

namespace GameLogic
{
namespace Widgets
{

Label::Label(UI::Widget* parent, uint16_t x, uint16_t y, uint16_t width, uint16_t height, Alignment alignment)
: UI::Widget(parent, x, y, width, height, flag_visible, alignment)
, m_text({})
{
    m_canvas.setTextColor(0.15, 0.15, 0.15);
    m_canvas.setFont(Style::CaptionFont);
}

Label::Label(UI::Widget* parent, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const std::string& font, Alignment alignment)
: UI::Widget(parent, x, y, width, height, flag_visible, alignment)
, m_text({})
{
    m_canvas.setTextColor(0.15, 0.15, 0.15);
    m_canvas.setFont(font);
}

void Label::setColor(float r, float g, float b, float a)
{
    m_canvas.setTextColor(r, g, b, a);
}

void Label::setColor(const vec3& color)
{
    m_canvas.setTextColor(color.x, color.y, color.z, 1.0f);
}

void Label::setColor(const vec4& color)
{
    m_canvas.setTextColor(color.x, color.y, color.z, color.w);
}

short Label::setText(const char * text)
{
    m_text = text;

    resize(m_canvas.getFont()->textWidth(m_text), m_height);
    refresh();

    return m_width;
}

void Label::display()
{
    m_canvas.text(m_left + 2, m_top + 1, m_text);
}

IconLabel::IconLabel(UI::Widget* parent, uint16_t x, uint16_t y, uint16_t width, uint16_t height, Image* img, Alignment alignment)
: UI::Widget(parent, x, y, width, height, flag_visible, alignment)
, m_image(img)
{
    m_canvas.setFillColor(1.0, 1.0, 1.0, 1.0);
}

IconLabel::IconLabel(UI::Widget* parent, uint16_t x, uint16_t y, Image* img, Alignment alignment)
: UI::Widget(parent, x, y, img->width, img->height, flag_visible, alignment)
, m_image(img)
{
    m_canvas.setFillColor(1.0, 1.0, 1.0, 1.0);
}

void IconLabel::setColor(float r, float b, float g, float a)
{
    m_canvas.setFillColor(r, g, b, a);
}

void IconLabel::setColor(const vec3& color)
{
    m_canvas.setFillColor(color.x, color.y, color.z, 1.0f);
}

void IconLabel::setColor(const vec4& color)
{
    m_canvas.setFillColor(color.x, color.y, color.z, color.w);
}

void IconLabel::display()
{
    m_canvas.image(m_image, m_left, m_top, m_right, m_bottom);
}

} // namespace Widgets
} // namespace GameLogic