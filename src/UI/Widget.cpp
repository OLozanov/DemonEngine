#include "Widget.h"
#include "UI/UiLayer.h"

namespace UI
{

bool Widget::InsidePolygon(const std::vector<vec2>& poly, const vec2& pt)
{
    if (poly.size() < 3) return false;

    int intersects = 0;

    for (size_t i = 0; i < poly.size(); i++)
    {
        size_t k = (i == poly.size() - 1) ? 0 : i + 1;

        if (fabs(poly[i].y - poly[k].y) < math::eps) continue;  // horizontal edge
    
        const vec2& a = (poly[i].y < poly[k].y) ? poly[i] : poly[k];
        const vec2& b = (poly[i].y < poly[k].y) ? poly[k] : poly[i];

        // TODO: this is tricky comparison. Consider to use fixed point coords.
        if (pt.y <= a.y || pt.y > b.y) continue;

        vec2 edge = b - a;
        vec2 pdir = pt - a;

        if ((edge ^ pdir) > 0.0f) intersects++;
    }

    return (intersects % 2) ? true : false;
}

Widget::Widget()
: m_parent(nullptr)
, m_flags(0)
, m_x(0)
, m_y(0)
, m_width(0)
, m_height(0)
{
    attachToUI(nullptr);
}

Widget::Widget(Widget* parent, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t flags, Alignment alignment)
: m_parent(parent)
, m_flags(flags)
, m_alignment(alignment)
, m_x(x)
, m_y(y)
, m_width(width)
, m_height(height)
, m_redraw(true)
{
    calculateExtent();
    attachToUI(parent);
}

Widget::~Widget()
{
    detachFromUI();

    auto child = m_children.begin();
    while (child != m_children.end())
    {
        auto cnext = ++child;

        m_children.remove(*child);
        delete(*child);

        child = cnext;
    }
}

void Widget::calculateExtent()
{
    switch (m_alignment)
    {
    case Alignment::TopLeft:
        m_left = 0;
        m_top = 0;
        m_right = m_width;
        m_bottom = m_height;
    break;

    case Alignment::Left:
        m_left = 0;
        m_top = -m_height / 2;
        m_right = m_width;
        m_bottom = m_height / 2;
    break;

    case Alignment::Right:
        m_left = -m_width;
        m_top = -m_height / 2;
        m_right = 0;
        m_bottom = m_height / 2;
    break;

    case Alignment::Top:
        m_left = -m_width / 2;
        m_top = 0;
        m_right = m_width / 2;
        m_bottom = m_height;
        break;

    case Alignment::Bottom:
        m_left = -m_width / 2;
        m_top = -m_height;
        m_right = m_width / 2;
        m_bottom = 0;
    break;

    case Alignment::Center:
        m_left = -m_width / 2;
        m_top = -m_height / 2;
        m_right = m_width / 2;
        m_bottom = m_height / 2;
    break;
    }
}

void Widget::attachToUI(Widget* parent)
{
    if(!parent) UiLayer::GetInstance().attach(this);
    else parent->addChild(this);
}

void Widget::detachFromUI()
{
    if(!m_parent) UiLayer::GetInstance().detach(this);
    else m_parent->removeChild(this);
}

void Widget::addChild(Widget* child)
{
    m_children.append(child);
}

void Widget::removeChild(Widget* child)
{
    m_children.remove(child);
}

bool Widget::testBorder(int16_t& x, int16_t& y)
{
    short localX = x - m_x;
    short localY = y - m_y;

    if (localX < m_left) return false;
    if (localX > m_right) return false;
    if (localY < m_top) return false;
    if (localY > m_bottom) return false;

    return m_border.empty() ? true : InsidePolygon(m_border, vec2(localX, localY));
}

void Widget::screenPos(int16_t& x, int16_t& y)
{
    x = m_x;
    y = m_y;

    Widget* parent = m_parent;

    while (parent)
    {
        x += parent->m_x;
        y += parent->m_y;

        parent = parent->m_parent;
    }
}

void Widget::show()
{
    m_flags |= flag_visible;
}

void Widget::hide()
{
    m_flags &= ~flag_visible;
}

bool Widget::isVisible()
{
    if (m_flags & flag_visible) return true;
    else return false;
}

void Widget::enable()
{
    m_flags |= flag_enabled;
}

void Widget::disable()
{
    m_flags &= ~flag_enabled;
}

bool Widget::isEnabled()
{
    if (m_flags & flag_enabled) return true;
    else return false;
}

void Widget::setFocus()
{
    UiLayer::GetInstance().setFocus(this);
}

void Widget::killFocus()
{
    UiLayer::GetInstance().killFocus(this);
}

void Widget::showCaret(short x, short y, short height)
{
    UiLayer& ui = UiLayer::GetInstance();

    ui.setCaretState(this, height, true);
    ui.setCaretPos(this, x, y);
}

void Widget::setCaretPos(short x, short y)
{
    UiLayer::GetInstance().setCaretPos(this, x, y);
}

void Widget::hideCaret()
{
    UiLayer::GetInstance().setCaretState(this, 0, false);
}

void Widget::move(int16_t x, int16_t y)
{
    m_x = x;
    m_y = y;
}

void Widget::resize(uint16_t width, uint16_t height)
{
    m_width = width;
    m_height = height;

    calculateExtent();
    onResize();
}

void Widget::refresh() 
{ 
    m_redraw = true; 

    for (Widget* child : m_children) child->refresh();
}

void Widget::display()
{

}

} //namespace UI